#include <mapGenerator.h>
#include <stuff.h>


struct Map maze_map(glm::ivec2 halfsize_minus_one, char visited = Air, char not_visited = Bedrock, bool generate_borders = true, 
	int seed = 69)
{
	struct Map map;

	auto size = glm::ivec2(halfsize_minus_one.x * 2 + 1, halfsize_minus_one.y * 2 + 1);
	map.blank(size, NotVisited);

	std::stack<glm::ivec2> cell_stack;
	cell_stack.push(glm::ivec2(1, 1));
	int visited_cells = 1;

	auto getOffset = [&](int ox, int oy)->char &
	{
		return map.unsafeGet(cell_stack.top().x + ox, cell_stack.top().y + oy);
	};

	auto getOffsetValue = [&](int ox, int oy)
	{
		return map.getValue(cell_stack.top().x + ox, cell_stack.top().y + oy);
	};

	getOffset(0, 0) = Visited;
	std::srand(seed);
	while (visited_cells < halfsize_minus_one.x * halfsize_minus_one.y)
	{
		std::vector<int> neighbours;
		// go north
		if (cell_stack.top().y >= 3)
		{
			if (getOffsetValue(0, -2) != Visited)
			{
				neighbours.push_back(0);
			}
		}
		if (cell_stack.top().y <= size.y - 3)
		{
			if (getOffsetValue(0, 2) != Visited)
			{
				neighbours.push_back(2);
			}
		}
		if (cell_stack.top().x >= 3)
		{
			if (getOffsetValue(-2, 0) != Visited)
			{
				neighbours.push_back(1);
			}
		}
		if (cell_stack.top().x <= size.x - 3)
		{
			if (getOffsetValue(2, 0) != Visited)
			{
				neighbours.push_back(3);
			}
		}

		if (!neighbours.empty())
		{
			// int next_cell_dir = 3;
			int next_cell_dir = neighbours[std::rand() % neighbours.size()];

			switch (next_cell_dir)
			{
			case 0:
			getOffset(0, 0) = Visited;
			getOffset(0, -1) = Visited;
			cell_stack.push(glm::ivec2(cell_stack.top().x, cell_stack.top().y - 2));
			break;
			case 1:
			getOffset(0, 0) = Visited;
			getOffset(-1, 0) = Visited;
			cell_stack.push(glm::ivec2(cell_stack.top().x - 2, cell_stack.top().y));
			break;
			case 2:
			getOffset(0, 0) = Visited;
			getOffset(0, 1) = Visited;
			cell_stack.push(glm::ivec2(cell_stack.top().x, cell_stack.top().y + 2));
			break;
			case 3:
			getOffset(0, 0) = Visited;
			getOffset(1, 0) = Visited;
			cell_stack.push(glm::ivec2(cell_stack.top().x + 2, cell_stack.top().y));
			break;
			}
			getOffset(0, 0) = Visited;
			visited_cells++;
		}
		else
		{
			cell_stack.pop();
		}
	}
	if (generate_borders)
	{
		for (int y = 0; y < size.y; y++)
		{
			for (int x = 0; x < size.x; x++)
			{
				if (map.getValue(x, y) == Visited)
				{
					map.unsafeGet(x, y) = visited;
				}
				else
				{
					map.unsafeGet(x, y) = not_visited;
				}
			}
		}
		return map;
	}
	else
	{
		struct Map bl_map;
		auto bl_size = glm::ivec2(size.x - 2, size.y - 2);
		bl_map.blank(bl_size, Air);
		for (int y = 1; y < size.y - 1; y++)
		{
			for (int x = 1; x < size.x - 1; x++)
			{
				if (map.getValue(x, y) == Visited)
				{
					bl_map.unsafeGet(x - 1, y - 1) = visited;
				}
				else
				{
					bl_map.unsafeGet(x - 1, y - 1) = not_visited;
				}
			}
		}
		return bl_map;
	}
}

