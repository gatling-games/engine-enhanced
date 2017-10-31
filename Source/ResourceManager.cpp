#include "ResourceManager.h"

#include <functional>
#include <string>

ResourceID ResourceManager::pathToResourceID(const std::string &path)
{
    // Return a hash of the path string.
    return std::hash<std::string>{}(path);
}
