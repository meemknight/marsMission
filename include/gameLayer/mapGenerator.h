#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <FastNoiseSIMD.h>
#include <vector>
#include <stack>
#include <stuff.h>

enum MazeTiles
{
	NotVisited = ' ',
	Visited = 4,
};

struct Map generate_world(glm::ivec2 maze_size, int seed);