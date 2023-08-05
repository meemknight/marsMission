#pragma once

#include "rover.h"

namespace rover {
    struct AStar {
        static void init();
        static void storeCoolRock(HxString name, int x, int y);
        static bool foundCoolRock(HxString name);
        static int h(int x, int y, int x2, int y2);

        static std::unordered_map<int, HxString> coolRocks;
        static std::unordered_map<HxString, std::pair<int, int> > coolRocksMap;
    };
}
