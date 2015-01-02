#include <algorithm>
#include <map>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <wx/app.h>
#include <wx/filename.h>
#include <Magick++.h>

#include "cmd-line-parser-helper.hpp"
#include "cpercep.hpp"
#include "headerfile.hpp"
#include "fileutils.hpp"
#include "implementationfile.hpp"
#include "logger.hpp"
#include "lutgen.hpp"
#include "scanner.hpp"
#include "shared.hpp"
#include "version.h"

void DoGBAExport(const std::vector<Image32Bpp>& images, const std::vector<Image32Bpp>& tilesets);
void DoDSExport(const std::vector<Image32Bpp>& images, const std::vector<Image32Bpp>& tilesets);
void Do3DSExport(const std::vector<Image32Bpp>& images, const std::vector<Image32Bpp>& tilesets);
void DoLUTExport(const std::vector<LutSpecification>& functions);

class Nin10KitApp : public wxAppConsole
{
    public:
        virtual bool OnInit();
        virtual void OnInitCmdLine(wxCmdLineParser& parser);
        virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
        bool DoExportImages();
        int OnRun();
};

static const wxCmdLineEntryDesc cmd_descriptions[] =
{
    {wxCMD_LINE_SWITCH, "h", "help", "Displays help on command line parameters",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP},

    // Debugging
    {wxCMD_LINE_OPTION, "log", "log", "Debug logging -log=num (0:fatal,1:error,2:warn,3:info,4:verbose) default 3",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},

    // Modes
    {wxCMD_LINE_OPTION, "mode", "mode", "Special Mode String -mode=(one of 0,3,4,tiles,map,sprites,lut)",
        wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "device", "device", "Special Device String (-device=(gba, ds, 3ds))",
        wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "bpp", "bpp", "Bits per pixel only for use with -mode0 or -sprites (Default 8).",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},

    // Lut options
    {wxCMD_LINE_OPTION, "func", "func", "--func=function_name,type,start,end,step[,in_degrees=false] adds lookup table for function",
        wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},

    // General helpful options
    {wxCMD_LINE_OPTION, "output_dir", "output_dir", "output directory for exported files", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "names", "names", "(Usage -names=name1,name2) Renames output array names to names given. If this is used each object/image given must be renamed. "
        "If not given then the file names of the images will be used to generate the name.", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "resize", "resize", "(Usage -resize=wxh,wxh2) Resize images to wxh must be given for all images "
    "(example -resize=10x12,,72x42 first image=10,12, second image is not resized, last image=72x42 )", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "transparent", "transparent",
        "(Usage -transparent=RRGGBB in hex) Makes the color RRGGBB transparent The range of r,g,b is [0,255]. "
        "In mode 3 using this gives you a #define for the transparent color so you can use it to ignore those pixels when drawing. "
        "In mode 4 this color will become palette entry 0 and thus the background color. "
        "And in mode0 and sprites it will also be palette entry 0 so the gba will colorkey the color.",
        wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "dither", "dither",
        "ADVANCED option use at your own risk. (Usage -dither=0 or -dither=1) "
        "Apply dithering.  Dithering makes the image look better by reducing "
        "large areas of one color as a result of reducing the number of colors in the image by adding noise.",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "dither_level", "dither_level",
        "ADVANCED option use at your own risk. (Usage -dither_level=num) "
        "Only applicable if -dither=1.  The range of num is [0,100]. This option affects how strong the "
        "dithering effect is by default it is set to 80.",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},

    // Mode 0/4 options
    {wxCMD_LINE_OPTION, "start", "start",
        "Only for mode4 exports. (Usage -start=X). Starts the palette off at index X. Useful if you "
        "have another image already exported that has X entries.", wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "palette", "palette",
        "Only for mode4 exports. (Usage -palette=X). Will restrict the palette to X entries rather than 256. "
        "Useful when combined with -start.",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "split", "split",
        "Only for mode0/4 exports.  Exports each individual image with its own palette (and tileset).  Useful for sets of screens. "
        "Or videos (this program even supports video formats).",
         wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},

    // Mode 0 exclusive options
    {wxCMD_LINE_OPTION, "split_sbb", "split_sbb", "(Usage -split_sbb=1-4) Given a big map image (>1024,1024) split it into multiple maps."
        " 1 = (32, 32), 2 = (64, 32), 3 = (32, 64), 4 = (64, 64). Image must be divisible by split size * 8 (NOT IMPLEMENTED).",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "tileset", "tileset", "Tileset image(s) to export against when using -map.",
        wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "border", "border", "Border around each tile in tileset image",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "force", "force",
        "For mode 0 4bpp export only. If a problem occurs that could result in a major loss of quality, forces the program to export anyway (NOT IMPLEMENTED YET).",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},

    // Sprite exclusive options
    {wxCMD_LINE_OPTION, "export_2d", "export_2d",
        "For sprites export only. Exports sprites for use in sprite 2d mode (Default false).",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "for_bitmap", "for_bitmap",
        "For sprites export only.  Exports sprites for use in modes 3 and 4 (Default false).",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},

    // Advanced Mode 4 options Use at your own risk.
    {wxCMD_LINE_OPTION, "weights", "weights",
        "Only for mode0/4 exports.  ADVANCED option use at your own risk. (Usage -weights=w1,w2,w3,w4) "
        "Fine tune? median cut algorithm.  w1 = volume, w2 = population, w3 = volume*population, w4 = error "
        "Affects image output quality for mode4, Range of w1-w4 is [0,100] but must sum up to 100 "
        "These values affects which colors are chosen by the median cut algorithm used to reduce the number "
        "of colors in the image.", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},

    {wxCMD_LINE_PARAM,  NULL, NULL, "output filename / input file(s)", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE},
    {wxCMD_LINE_NONE}
};

