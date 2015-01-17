#ifndef SCENE_HPP
#define SCENE_HPP

#include "image.hpp"
#include <memory>
#include <vector>

/** Represents a set of images who all share some resource (palette, tileset) */
class Scene : public Exportable
{
    public:
        Scene(const std::string& _name) : Exportable(_name) {}
        virtual ~Scene() {}
        unsigned int NumImages() const {return images.size();}
        std::vector<std::unique_ptr<Image>>& GetImages() {return images;}
        virtual void WriteData(std::ostream& file) const;
        virtual void WriteExport(std::ostream& file) const;
    protected:
        /** Images contained in this scene */
        std::vector<std::unique_ptr<Image>> images;
};

#endif
