#include "AStar.h"

#include <stdio.h>

namespace rover {
    void AStar::init() {
        coolRocks.reserve(4);
        coolRocksMap.rehash(4);
    }

    void AStar::execute(int x, int y, int width, int height) {
        
    }

    void AStar::storeCoolRock(HxString name, int x, int y) {
        if(contains(coolRocks, name)) {
            return;
        }

        coolRocks.insert(name);
        coolRocksMap[name] = std::make_pair(x, y);
    }
}