IMPLEMENT_APP(Nin10KitApp);

// All of the read in command line flags will be in this structure.
ExportParams params;

/** OnInit
  *
  * Initializes the program
  */
bool Nin10KitApp::OnInit()
{
    logger->SetLogLevel(LogLevel::INFO);
    VerboseLog("Init");

    cpercep_init();

    if (argc <= 1)
    {
        wxCmdLineParser parser(argc, argv);
        OnInitCmdLine(parser);
        parser.Usage();
        return false;
    }

    Magick::InitializeMagick(static_cast<const char*>(argv[0].c_str()));

    if (!wxAppConsole::OnInit())
        FatalLog("A problem occurred, please report this and give any images the command line that caused this");

    // Give me the invocation
    std::ostringstream out;
    for (int i = 1; i < argc; i++)
        out << argv[i].ToStdString() << " ";

    header.SetInvocation(out.str());
    implementation.SetInvocation(out.str());

    return true;
}

/** @brief OnInitCmdLine
  *
  * @todo: document this function
  */
void Nin10KitApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    VerboseLog("OnInitCmdLine");
    parser.SetLogo(wxString::Format(_("nin10kit version %s"), AutoVersion::FULLVERSION_STRING));
    parser.SetDesc(cmd_descriptions);
    parser.SetSwitchChars (_("-"));
}

/** @brief OnCmdLineParsed
  *
  * @todo: document this function
  */
