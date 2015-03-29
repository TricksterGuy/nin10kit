#ifndef IMAGE_UTIL_HPP
#define IMAGE_UTIL_HPP

#include <wx/bitmap.h>

#include "shared.hpp"
#include "alltypes.hpp"

#define THUMBNAIL_WIDTH 64
#define THUMBNAIL_HEIGHT 64

class ImageInfo
{
    public:
        ImageInfo(const std::string& filename);
        ~ImageInfo() {}
        const std::string& GetName() const {return name;}
        void SetName(const std::string& new_name) {name = new_name;}
        unsigned int GetWidth() const {return width;}
        void SetWidth(unsigned int new_width) {width = new_width;}
        unsigned int GetHeight() const {return height;}
        void SetHeight(unsigned int new_height) {height = new_height;}
        const wxBitmap& GetThumbnail() const {return thumbnail;}
        bool IsAnimated() const {return images.size() > 1;}
        unsigned int GetFrameCount() const {return images.size();}
        const std::vector<Magick::Image>& GetImages() const {return images;}
    private:
        std::string filename;
        std::string name;
        unsigned int width, height;
        wxBitmap thumbnail;
        std::vector<Magick::Image> images;
};

wxBitmap MagickToBitmap(Magick::Image, int width, int height);

void ConvertToMode3(std::map<std::string, ImageInfo>& images, std::vector<Image16Bpp>& images16);
void ConvertToMode4(std::map<std::string, ImageInfo>& images, std::vector<Image8Bpp>& images8);
void ConvertToMode0(std::map<std::string, ImageInfo>& images, std::vector<Map>& maps, int bpp);
void ConvertToSprites(std::map<std::string, ImageInfo>& images, std::vector<Sprite>& sprites, std::vector<PaletteBank>& banks, int bpp, bool for_bitmap);
void TransferToWx(const Image16Bpp& image, wxImage& wx);
void TransferToWx(const Image8Bpp& image, wxImage& wx);
void TransferToWx(const Palette& palette, wxImage& wx);
void TransferToWx(const std::vector<PaletteBank>& banks, wxImage& wx);
void TransferToWx(const Tileset& tileset, wxImage& wx);
void TransferToWx(const Map& map, wxImage& wx);
void TransferToWx(const Sprite& sprite, const std::vector<PaletteBank>& banks, wxImage& wx);

#endif
