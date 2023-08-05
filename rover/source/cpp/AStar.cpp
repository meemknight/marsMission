#include "AStar.h"

#include <stdio.h>

namespace rover {
    std::unordered_map<int, HxString> AStar::coolRocks;
    std::unordered_map<HxString, std::pair<int, int> > AStar::coolRocksMap;

    void AStar::init() {
        coolRocks.rehash(4);
        coolRocksMap.rehash(4);
    }

    void AStar::storeCoolRock(HxString name, int x, int y) {
        if(contains(coolRocksMap, name)) {
            return;
        }

        if(strcmp(name, "c") == 0) {
            coolRocks[0] = name;
        }else if(strcmp(name, "d") == 0) {
            coolRocks[1] = name;
        }else if(strcmp(name, "e") == 0) {
            coolRocks[2] = name;
        }else if(strcmp(name, "f") == 0) {
            coolRocks[3] = name;
        }else {
            return;
        }

        coolRocksMap[name] = std::make_pair(x, y);
    }

    bool AStar::foundCoolRock(HxString name) {
        return contains(coolRocksMap, name);
    }

    int AStar::h(int x, int y, int x2, int y2) { // Using the Manhattan Distance Algorithm
        if(coolRocksMap.empty()) {
            return abs(x - x2) + abs(y - y2);
        }
    }
}
