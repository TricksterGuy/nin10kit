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
#include "export_params.hpp"
#include "fileutils.hpp"
#include "implementationfile.hpp"
#include "logger.hpp"
#include "lutgen.hpp"
#include "scanner.hpp"
#include "shared.hpp"
#include "version.h"


#ifndef _WIN32
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define EMPH "\033[1;33m"
#define END "\033[0m"
#define ENDEMPH END
#else
#define RED "<"
#define GREEN " "
#define END ">"
#define EMPH ""
#define ENDEMPH ""
#endif

void PrintMagickFormats(void)
{
    MagickCore::ExceptionInfo* exception = MagickCore::AcquireExceptionInfo();
    MagickCore::ListMagickInfo((FILE *)NULL, exception);
    CatchException(exception);
    exception = MagickCore::DestroyExceptionInfo(exception);
}

void DoGBAExport(const std::vector<Image32Bpp>& images, const std::vector<Image32Bpp>& tilesets, const std::vector<Image32Bpp>& palettes);
void DoDSExport(const std::vector<Image32Bpp>& images, const std::vector<Image32Bpp>& tilesets, const std::vector<Image32Bpp>& palettes);
void Do3DSExport(const std::vector<Image32Bpp>& images, const std::vector<Image32Bpp>& tilesets, const std::vector<Image32Bpp>& palettes);
void DoLUTExport(const std::vector<LutSpecification>& functions);

class Nin10KitApp : public wxAppConsole
{
    public:
        virtual bool OnInit();
        virtual void OnInitCmdLine(wxCmdLineParser& parser);
        virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
        void ShowBasicHelp();
        void OnHelp(const std::string& topic);
        bool DoExportImages();
        int OnRun();
};

static const wxCmdLineEntryDesc help_description[] =
{
    // Help
    {wxCMD_LINE_OPTION, "h", "h",       "", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_SWITCH, "help", "help", "", wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP},
    {wxCMD_LINE_NONE}
};

static const wxCmdLineEntryDesc cmd_descriptions[] =
{
    // Help
    {wxCMD_LINE_OPTION, "h", "h",                "", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_SWITCH, "help", "help",          "", wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP},
    // Debugging
    {wxCMD_LINE_OPTION, "",  "log",              "", wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},

    // Modes
    {wxCMD_LINE_OPTION, "", "mode",              "", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "", "device",            "", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "", "bpp",               "", wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},

    // Lut options
    {wxCMD_LINE_OPTION, "", "func",              "", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},

    // General helpful options
    {wxCMD_LINE_OPTION, "", "output_dir",        "", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "", "names",             "", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "", "resize",            "", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "", "transparent",       "", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_SWITCH, "", "dither",            ""},
    {wxCMD_LINE_SWITCH, "", "no_dither",         ""},
    {wxCMD_LINE_OPTION, "", "dither_level",      "", wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},

    // Mode 0/4 options
    {wxCMD_LINE_OPTION, "", "start",             "", wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "", "palette",           "", wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "", "palette_image",     "", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_SWITCH, "", "split",             ""},
    {wxCMD_LINE_SWITCH, "", "no_split",          ""},

    // Mode 0/1/2 exclusive options
    {wxCMD_LINE_SWITCH, "", "split_sbb",         ""},
    {wxCMD_LINE_SWITCH, "", "no_split_sbb",      ""},
    {wxCMD_LINE_OPTION, "", "tileset_image",     "", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_OPTION, "", "border",            "", wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_SWITCH, "", "affine",            ""},
    {wxCMD_LINE_SWITCH, "", "no_affine",         ""},
    {wxCMD_LINE_SWITCH, "", "force",             ""},
    {wxCMD_LINE_SWITCH, "", "no_force",          ""},

    // Sprite exclusive options
    {wxCMD_LINE_SWITCH, "", "export_2d",         ""},
    {wxCMD_LINE_SWITCH, "", "no_export_2d",      ""},
    {wxCMD_LINE_SWITCH, "", "for_bitmap",        ""},
    {wxCMD_LINE_SWITCH, "", "no_for_bitmap",     ""},

    // 3ds exclusive options
    {wxCMD_LINE_SWITCH, "", "3ds_rotate",        ""},
    {wxCMD_LINE_SWITCH, "", "no_3ds_rotate",     ""},

    // Other
    {wxCMD_LINE_SWITCH, "", "export_images",     ""},
    {wxCMD_LINE_SWITCH, "", "no_export_images",  ""},

    // Devkitpro
    {wxCMD_LINE_SWITCH, "", "for_devkitpro",     ""},
    {wxCMD_LINE_SWITCH, "", "no_for_devkitpro",  ""},

    // To accept the list of images this is used.
    {wxCMD_LINE_PARAM,  NULL, NULL, "", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE},
    {wxCMD_LINE_NONE}
};

