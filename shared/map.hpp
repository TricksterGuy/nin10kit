#ifndef MAP_HPP
#define MAP_HPP

#include <memory>
#include <vector>

#include "image.hpp"
#include "scene.hpp"

class Image16Bpp;
class Tileset;

/** Class representing a map can be 4 or 8 bpp
  * Maps can only be 256x256 512x256 256x512 or 512x512
  */
class Map : public Image
{
    public:
        Map(const Image16Bpp& image, int bpp);
        Map(const Image16Bpp& image, std::shared_ptr<Tileset>& global_tileset);
        void WriteData(std::ostream& file) const;
        void WriteCommonExport(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        std::vector<unsigned short> data;
        std::shared_ptr<Tileset> tileset;
    private:
        void Init4bpp(const Image16Bpp& image);
        void Init8bpp(const Image16Bpp& image);
        bool export_shared_info;
};

/** Class representing a set of maps which use the same shared Tileset */
class MapScene : public Scene
{
    public:
        MapScene(const std::vector<Image16Bpp>& images, const std::string& name, int bpp);
        MapScene(const std::vector<Image16Bpp>& images, const std::string& name, std::shared_ptr<Tileset>& tileset);
        void WriteData(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        std::shared_ptr<Tileset> tileset;
};

#endif
