#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "exportable.hpp"

/** Represents a single frame of an image */
class Image : public Exportable
{
    public:
        Image(unsigned int _width, unsigned int _height, const std::string& _name = "", const std::string& _filename = "", unsigned int _frame = 0, bool _animated = false);
        /** If Image is actually part of an animated Image export information common to all of these images */
        virtual void WriteCommonExport(std::ostream& file) const = 0;
        /** Gets type of image array for generating an array of images for animation. */
        virtual std::string GetImageType() const {return "const unsigned short*";}
        /** Gets the symbol base name for this image */
        virtual std::string GetExportName() const {return export_name;}
        /** Does this image have a palette attached to it */
        virtual bool HasPalette() const {return false;}
        /** Width of image in either pixels or tiles */
        unsigned int width;
        /** Height of image in either pixels or tiles */
        unsigned int height;
        /** Filename of image */
        std::string filename;
        /** Frame of image if this image came from an animated image */
        unsigned int frame;
        /** Is this image part of an animated image */
        bool animated;
    protected:
        /** Symbol base name */
        std::string export_name;
};

#endif
