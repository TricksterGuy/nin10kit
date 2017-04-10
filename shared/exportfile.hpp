#ifndef EXPORT_FILE_HPP
#define EXPORT_FILE_HPP

#include <iostream>
#include <memory>
#include <map>
#include <string>
#include <vector>

#include "lutgen.hpp"
#include "image.hpp"

/** Base class for a file created by this program.*/
class ExportFile
{
    public:
        ExportFile(const std::string& _invocation = "") : invocation(_invocation), transparent_color(-1), mode("3") {};
        virtual ~ExportFile() {};

        void SetInvocation(const std::string& invo) {invocation = invo;};
        void SetTransparent(int color) {transparent_color = color;};
        void SetMode(const std::string& _mode) {mode = _mode;};
        void SetTilesets(const std::vector<std::string>& _tilesets) {tilesets = _tilesets;};

        void AddLine(const std::string& line);
        void AddImageInfo(const std::string& filename, int scene, int width, int height, bool frame);
        void AddLutInfo(const LutSpecification& spec);
        void Add(std::shared_ptr<Exportable>& image);

        void Clear();

        virtual void Write(std::ostream& file);

    private:
        std::string invocation;
        std::vector<std::string> lines;
        std::vector<std::string> imageInfos;
        std::vector<std::string> tilesets;
        std::vector<LutSpecification> luts;

    protected:
        int transparent_color;
        std::string mode;
        std::vector<std::shared_ptr<Exportable>> exportables;
        std::map<std::string, std::vector<Image*>> GetAnimatedImages() const;
};

#endif
