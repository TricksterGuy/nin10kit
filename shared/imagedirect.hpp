#pragma once

#include <Magick++.h>
#include <memory>
#include <vector>

#include "color.hpp"
#include "image.hpp"
#include "scene.hpp"
#include "palette.hpp"

// Represents a single direct-through image.
// The image will be exported without color conversion if possible. 
class ImageDirect : public Image
{
    public:
        ImageDirect(const Magick::Image& image, const std::string& name, std::shared_ptr<Palette> globalPalette = nullptr);
        virtual void WriteData(std::ostream& file) const override;
        virtual void WriteExport(std::ostream& file) const override;
        virtual void WriteCommonExport(std::ostream& file) const override;
        virtual std::string GetImageType() const override;
        virtual bool HasPalette() const override;

    private:
        std::string m_imageTypeString;
        unsigned int m_nrOfPixels = 0;
        unsigned int m_nrOfuint16Pixels = 0;
        std::shared_ptr<Palette> m_palette;
        std::vector<unsigned char> m_data;
};

// Represents a set of direct-through images.
// The images will be exported without color conversion if possible. 
/*class ImageDirectScene : public Scene
{
    public:
        ImageDirectScene(const std::vector<Magick::Image>& images, const std::string& name, std::shared_ptr<Palette> globalPalette = nullptr);
        const ImageDirect& GetImage(int index) const;
        virtual void WriteData(std::ostream& file) const override;
        virtual void WriteExport(std::ostream& file) const override;
        std::shared_ptr<Palette> GetPalette() const;
        
    private:
        std::shared_ptr<Palette> m_palette;
};*/
