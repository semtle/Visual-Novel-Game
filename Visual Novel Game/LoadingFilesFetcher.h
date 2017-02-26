#pragma once

#include <vector>
#include <string>
#include <Bengine/IOManager.h>

class LoadingFilesFetcher {
public:
    static std::vector<std::string> getMenuLoadingScreenImagePaths();
private:
    // Replaces all double backslashes with a single forward slash
    static void fixPathSlashes(std::vector<std::string>& paths);
};