// All of the read in command line flags will be in this structure.
ExportParams params;

struct HelpDesc
{
    HelpDesc(const std::string& _usage, const std::string& _text) : usage(_usage), text(_text) {}
    std::string usage;
    std::string text;
};

// Help text
const std::map<std::string, HelpDesc> help_text = {
{"help", HelpDesc("one of command_line_flag, formats, basic, flags, all","Display help on command line option.")},
{"log", HelpDesc("number [0-4]", "Logging level (0=fatal,1=error,2=warn,3=info,4=verbose). Default 3\n"
                                 "Any logging messages above the logging level will not be displayed.")},
{"mode", HelpDesc("one of 0, 3, 4, palette, tiles, map, sprites, bitmap, indexed, tilemap, lut",
                  "Sets the mode to export to.\n"
                  "0       - Used for GBA Mode 0 Exports a palette, tileset, and map.\n"
                  "3       - Used for GBA Mode 3 Exports a 16 bits per pixel bitmap image.\n"
                  "4       - Used for GBA Mode 4 Exports a 8 bits per pixel bitmap image with palette.\n"
                  "palette - Exports only a palette from images given.\n"
                  "tiles   - Exports just a palette and tileset from images given.\n"
                  "map     - Exports a just map given from images. [see -tileset]\n"
                  "sprites - Exports a bunch of sprites for use with the GBA/NDS.\n"
                  "bitmap  - Export a bitmap for use with the NDS.\n"
                  "indexed - Export a bitmap and palette for use with the NDS.\n"
                  "tilemap - Exports a palette, tileset, and map for use with the NDS.\n"
                  "lut     - Exports a look up table from a function. [see -func]")},
{"device", HelpDesc("one of gba nds or 3ds", "Specifies device to output data for default gba.\n"
                                            "GBA uses 16 bit color 0BGR 5 bits each for blue green and red.\n"
                                            "NDS uses 16 bit color ABGR 5 bits each for blue green and red and A is 1 bit of transparency 1=opaque 0=transparent.\n"
                                            "3DS uses 32? bit color ABGR 8 bits for each channel.")},
{"bpp", HelpDesc("number 4 or 8", "Bits per pixel for modes 0, tiles, map, and sprites. Default 8.")},
{"func", HelpDesc("function_name,type,start,end,step[,in_degrees]",
                  "For use with mode lut only adds lookup table for function\n"
                  "function_name: name of the function to generate a lut for see - http://www.cplusplus.com/reference/cmath/\n"
                  "\tAny function on that page who takes in a single input can be used for this.\n"
                  "type: Input and output type for the look up table.\n"
                  "\tCan be byte (8 bits), short (16 bits), int (32 bits), or long (64 bits).\n"
                  "\tTo specify a fixed point type use one of the types above and suffix .fixed_length to it.\n"
                  "\tExample byte.3 means use 8 bit values but use the bottom 3 bits for the fractional portion.\n"
                  "start: Starting value as a real number in the look up table array.\n"
                  "end: Ending value as a real number in the look up table array.\n"
                  "step: The step between values in the look up table.\n"
                  "\tThe value for step should evenly divide (end - start).\n"
                  "in_degrees: Inputs are specified in degrees default is false.\n\n"
                  "Example: --func=sin,short.8,0,360,0.25,true\n"
                  "\tThis will generate a lut for sin whose values are expressed as 16 bit fixed point\n"
                  "\twith 8 bits for the fractional part of the number\n"
                  "\tStarting with 0 ending with 360 in steps of 0.25\n"
                  "\tValues are specified in degrees.")},
{"output_dir", HelpDesc("path", "Output directory for exported files.\n"
                                "\tIf specified then this will be the directory where the files are exported\n"
                                "\tAnd will override any path given in the export file name.")},
{"names", HelpDesc("list_of_names", "Renames output array names to names given.\n"
                                    "\tIf used then each image given must be renamed.\n"
                                    "\tIf not given then the file names of the images will be used to generate the array name.")},
{"resize", HelpDesc("list of strings format numberxnumber",
                    "Resize images to wxh must be given for ALL images.\n"
                    "Example: --resize=10x12,,72x42\n"
                    "\tFirst image will be resized to width 10 height 12.\n"
                    "\tSecond image is not resized.\n"
                    "\tThird image is resized to width 72 height 42.")},
{"transparent", HelpDesc("color value using hex notation",
                         "Marks the color RRGGBB as transparent / backdrop.\n"
                         "Example --transparent=FFFFFF marks the color white as transparent / backdrop.\n"
                         "\tIn mode 3 a #define is given with the color so you can use it to ignore those pixels when drawing.\n"
                         "\tIn mode 4 this color will become palette entry 0 and also the background color.\n"
                         "\tIn any other GBA mode it will become palette entry 0 and the gba hardware will colorkey the color.\n"
                         "\tWhen passing an indexed color that is not used in the image, the color will be added to palette entry 0\n"
                         "\tand the indices of all image pixels will be increased by 1.\n")},
{"dither", HelpDesc("", "Enables dithering for mode 4.\n"
                              "Dithering makes the image look better by minimizing large areas of one color,\n"
                              "due to reducing the number of colors in the image by adding noise. default on\n"
                              "\tFor more information see: http://en.wikipedia.org/wiki/Dither")},
{"dither_level", HelpDesc("number [0-100]", "Affects the strength of the dithering algorithm used. default 10.")},
{"start", HelpDesc("number [0-255]", "Starts the palette off at index X.\n"
                                     "Useful if you have another image already exported that has X entries.\n"
                                     "This option is only available for mode 4.\n"
                                     "See also --palette")},
{"palette", HelpDesc("number [1-256]", "Restricts the palette to X colors.\n"
                                       "Useful for specifying the number of colors for each image.\n"
                                       "This option is only available for mode 4.\n"
                                       "See also --start")},
{"split", HelpDesc("", "Exports each individual image as if the program was called with just that one image.\n"
                             "In mode 4 using this will export each image with its own palette instead of a global palette.\n"
                             "In mode 0 using this will export each map with its own palette and tileset instead of a global tileset/palette.\n"
                             "In all other modes this option is ignored.")},
{"tileset_image", HelpDesc("list_of_images_or_urls", "Tileset image(s) to match tiles against when using --mode=map.\n"
                                                     "\tTo use form an image with the tileset you will use.\n"
                                                     "\tExport the tileset using --mode=tiles\n"
                                                     "\tThen form map images using the tiles in your tileset\n"
                                                     "\tThen export the maps using --mode=map with --tileset_image=tileset_image")},
{"palette_image", HelpDesc("list_of_images_or_urls", "Palette images(s) to match image against when using --mode=(4, tiles, sprites).\n"
                                                     "\tTo use form an image with the palette you will use.\n"
                                                     "\tAlternative just use --mode=palette and give it a set of images.\n"
                                                     "\tThen form images using the colors in your palette.\n"
                                                     "\tExport the images using --mode=mode with --palette_image=palette_image")},
{"border", HelpDesc("number", "Border around each tile in tileset image\n"
                              "Only for use with --mode=tiles")},
{"affine", HelpDesc("", "For use with --mode=tiles,map,0,tilemap.\n"
                        "Exports the map for use with affine backgrounds.\n"
                        "Ensures the palette generated is 8 bpp")},
{"export_2d", HelpDesc("", "Exports sprites for use in sprite 2d mode. Default 0.")},
{"for_bitmap", HelpDesc("", "Exports sprites for use in modes 3 and 4. Default 0.")},
{"for_devkitpro", HelpDesc("", "Exported definitions in header file are friendly with devkitpro libraries.\n"
                                     "\tOnly for DS exports only no effect on GBA/3DS. Default 0.")},
{"3ds_rotate", HelpDesc("", "Rotates the image for use in 3ds framebuffer mode. Default 0.")},
{"export_images", HelpDesc("", "In addition to generating a .c.h pair\n"
                                     "\texport images of each array generated as if it were displayed on the gba.\n"
                                     "\tThis means in a mode 4 export you will get a palette image showing the palette.\n"
                                     "\tIn a mode 0 export you will get a tileset image, a palette image, and a map image.\n"
                                     "\tIn sprites export you will get a palette image, and a sprite image.")},
{"force", HelpDesc("", "NOT IMPLEMENTED")},
{"split_sbb", HelpDesc("number [0-3]", "NOT IMPLEMENTED")},
};

