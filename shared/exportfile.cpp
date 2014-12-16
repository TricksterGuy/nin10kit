#include "exportfile.hpp"

#include <cstdio>
#include <ctime>
#include <sstream>

#include "shared.hpp"
#include "version.h"

/** @brief Write
  *
  * @todo: document this function
  */
void ExportFile::Write(std::ostream& file)
{
    char str[1024];
    time_t aclock;
    struct tm *newtime;

    time(&aclock);
    newtime = localtime( &aclock );
    strftime(str, 96, "%A %m/%d/%Y, %H:%M:%S", newtime);

    file << "/*\n";
    file << " * Exported with nin10kit v" << AutoVersion::MAJOR << "." << AutoVersion::MINOR << "\n";
    if (!invocation.empty())
        file << " * Invocation command was nin10kit " << invocation << "\n";
    file << " * Time-stamp: " << str << "\n";
    if (!imageInfos.empty())
    {
        file << " * \n";
        file << " * Image Information\n";
        file << " * -----------------\n";
        for (const auto& imageInfo : imageInfos)
            file << " * " << imageInfo << "\n";
    }
    if (!tilesets.empty())
    {
        file << " * \n";
        file << " * Using tilesets\n";
        file << " * --------------\n";
        for (const auto& tileset : tilesets)
            file << " * " << tileset << "\n";
    }
    if (params.transparent_color != -1)
        file << " * Transparent color: 0x" << std::hex << transparent_color << std::dec << "\n";
    if (!luts.empty())
    {
        file << " * \n";
        file << " * Look Up Table Information\n";
        file << " * -------------------------\n";
        for (const auto& lut : luts)
            file << " * " << lut.str() << "\n";
    }
    file << " * \n";
    file << " * Quote/Fortune of the Day!\n";
    file << " * -------------------------\n";
    file << " * \n";
    file << " * All bug reports / feature requests are to be sent to Brandon (bwhitehead0308@gmail.com)\n";
    for (unsigned int i = 0; i < lines.size() ; i++)
        file << " * " << lines[i] << "\n";
    file << " */\n\n";
}

/** @brief AddLine
  *
  * @todo: document this function
  */
void ExportFile::AddLine(const std::string& line)
{
    lines.push_back(line);
}

/** @brief AddImageInfo
  *
  * @todo: document this function
  */
void ExportFile::AddImageInfo(const std::string& filename, int scene, int width, int height, bool frame)
{
    char buffer[1024];
    if (frame)
        snprintf(buffer, 1024, "%s (frame %d) %d@%d", filename.c_str(), scene, width, height);
    else
        snprintf(buffer, 1024, "%s %d@%d", filename.c_str(), width, height);
    imageInfos.push_back(buffer);
}

void ExportFile::AddLutInfo(const LutSpecification& spec)
{
    luts.push_back(spec);
}

void ExportFile::Add(std::shared_ptr<Exportable>& image)
{
    exportables.push_back(image);
}

std::map<std::string, std::vector<Image*>> ExportFile::GetAnimatedImages() const
{
    std::map<std::string, std::vector<Image*>> ret;
    /// TODO possibly sort with frame being key.
    for (const auto& exportable : exportables)
    {
        Exportable* export_ptr = exportable.get();
        Image* image = dynamic_cast<Image*>(export_ptr);
        Scene* scene = dynamic_cast<Scene*>(export_ptr);
        if (image)
        {
            ret[image->name].push_back(image);
        }
        else if (scene)
        {
            for (auto& image : scene->GetImages())
                ret[image->name].push_back(image.get());
        }
    }
    return ret;
}