bool Nin10KitApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    VerboseLog("OnCmdLineParsed");

    CmdLineParserHelper parse(parser);
    logger->SetLogLevel((LogLevel)parse.GetInt("log", 3, 0, 4));

    // mode params
    params.mode = ToUpper(parse.GetString("mode", ""));
    params.device = ToUpper(parse.GetString("device", "GBA"));
    params.bpp = parse.GetInt("bpp", 8);

    std::string function = parse.GetString("func");

    // Helpful options
    params.output_dir = parse.GetString("output_dir");
    params.names = parse.GetListString("names");
    std::vector<std::string> resizes = parse.GetListString("resize");
    int transparent = parse.GetHexInt("transparent", -1, 0, 0xFFFFFF);
    params.transparent_given = transparent != -1;
    params.transparent_color = Color(transparent);
    params.dither = parse.GetBoolean("dither", true);
    params.dither_level = parse.GetInt("dither_level", 80, 0, 100) / 100.0f;

    params.offset = parse.GetInt("start", 0, 0, 255);
    params.palette = parse.GetInt("palette", 256, 1, 256);
    params.split = parse.GetBoolean("split", false);

    params.split_sbb = parse.GetBoolean("split_sbb", false);
    params.tilesets = parse.GetListString("tileset");
    params.border = parse.GetInt("border", 0, 0);
    params.force = parse.GetBoolean("force", false);

    params.export_2d = parse.GetBoolean("export_2d", false);
    params.for_bitmap = parse.GetBoolean("for_bitmap", false);

    params.weights = parse.GetListInt("weights", {25, 25, 25, 25});

    std::string export_file = parser.GetParam(0).ToStdString();
    params.export_file = Chop(export_file);
    params.filename = params.output_dir.empty() ? export_file : params.output_dir + Chop(export_file);
    params.symbol_base_name = Sanitize(export_file);
    InfoLog("Exporting to %s symbol base name is %s", params.filename.c_str(), params.symbol_base_name.c_str());

    // get unnamed parameters
    std::set<std::string> filenames;
    for (unsigned int i = 1; i < parser.GetParamCount(); i++)
    {
        const std::string filename = parser.GetParam(i).ToStdString();
        if (filenames.find(filename) != filenames.end())
            FatalLog("Duplicate image filename given");
        params.files.push_back(filename);
        filenames.insert(filename);
    }

    if (params.mode.empty())
        FatalLog("No mode set.");

    if (params.mode == "LUT")
    {
        if (!params.files.empty())
            WarnLog("Ignoring files passed in, currently generating look up tables");

        if (function.empty())
            FatalLog("Must specify function to generate look up table for.");

        params.functions.push_back(LutSpecification(function));

        if (params.names.empty())
        {
            for (const auto& func : params.functions)
                params.names.push_back(func.function + "_table");
        }

        if (params.names.size() != params.functions.size())
            FatalLog("Incorrect number of override names given %d != %d, this must be equal to the number of lut functions", params.names.size(), params.functions.size());

        return true;
    }

    if (!params.functions.empty())
        WarnLog("Ignoring functions passed in, currently exporting images");

    if (params.files.empty())
        FatalLog("You must specify an output filename and a list of image files you want to export.");

    if (params.names.empty())
    {
        for (const auto& file : params.files)
            params.names.push_back(Format(file));
    }

    if (params.names.size() != params.files.size())
        FatalLog("Incorrect number of override names given %d != %d, this must be equal to the number of images passed in", params.names.size(), params.files.size());

    if (!resizes.empty() && resizes.size() != params.files.size())
        FatalLog("Incorrect number of resizes given %d != %d, this must be equal to the number of images passed in", resizes.size(), params.files.size());

    for (const auto& resize_str : resizes)
    {
        int w = -1, h = -1;
        if (!resize_str.empty())
        {
            Scanner scan(resize_str, "x");
            if (!scan.Next(w))
                FatalLog("Invalid width given %s", resize_str.c_str());
            if (!scan.Next(h))
                FatalLog("Invalid height given %s", resize_str.c_str());
        }
        params.resizes.push_back(resize(w, h));
    }

    if (params.weights.size() != 4)
        FatalLog("Error parsing -weights expected 4 elements %d given", params.weights.size());
    int p = params.weights[0];
    int v = params.weights[1];
    int pv = params.weights[2];
    int error = params.weights[3];
    if (p < 0 || v < 0 || pv < 0 || error < 0 || (p+v+pv+error != 100))
        WarnLog("-weights total does not sum up to 100 or invalid value given.");

    if (!params.tilesets.empty())
    {
        header.SetTilesets(params.tilesets);
        implementation.SetTilesets(params.tilesets);
    }

    if (params.mode == "tiles")
    {
        if (!params.resizes.empty())
        {
            WarnLog("Ignoring -resize when in tileset export mode, please resize manually.");
            params.resizes.clear();
        }
        header.SetTilesets(params.files);
        implementation.SetTilesets(params.files);
    }

    return true;
}