IMPLEMENT_APP_NO_MAIN(Nin10KitApp);

int main(int argc, char** argv)
{
#ifdef _WIN32
    // InitializeMagick doesn't seem to play nice...
    // Plus *argv is just the application binary name not a full path.
    wxFileName magick = wxFileName::DirName(wxFileName::GetCwd());
    magick.SetFullName("magick");
    Magick::InitializeMagick(magick.GetFullPath().ToStdString().c_str());
    MagickCore::SetClientPath(magick.GetFullPath().ToStdString().c_str());
#else
    Magick::InitializeMagick(*argv);
#endif
    cpercep_init();

    wxEntryStart(argc, argv);

    if (!wxTheApp->CallOnInit())
        return EXIT_FAILURE;

    wxTheApp->OnRun();

    return EXIT_SUCCESS;
}

/** OnInit
  *
  * Initializes the program
  */
bool Nin10KitApp::OnInit()
{
    logger->SetLogLevel(LogLevel::WARNING);
    VerboseLog("Init");

    if (argc <= 1)
    {
        ShowBasicHelp();
        return false;
    }

    if (argc == 2)
    {
      wxCmdLineParser help_parser(argc, argv);
      help_parser.SetDesc(help_description);
      help_parser.Parse(false);
      help_parser.SetSwitchChars (_("-"));
      wxString help_topic;
      if (help_parser.Found("h", &help_topic) || help_parser.Found("help", &help_topic))
      {
          OnHelp(help_topic.ToStdString());
          return false;
      }
    }

    wxCmdLineParser parser(argc, argv);
    OnInitCmdLine(parser);

    bool cont = true;
    if (parser.Parse(false) == 0)
        cont = OnCmdLineParsed(parser);
    else
    {
        cont = false;
        ShowBasicHelp();
    }

    if (!cont)
        return false;

    // Give me the invocation
    std::ostringstream out;
    for (int i = 1; i < argc; i++)
        out << argv[i].ToStdString() << " ";

    header.SetInvocation(out.str());
    implementation.SetInvocation(out.str());

    return true;
}

