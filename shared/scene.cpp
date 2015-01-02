#include "scene.hpp"

void Scene::WriteData(std::ostream& file) const
{
    for (const auto& image : images)
        image->WriteData(file);
}

void Scene::WriteExport(std::ostream& file) const
{
    for (const auto& image : images)
        image->WriteExport(file);
}
