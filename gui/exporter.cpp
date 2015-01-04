#include "exporter.hpp"

#include <wx/filename.h>
#include <fstream>

#include "fileutils.hpp"
#include "logger.hpp"
#include "shared.hpp"

ExportParams params;
void DoGBAExport(const std::vector<Image32Bpp>& images, const std::vector<Image32Bpp>& tilesets);
void DoDSExport(const std::vector<Image32Bpp>& images, const std::vector<Image32Bpp>& tilesets);
void Do3DSExport(const std::vector<Image32Bpp>& images, const std::vector<Image32Bpp>& tilesets);

void GetModeInfo(int prog_mode, std::string& mode, std::string& device, int& bpp)
{
    bpp = 0;
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
    }
}

void DoExport(int mode, const std::string& filename, std::vector<std::string>& filenames, std::map<std::string, ImageInfo>& images)
{
    GetModeInfo(mode, params.mode, params.device, params.bpp);

    InfoLog("Using %s exporter mode %s", params.device.c_str(), params.mode.c_str());

    params.transparent_color = -1;
    params.dither_level = 80 / 100.0f;
    params.offset = 0;
    params.palette = 256;
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
    for (const auto& filename : filenames)
    {
        ImageInfo& info = images.at(filename);
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

    if (params.device == "GBA")
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
}
