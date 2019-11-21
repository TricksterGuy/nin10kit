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
        ExportFile() {};
        virtual ~ExportFile() {};

        static void SetInvocation(const std::string& invo) {invocation = invo;};
        static void SetTransparent(int color) {transparent_color = color;};
        static void SetMode(const std::string& _mode) {mode = _mode;};
        static void SetTilesets(const std::vector<std::string>& _tilesets) {tilesets = _tilesets;};

        static void AddLine(const std::string& line);
        static void AddImageInfo(const std::string& filename, int scene, int width, int height, bool frame);
        static void AddLutInfo(const LutSpecification& spec);
        static void Add(std::unique_ptr<Exportable> image);

        static void Clear();

        virtual void Write(std::ostream& file);

    private:
        static inline std::string invocation;
        static inline std::vector<std::string> lines;
        static inline std::vector<std::string> imageInfos;
        static inline std::vector<std::string> tilesets;
        static inline std::vector<LutSpecification> luts;

    protected:
        static inline int transparent_color = -1;
        static inline std::string mode = "3";
        static inline std::vector<std::unique_ptr<Exportable>> exportables;

        static std::map<std::string, std::vector<Image*>> GetAnimatedImages();
};

#endif
