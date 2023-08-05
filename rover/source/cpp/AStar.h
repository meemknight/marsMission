#pragma once

#include "rover.h"

namespace rover {
    struct AStar {
        static void init();
        static void execute(int x, int y, int width, int height);
        static void storeCoolRock(HxString name, int x, int y);

        static std::unordered_set<HxString> coolRocks;
        static std::unordered_map<HxString, std::pair<int, int> > coolRocksMap;
    };
}