struct Map simplex_map(glm::ivec2 size, FastNoiseSIMD *fn, char fill_above = Bedrock, char fill_below = Air, float threshold = 0.5, float noise_zoom = 1.0, int seed = 69)
{
	struct Map map;
	map.blank(size, fill_below);
	fn->SetSeed(seed);
	auto fs = fn->GetSimplexSet(0, 0, 0, size.y, size.x, 1, noise_zoom);
	for (int y = 0; y < size.y; y++)
	{
		for (int x = 0; x < size.x; x++)
		{
			auto f = fs[x + size.x * y];
			if (f >= threshold)
			{
				map.unsafeGet(x, y) = fill_above;
			}
			else
			{
				map.unsafeGet(x, y) = fill_below;
			}
		}
	}
	fn->FreeNoiseSet(fs);
	return map;
}

struct Map mask_map(struct Map *map, struct Map *mask, glm::ivec2 mask_offset, char ignore_block = Air)
{
	assert(mask_offset.x >= 0);
	assert(mask_offset.y >= 0);
	assert(map->size.x >= mask->size.x + mask_offset.x);
	assert(map->size.y >= mask->size.y + mask_offset.y);
	struct Map isect;
	isect.blank(map->size, ignore_block);
	for (int y = 0; y < mask->size.y; y++)
	{
		for (int x = 0; x < mask->size.x; x++)
		{
			if (mask->getValue(x, y) != ignore_block)
			{
				isect.unsafeGet(x + mask_offset.x, y + mask_offset.y) = map->getValue(x + mask_offset.x, y + mask_offset.y);
			}
		}
	}
	return isect;
}

struct Map invert_map(struct Map *map, char old_zero = Air, char old_one = Bedrock)
{
	struct Map inv;
	inv.blank(map->size, old_zero);
	for (int y = 0; y < inv.size.y; y++)
	{
		for (int x = 0; x < inv.size.x; x++)
		{
			if (map->getValue(x, y) == old_zero)
			{
				inv.unsafeGet(x, y) = old_one;
			}
			else
			{
				inv.unsafeGet(x, y) = old_zero;
			}
		}
	}
	return inv;
}


struct Map additive_mask(struct Map *map, struct Map *mask, glm::ivec2 mask_offset, char ignore_block = Air)
{
	assert(mask_offset.x >= 0);
	assert(mask_offset.y >= 0);
	assert(map->size.x >= mask->size.x + mask_offset.x);
	assert(map->size.y >= mask->size.y + mask_offset.y);
	struct Map add = map->clone();
	for (int y = 0; y < mask->size.y; y++)
	{
		for (int x = 0; x < mask->size.x; x++)
		{
			if (mask->getValue(x, y) != ignore_block)
			{
				add.unsafeGet(x + mask_offset.x, y + mask_offset.y) = mask->getValue(x + mask_offset.x, y + mask_offset.y);
			}
		}
	}
	return add;
}

struct Map layered_simplex_map(glm::ivec2 size, FastNoiseSIMD *fn, char fill_above = Bedrock, char fill_below = Air, float base_threshold = 0.5, float threshold_multiplier = 0.5, 
	float base_noise_zoom = 1.0, float zoom_multiplier = 0.5, int octaves = 2, int seed = 69)
{
	assert(octaves >= 1);
	auto threshold = base_threshold;
	auto zoom = base_noise_zoom;
	int i = 0;
	auto lsm = simplex_map(size, fn, fill_above, fill_below, threshold, zoom, seed);
	while (i < octaves)
	{
		threshold *= threshold_multiplier;
		zoom *= zoom_multiplier;
		auto lsm_to_add = simplex_map(size, fn, fill_above, fill_below, threshold, zoom, seed);
		lsm = additive_mask(&lsm, &lsm_to_add, glm::ivec2(0, 0), fill_below);
		i++;
	}
	return lsm;
}

struct Map generate_world(glm::ivec2 maze_size, int seed, bool fewerResources)
{
	auto addSpawn = [&](int x, int y, Map &m)
	{
		for (int j = -2; j < 2; j++)
			for (int i = -2; i < 2; i++)
			{
				if (glm::distance(glm::vec2(0, 0), glm::vec2(i, j)) < sqrt(7.f) + 0.1)
				{
					m.safeSet(x + i, y + j, Air);
				}
			}

		m.safeSet(x, y, Base);
	};

