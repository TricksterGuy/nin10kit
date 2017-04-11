#include "exporter.hpp"

#include <wx/filename.h>
#include <fstream>

#include "export_params.hpp"
#include "fileutils.hpp"
#include "logger.hpp"
#include "shared.hpp"

ExportParams params;
void DoGBAExport(const std::vector<Image32Bpp>& images, const std::vector<Image32Bpp>& tilesets, const std::vector<Image32Bpp>& palettes);
void DoDSExport(const std::vector<Image32Bpp>& images, const std::vector<Image32Bpp>& tilesets, const std::vector<Image32Bpp>& palettes);
void Do3DSExport(const std::vector<Image32Bpp>& images, const std::vector<Image32Bpp>& tilesets, const std::vector<Image32Bpp>& palettes);

void GetModeInfo(int prog_mode, std::string& mode, std::string& device, int& bpp, bool& sprites_for_bitmap)
{
    bpp = 0;
    sprites_for_bitmap = false;
    switch (prog_mode)
    {
        case GBAMode3:
            mode = "3";
            device = "GBA";
            break;
        case GBAMode4:
            mode = "4";
            device = "GBA";
            break;
        case GBAMode08Bpp:
            mode = "0";
            device = "GBA";
            bpp = 8;
            break;
        case GBAMode04Bpp:
            mode = "0";
            device = "GBA";
            bpp = 4;
            break;
        case GBASprites8Bpp:
            mode = "SPRITES";
            device = "GBA";
            bpp = 8;
            break;
        case GBASprites4Bpp:
            mode = "SPRITES";
            device = "GBA";
            bpp = 4;
            break;
        case GBAMode3Sprites8Bpp:
            mode = "SPRITES";
            device = "GBA";
            bpp = 8;
            sprites_for_bitmap = true;
            break;
        case GBAMode3Sprites4Bpp:
            mode = "SPRITES";
            device = "GBA";
            bpp = 4;
            sprites_for_bitmap = true;
            break;
        case NDSBitmap:
            mode = "BITMAP";
            device = "NDS";
            break;
        case NDSIndexedBitmap:
            mode = "INDEXED";
            device = "NDS";
            break;
        case NDSMap8Bpp:
            mode = "TILEMAP";
            device = "NDS";
            bpp = 8;
            break;
        case NDSMap4Bpp:
            mode = "TILEMAP";
            device = "NDS";
            bpp = 4;
            break;
        case NDSSprites8Bpp:
            mode = "SPRITES";
            device = "NDS";
            bpp = 8;
            break;
        case NDSSprites4Bpp:
            mode = "SPRITES";
            device = "NDS";
            bpp = 4;
            break;
    }
}

void DoExport(int mode, const std::string& filename, const std::vector<std::string>& filenames, const std::map<std::string, ImageInfo>& images)
{
    GetModeInfo(mode, params.mode, params.device, params.bpp, params.for_bitmap);

    InfoLog("Using %s exporter mode %s", params.device.c_str(), params.mode.c_str());

    params.transparent_color = -1;
    params.dither_level = 10 / 100.0f;
    params.offset = 0;
    params.palette_size = 256;
    params.border = 0;

    header.SetMode(params.mode);
    implementation.SetMode(params.mode);

    wxFileName file(filename);

    params.output_dir = file.GetFullPath().ToStdString();
    params.export_file = file.GetFullName().ToStdString();
    params.filename = filename;
    params.symbol_base_name = Format(filename);
    params.files = filenames;

    VerboseLog("output dir is %s export file is %s filename %s symbol base name %s",
            params.output_dir.c_str(), params.export_file.c_str(), params.filename.c_str(), params.symbol_base_name.c_str());

    params.images.clear();
    params.names.clear();
    params.functions.clear();
    params.tilesets.clear();
    params.palettes.clear();
    params.tileset_images.clear();
    params.palette_images.clear();
    params.resizes.clear();

    for (const auto& filename : filenames)
    {
        const ImageInfo& info = images.at(filename);
        params.names.push_back(info.GetName());
        for (auto image : info.GetImages())
        {
            Magick::Geometry geom(info.GetWidth(), info.GetHeight());
            geom.aspect(true);
            image.resize(geom);
            params.images.push_back(Image32Bpp(image, info.GetName(), filename, image.scene(), info.IsAnimated()));
            header.AddImageInfo(filename, image.scene(), image.columns(), image.rows(), info.IsAnimated());
            implementation.AddImageInfo(filename, image.scene(), image.columns(), image.rows(), info.IsAnimated());
        }
    }

    try
    {
        if (params.device == "GBA")
            DoGBAExport(params.images, params.tileset_images, params.palette_images);
        else if (params.device == "NDS")
            DoDSExport(params.images, params.tileset_images, params.palette_images);
        else if (params.device == "3DS")
            Do3DSExport(params.images, params.tileset_images, params.palette_images);
    }
    // Catch FatalLogs from exporting.  This is handled in wxlogger
    catch (const char* e)
    {
        return;
    }

    InfoLog("Export complete now writing files");
    // Write the files
    std::ofstream file_c, file_h;
    InitFiles(file_c, file_h, params.filename);

    header.Write(file_h);
    implementation.Write(file_c);

    file_h.close();
    file_c.close();

    header.Clear();
    implementation.Clear();
}
