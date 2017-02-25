#pragma once

#include <vector>
#include <string>
#include <Bengine/IOManager.h>

class LoadingFilesFetcher {
public:
    static std::vector<std::string> getMenuLoadingScreenImagePaths();
};