	srand(seed);

	auto fn = FastNoiseSIMD::NewFastNoiseSIMD();

	auto m1 = maze_map(maze_size, Air, Stone, false);

	auto s_size = m1.size;

	auto lab_holes = layered_simplex_map(s_size, fn, Stone, Air, 0.5, 0.95, 3, 2, 2, seed + 2);
	auto lab_bedrock = layered_simplex_map(s_size, fn, Bedrock, Air, 0.75, 0.90, 3, 2, 2, seed + 7);

	auto extraRock = layered_simplex_map(s_size, fn, Stone, Air, 0.6, 0.95, 3, 2, 2, seed + 3);

	auto maze_with_holes = mask_map(&m1, &lab_holes, glm::ivec2(0, 0), Air);
	auto maze_with_bedrock = mask_map(&lab_bedrock, &maze_with_holes, glm::ivec2(0, 0), Air);

	maze_with_bedrock = additive_mask(&maze_with_bedrock, &extraRock, glm::ivec2(0, 0), Air);
	maze_with_bedrock = additive_mask(&maze_with_holes, &maze_with_bedrock, {}, Air);

	float baseIronTresshold = 0.975;
	if (fewerResources) { baseIronTresshold = 1; }
	auto random_iron = layered_simplex_map(s_size, fn, Iron, Air, baseIronTresshold, 0.975, 8, 2, 4, seed + 1);
	
	Map cobaltMap;
	cobaltMap.blank(s_size, Air);
	int advance = 10;
	if (fewerResources) { advance = 15; }
	for (int j = 0; j < s_size.y; j+=advance)
	{
		for (int i = 0; i < s_size.x; i+= advance)
		{
			int offsetX = rand() % advance;
			int offsetY = rand() % advance;

			cobaltMap.safeSet(i + offsetX, j + offsetY, Tiles::Osmium);

			cobaltMap.safeSet(i + offsetX+1, j + offsetY, Tiles::Stone);
			cobaltMap.safeSet(i + offsetX-1, j + offsetY, Tiles::Stone);
			cobaltMap.safeSet(i + offsetX, j + offsetY-1, Tiles::Stone);
			cobaltMap.safeSet(i + offsetX, j + offsetY+1, Tiles::Stone);
		}
	}

	//auto cobalt_stone = layered_simplex_map(s_size, fn, Stone, Air, 0.8, 1.0, 9, 2, 4);
	//auto random_cobalt = layered_simplex_map(s_size, fn, Tiles::Osmium, Air, 0.99, 0.993, 9, 2, 8);
	//random_cobalt = additive_mask(&cobalt_stone, &random_cobalt, glm::ivec2(0, 0), Air);

	auto final_map = additive_mask(&maze_with_bedrock, &random_iron, glm::ivec2(0, 0), Air);
	final_map = additive_mask(&final_map, &random_iron, glm::ivec2(0, 0), Air);
	final_map = additive_mask(&final_map, &cobaltMap, glm::ivec2(0, 0), Air);

	std::vector<glm::vec2> positions;

	float radians = 0;
	for (int i = 0; i < 5; i++)
	{
		glm::vec2 points(sin(radians), cos(radians));
		
		points *= (final_map.size.x / 2.f) - 7;

		points += final_map.size / 2;

		positions.push_back(points);

		radians += (2 * 3.141592) / 5.f;
	}

	for (auto i : positions)
	{
		addSpawn(i.x, i.y, final_map);
	}

	for (int i = 0; i < final_map.size.x; i++)
	{
		final_map.unsafeGet(i, 0) = Bedrock;
		final_map.unsafeGet(i, final_map.size.y - 1) = Bedrock;
		final_map.unsafeGet(0, i) = Bedrock;
		final_map.unsafeGet(final_map.size.x - 1, i) = Bedrock;
	}

	return final_map;
}