bool Nin10KitApp::DoExportImages()
{
    VerboseLog("DoLoadImages");
    std::map<std::string, std::vector<Magick::Image>> file_images;
    std::map<std::string, std::vector<Magick::Image>> file_tilesets;
    for (const auto& filename : params.files)
    {
        file_images[filename] = std::vector<Magick::Image>();
        readImages(&file_images[filename], filename);
    }
    for (const auto& tileset : params.tilesets)
    {
        file_tilesets[tileset] = std::vector<Magick::Image>();
        readImages(&file_tilesets[tileset], tileset);
    }

    VerboseLog("DoHandleResize");
    for (unsigned int i = 0; i < params.resizes.size(); i++)
    {
        const std::string& filename = params.files[i];
        const resize& size = params.resizes[i];
        if (!size.IsValid()) continue;
        Magick::Geometry geom(size.width, size.height);
        geom.aspect(true);
        for (auto& image : file_images[filename])
            image.resize(geom);
    }

    VerboseLog("DoCheckAndLabelImages");
    for (unsigned int i = 0; i < params.files.size(); i++)
    {
        const std::string& filename = params.files[i];
        const std::vector<Magick::Image> images = file_images[filename];
        bool isAnim = images.size() > 1;
        for (unsigned int j = 0; j < images.size(); j++)
        {
            const Magick::Image& image = images[j];
            header.AddImageInfo(filename, j, image.columns(), image.rows(), isAnim);
            implementation.AddImageInfo(filename, j, image.columns(), image.rows(), isAnim);
        }
    }

    VerboseLog("Converting to Image32Bpp");
    for (unsigned int i = 0; i < params.files.size(); i++)
    {
        const std::string& filename = params.files[i];
        std::vector<Magick::Image>& images = file_images[filename];
        for (unsigned int j = 0; j < images.size(); j++)
        {
            const auto& image = images[j];
            params.images.push_back(Image32Bpp(image, params.names[i], filename, j, images.size() > 1));
        }
    }

    for (unsigned int i = 0; i < params.tilesets.size(); i++)
    {
        const std::string& filename = params.tilesets[i];
        std::vector<Magick::Image>& images = file_tilesets[filename];
        for (unsigned int j = 0; j < images.size(); j++)
        {
            const auto& image = images[j];
            params.tileset_images.push_back(Image32Bpp(image, params.names[i], filename, j, images.size() > 1));
        }
    }

    VerboseLog("DoExportImages");
    InfoLog("Using %s exporter mode %s", params.device.c_str(), params.mode.c_str());

    header.SetMode(params.mode);
    implementation.SetMode(params.mode);

    if (params.mode == "LUT")
        DoLUTExport(params.functions);
    else if (params.device == "GBA")
        DoGBAExport(params.images, params.tileset_images);
    else if (params.device == "DS")
        DoDSExport(params.images, params.tileset_images);
    else if (params.device == "3DS")
        Do3DSExport(params.images, params.tileset_images);

    InfoLog("Export complete now writing files");
    // Write the files
    std::ofstream file_c, file_h;
    InitFiles(file_c, file_h, params.filename);

    header.Write(file_h);
    implementation.Write(file_c);

    file_h.close();
    file_c.close();

    return true;
}

// Do cool things here
int Nin10KitApp::OnRun()
{
    VerboseLog("OnRun");
    try
    {

        if (!DoExportImages()) return EXIT_FAILURE;

        InfoLog("File exported successfully as %s.c and %s.h", params.filename.c_str(), params.filename.c_str());
    }
    catch(Magick::Exception &error_)
    {
        FatalLog("Exception occurred!: %s\nPlease check the images you are trying to load into the program.", error_.what());
    }
    catch (const std::exception& ex)
    {
        FatalLog("Exception occurred! Reason: %s", ex.what());
    }
    catch (const std::string& ex)
    {
        FatalLog("Exception occurred!  Reason: %s", ex.c_str());
    }
    catch (const char* ex)
    {
        FatalLog("Exception occurred!  Reason: %s", ex);
    }
    catch (...)
    {
        FatalLog("Uncaught exception occurred!");
    }

    VerboseLog("Done");
    return EXIT_SUCCESS;
}
