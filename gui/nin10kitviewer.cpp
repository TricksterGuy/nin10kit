#include "nin10kitviewer.hpp"

#include <algorithm>
#include "alltypes.hpp"
#include "exporter.hpp"
#include "logger.hpp"

Nin10KitViewerFrame::Nin10KitViewerFrame() : Nin10KitViewerGUI(NULL)
{
    wxSize size = buttonSizer->GetSize();
    size.SetWidth(256);
    buttonSizer->SetMinSize(size);
}

Nin10KitViewerFrame::~Nin10KitViewerFrame()
{
}

void Nin10KitViewerFrame::Set(int prog_mode, std::map<std::string, ImageInfo>& images)
{
    std::string mode, device;
    int bpp;

    GetModeInfo(prog_mode, mode, device, bpp);

    if (mode == "3")
        UpdateMode3(images);
    else if (mode == "4")
        UpdateMode4(images);
    else if (mode == "0")
        UpdateMode0(images, bpp);
    else if (mode == "sprites")
        UpdateSprites(images, bpp);
}

void Nin10KitViewerFrame::UpdateMode3(std::map<std::string, ImageInfo>& images)
{
    EventLog l(__func__);
    sharedSizer->Show(paletteSizer, false, true);
    sharedSizer->Show(tilesetSizer, false, true);
    graphics.clear();
    selectedGraphic = 0;

    std::vector<Image16Bpp> images16;
    ConvertToMode3(images, images16);

    for (const auto& image : images16)
    {
        wxImage wxImage;
        TransferToWx(image, wxImage);
        graphics.push_back(wxBitmap(wxImage));
    }

    graphicsBitmap->SetBitmap(graphics[selectedGraphic]);
}

void Nin10KitViewerFrame::UpdateMode4(std::map<std::string, ImageInfo>& images)
{
    EventLog l(__func__);
    sharedSizer->Show(paletteSizer, true, true);
    sharedSizer->Show(tilesetSizer, false, true);
    graphics.clear();
    selectedGraphic = 0;

    std::vector<Image8Bpp> images8;
    ConvertToMode4(images, images8);

    wxImage palette;
    TransferToWx(*images8[0].palette, palette);
    palette.Rescale(256, 256, wxIMAGE_QUALITY_NORMAL);

    for (const auto& image : images8)
    {
        wxImage wxImage;
        TransferToWx(image, wxImage);
        graphics.push_back(wxBitmap(wxImage));
    }

    wxBitmap& bitmap = graphics[selectedGraphic];
    graphicsBitmap->SetBitmap(bitmap);
    paletteBitmap->SetBitmap(wxBitmap(palette));
}

void Nin10KitViewerFrame::UpdateMode0(std::map<std::string, ImageInfo>& images, int bpp)
{
    EventLog l(__func__);
    sharedSizer->Show(paletteSizer, true, true);
    sharedSizer->Show(tilesetSizer, true, true);
    graphics.clear();
    selectedGraphic = 0;

    std::vector<Map> maps;
    ConvertToMode0(images, maps, bpp);

    Tileset& tileset = *maps[0].tileset;

    wxImage wxtileset;
    wxImage wxpalette;

    if (bpp == 4)
        TransferToWx(tileset.paletteBanks, wxpalette);
    else
        TransferToWx(*tileset.palette, wxpalette);
    wxpalette.Rescale(256, 256, wxIMAGE_QUALITY_NORMAL);

    TransferToWx(tileset, wxtileset);
    wxtileset.Rescale(256, 256, wxIMAGE_QUALITY_NORMAL);

    for (const auto& map : maps)
    {
        wxImage wxImage;
        TransferToWx(map, wxImage);
        graphics.push_back(wxBitmap(wxImage));
    }

    wxBitmap& bitmap = graphics[selectedGraphic];
    graphicsBitmap->SetBitmap(bitmap);
    paletteBitmap->SetBitmap(wxBitmap(wxpalette));
    tilesetBitmap->SetBitmap(wxBitmap(wxtileset));
}

void Nin10KitViewerFrame::UpdateSprites(std::map<std::string, ImageInfo>& images, int bpp)
{
    EventLog l(__func__);
}

void Nin10KitViewerFrame::OnPrev(wxCommandEvent& event)
{
    EventLog l(__func__);
    selectedGraphic = std::max(std::min(selectedGraphic - 1, (int)graphics.size() - 1), 0);
    VerboseLog("%d is now selected", selectedGraphic);
    graphicsBitmap->SetBitmap(graphics[selectedGraphic]);

}

void Nin10KitViewerFrame::OnNext(wxCommandEvent& event)
{
    EventLog l(__func__);
    selectedGraphic = std::max(std::min(selectedGraphic + 1, (int)graphics.size() - 1), 0);
    VerboseLog("%d is now selected", selectedGraphic);
    graphicsBitmap->SetBitmap(graphics[selectedGraphic]);
}
