#include "LoadingFilesFetcher.h"
#include <Bengine/ResourceManager.h>

std::vector<std::string> LoadingFilesFetcher::getMenuLoadingScreenImagePaths()
{
    std::vector<std::string> paths;
    std::vector<Bengine::DirEntry> entries;
    Bengine::IOManager::getDirectoryEntries("Textures/Characters/", entries);

    // Characters
    for (auto& entry : entries) {
        // Chibi is loaded within the main menu loading screen class, also all of these should be directories
        if (entry.path.find("Chibi") != std::string::npos || !entry.isDirectory) continue;

        // Get all of the characters
        std::vector<Bengine::DirEntry> characters;
        Bengine::IOManager::getDirectoryEntries(entry.path.c_str(), characters);

        // Loads all of the characters images
        for (auto& c : characters) {
            if (c.path.find(".png") != std::string::npos && !c.isDirectory) {
                paths.push_back(c.path);
            }
        }
    }

    entries.clear();
    Bengine::IOManager::getDirectoryEntries("Textures/Backgrounds/Main Menu/", entries);

    // Backgrounds
    for (auto& entry : entries) {
        if (entry.path.find(".png") != std::string::npos && !entry.isDirectory) {
            paths.push_back(entry.path);
        }
    }

    entries.clear();
    Bengine::IOManager::getDirectoryEntries("Textures/Visuals/Main Menu/", entries);

    // Visuals
    for (auto& entry : entries) {
        if (entry.path.find(".png") != std::string::npos && !entry.isDirectory) {
            paths.push_back(entry.path);
        }
    }

    fixPathSlashes(paths);

    return paths;
}

void LoadingFilesFetcher::fixPathSlashes(std::vector<std::string>& paths)
{
    const std::string s = "\\";
    const std::string t = "/";

    std::string::size_type n;

    for (size_t i = 0; i < paths.size(); i++) {
        n = 0;

        while ((n = paths[i].find(s, n)) != std::string::npos) {
            paths[i].replace(n, s.size(), t);
            n += t.size();
        }
    }
}