void Nin10KitApp::ShowBasicHelp()
{
    printf("nin10kit version %s\n", AutoVersion::FULLVERSION_STRING);
    printf("Basic usage\n");
    printf("-----------\n");
    printf("nin10kit --mode=" GREEN "mode" END " " GREEN "export_file_to" END " " GREEN "list_of_image_files_or_urls" END "\n");
    printf(GREEN "mode" END " is one of 0, 3, 4, palette, tiles, map, sprites, or lut\n");
    printf(GREEN "export_file_to" END " is the filename to export to (can include directories)\n"
           "\tex: my_images will create my_images.c and my_images.h in the current directory.\n");
    printf(GREEN "list_of_image_files_or_urls" END " is a list of image files or URLs of images.\n"
           "\tTo see what image formats are supported use command nin10kit --h=formats\n\n");
    printf("For more detailed help use command nin10kit --h or --help\n"
           "\tnin10kit --h=" GREEN "flag" END " for a detailed description of what flag does\n"
           "\tnin10kit --h=flags for a list of all of the flags\n"
           "\tnin10kit --h=all or --help for all flags and their descriptions.\n");
}

void Nin10KitApp::OnHelp(const std::string& topic)
{
    if (topic == "all" || topic.empty())
    {
        for (const auto& help_desc : help_text)
        {
            const auto& desc = help_desc.second;
            printf("Flag %s\n"
                   "-----------------\n", help_desc.first.c_str());

            printf("Usage: " EMPH);
            if (desc.usage.empty())
                printf("--");
            printf("%s" ENDEMPH, help_desc.first.c_str());
            if (!desc.usage.empty())
                printf("=%s", desc.usage.c_str());
            printf("\n%s\n\n", desc.text.c_str());
        }
    }
    else if (topic == "flags")
    {
        printf("Available command line flags are as follows\n");
        for (const auto& flag_desc : help_text)
        {
            const auto& flag = flag_desc.first;
            const auto& desc = flag_desc.second;
#ifndef _WIN32
            printf(EMPH);
#endif
            if (desc.usage.empty())
                printf("--");
            printf("%s" ENDEMPH, flag.c_str());
            if (!desc.usage.empty())
                printf("=%s", desc.usage.c_str());
            printf("\n");
        }
        printf("\nTo negate a switch pass --no_switch instead!\n");
        printf("For help on a specific command line flag pass --help=flag\n");
    }
    else if (topic == "formats")
    {
        PrintMagickFormats();
        printf("The above table is what the underlying library ImageMagick supports\n");
        printf("The first entry in the table is what you are looking for\n");
    }
    else if (topic == "basic")
    {
        ShowBasicHelp();
    }
    else if (help_text.find(topic) != help_text.end())
    {
        const auto& desc = help_text.at(topic);
        printf("Help on %s\n"
               "-----------------\n", topic.c_str());

        printf("Usage: " EMPH);
        if (desc.usage.empty())
            printf("--");
        printf("%s" ENDEMPH, topic.c_str());
        if (!desc.usage.empty())
            printf("=%s", desc.usage.c_str());
        printf("\n%s\n", desc.text.c_str());
    }
    else
    {
        printf("No help available for %s please use --h=all or --help for a list of flags\n", topic.c_str());
    }
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

    wxString help_topic;
    if (parser.Found("h", &help_topic) || parser.Found("help", &help_topic))
    {
        OnHelp(help_topic.ToStdString());
        return false;
    }

    CmdLineParserHelper parse(parser);
    logger->SetLogLevel((LogLevel)parse.GetInt("log", 3, 0, 4));

    // mode params
    const std::set<std::string> valid_3ds_modes {"RGBA8", "RGB8", "RGB5A1", "RGBA5551", "RGB565", "RGBA4", "LUT"};
    const std::set<std::string> valid_gba_modes {"3", "4", "0", "SPRITES", "TILES", "MAP", "PALETTE", "LUT"};
    const std::set<std::string> valid_nds_modes {"BITMAP", "INDEXED", "TILEMAP", "SPRITES", "TILES", "MAP", "PALETTE", "LUT"};
    const std::set<std::string> valid_devices {"GBA", "DS", "NDS", "3DS"};

    params.device = ToUpper(parse.GetString("device", "GBA"));
    if (valid_devices.find(params.device) == valid_devices.end())
        FatalLog("Invalid device %s given.  Valid devices are [GBA, NDS, 3DS].", params.device.c_str());
    if (params.device == "DS") params.device = "NDS";


    params.mode = ToUpper(parse.GetString("mode", ""));
    if (params.mode.empty())
        FatalLog("No mode set.");

    if (params.device == "3DS" && valid_3ds_modes.find(params.mode) == valid_3ds_modes.end())
        FatalLog("Invalid mode specified %s for 3ds export. Valid modes are [RGBA8, RGB8, RGB5A1, RGB5551, RGB565, RGBA4]. Image not exported", params.mode.c_str());
    if (params.device == "NDS" && valid_nds_modes.find(params.mode) == valid_nds_modes.end())
        FatalLog("Invalid mode %s specified for nds export. Valid modes are [BITMAP, INDEXED, TILEMAP, SPRITES, TILES, MAP, PALETTE]", params.mode.c_str());
    if (params.device == "GBA" && valid_gba_modes.find(params.mode) == valid_gba_modes.end())
        FatalLog("Invalid mode %s specified for gba export. Valid modes are [3, 4, 0, SPRITES, TILES, MAP, PALETTE]", params.mode.c_str());

    params.bpp = parse.GetInt("bpp", 8);
    if (params.bpp != 4 && params.bpp != 8)
        FatalLog("Invalid bpp given %d.  Bpp must be either 4 or 8", params.bpp);

    std::string function = parse.GetString("func");

    // Helpful options
    params.output_dir = parse.GetString("output_dir");
    if (!params.output_dir.empty())
    {
        if (params.output_dir[params.output_dir.size() - 1] != '/')
            params.output_dir += "/";
    }

    params.names = parse.GetListString("names");

    std::vector<std::string> resizes = parse.GetListString("resize");

    unsigned int transparent = parse.GetHexInt("transparent", 0xFF000000, 0, 0xFFFFFF);
    params.transparent_given = transparent != 0xFF000000;
    params.transparent_color = Color(transparent);

    params.dither = parse.GetSwitch("dither", true);
    params.dither_level = parse.GetInt("dither_level", 10, 0, 100) / 100.0f;

    params.export_images = parse.GetSwitch("export_images");

    params.offset = parse.GetInt("start", 0, 0, 255);
    params.palette_size = parse.GetInt("palette", 256, 1, 256);
    params.palettes = parse.GetListString("palette_image");
    params.split = parse.GetSwitch("split");

    params.split_sbb = parse.GetSwitch("split_sbb");
    params.tilesets = parse.GetListString("tileset_image");
    params.border = parse.GetInt("border", 0, 0);
    params.affine = parse.GetSwitch("affine");
    params.force = parse.GetSwitch("force");

    params.export_2d = parse.GetSwitch("export_2d");
    params.for_bitmap = parse.GetSwitch("for_bitmap");
    params.for_devkitpro = parse.GetSwitch("for_devkitpro");
    params.rotate = parse.GetSwitch("3ds_rotate");

    std::string export_file = parser.GetParam(0).ToStdString();
    params.export_file = Chop(export_file);
    params.filename = params.output_dir.empty() ? export_file : params.output_dir + Chop(export_file);
    params.symbol_base_name = Sanitize(params.export_file);
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

    if (!params.tilesets.empty())
    {
        header.SetTilesets(params.tilesets);
        implementation.SetTilesets(params.tilesets);
    }

    if (params.mode == "tiles")
    {
        if (!params.resizes.empty())
        {
            WarnLog("Ignoring --resize when in tileset export mode, please resize manually.");
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
    std::map<std::string, std::vector<Magick::Image>> file_palettes;
    for (const auto& filename : params.files)
    {
        InfoLog("Reading image %s", filename.c_str());
        file_images[filename] = std::vector<Magick::Image>();
        readImages(&file_images[filename], filename);
    }
    for (const auto& tileset : params.tilesets)
    {
        InfoLog("Reading tileset %s", tileset.c_str());
        file_tilesets[tileset] = std::vector<Magick::Image>();
        readImages(&file_tilesets[tileset], tileset);
    }
    for (const auto& palette : params.palettes)
    {
        InfoLog("Reading image (for palette) %s", palette.c_str());
        file_palettes[palette] = std::vector<Magick::Image>();
        readImages(&file_palettes[palette], palette);
    }

    VerboseLog("DoHandleResize");
    for (unsigned int i = 0; i < params.resizes.size(); i++)
    {
        const std::string& filename = params.files[i];
        const resize& size = params.resizes[i];
        if (!size.IsValid()) continue;
        Magick::Geometry geom(size.width, size.height);
        geom.aspect(true);
        InfoLog("Resizing %s to (%d %d)", filename.c_str(), size.width, size.height);
        for (auto& image : file_images[filename])
            image.resize(geom);
    }

    VerboseLog("DoCheckAndLabelImages");
    for (unsigned int i = 0; i < params.files.size(); i++)
    {
        const std::string& filename = params.files[i];
        bool isAnim = file_images[filename].size() > 1;
        if (isAnim)
        {
            InfoLog("%s, detected as having %d frames", filename.c_str(), file_images[filename].size());
            if (file_images[filename][0].magick() == "GIF")
            {
                size_t disposalMethod = file_images[filename][0].gifDisposeMethod();
                VerboseLog("GIF Disposal Method is %d", disposalMethod);
                std::vector<Magick::Image> images;
                coalesceImages(&images, file_images[filename].begin(), file_images[filename].end());
                file_images[filename] = images;
            }
        }

        const std::vector<Magick::Image>& images = file_images[filename];
        unsigned int anim_width = images[0].columns();
        unsigned int anim_height = images[0].rows();
        for (unsigned int j = 0; j < images.size(); j++)
        {
            const Magick::Image& image = images[j];
            // Grumble some animations can have frames of different size
            if (anim_width != image.columns() || anim_height != image.rows())
            {
                WarnLog("Animated image %s has frames that aren't the same size expected (%d %d) got (%d %d). "
                        "You should resize this image via the --resize parameter.", filename.c_str(), anim_width,
                        anim_height, image.columns(), image.rows());
            }
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
            auto& image = images[j];
            if (params.rotate)
                image.rotate(90);
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

    for (unsigned int i = 0; i < params.palettes.size(); i++)
    {
        const std::string& filename = params.palettes[i];
        std::vector<Magick::Image>& images = file_palettes[filename];
        for (unsigned int j = 0; j < images.size(); j++)
        {
            const auto& image = images[j];
            params.palette_images.push_back(Image32Bpp(image, params.names[i], filename, j, images.size() > 1));
        }
    }

    VerboseLog("DoExportImages");
    InfoLog("Using %s exporter mode %s", params.device.c_str(), params.mode.c_str());

    header.SetMode(params.mode);
    implementation.SetMode(params.mode);

    if (params.mode == "LUT")
        DoLUTExport(params.functions);
    else if (params.device == "GBA")
        DoGBAExport(params.images, params.tileset_images, params.palette_images);
    else if (params.device == "NDS")
        DoDSExport(params.images, params.tileset_images, params.palette_images);
    else if (params.device == "3DS")
        Do3DSExport(params.images, params.tileset_images, params.palette_images);

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
