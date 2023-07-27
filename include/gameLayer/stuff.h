#pragma once
#include <gl2d/gl2d.h>]
#include <vector>
#define MAX_ROVER_LIFE 15

void renderRover(gl2d::Renderer2D &renderer,
	gl2d::Texture &roverTexture, gl2d::TextureAtlasPadding &roverAtlas,
	glm::vec2 pos, glm::vec3 color,
	bool hasAntena, int wheelLevel, int drilLevel, int gunLevel, int life, bool hasBatery, 
	int cameraLevel);

bool calculateView(glm::ivec2 playerPos, glm::ivec2 blockPos, int level);

struct Player
{

	Player() {};
	Player(glm::ivec2 p)
	{
		position = p;
		spawnPoint = p;
	};

	glm::ivec2 position = {1,1};
	int life = MAX_ROVER_LIFE - 5;
	bool hasAntena = 0;
	bool hasBatery = 0;
	int wheelLevel = 1;
	int cameraLevel = 1;
	int gunLevel = 1;
	int drilLevel = 1;

	int scannedThisTurn = 0;//up down left right

	int stones = 0;
	int iron = 0;
	int osmium = 0;

	int currentRound = 0;
	int id = 0;

	glm::ivec2 spawnPoint = {};

	glm::vec3 color = Colors_White;
};

void renderRover(gl2d::Renderer2D &renderer,
	gl2d::Texture &roverTexture, gl2d::TextureAtlasPadding &roverAtlas, Player &player);


enum Tiles
{
	Air = '.',
	Stone = 'X',
	Cobble_stone = 'A',
	Bedrock = 'B',
	Iron = 'C',
	Osmium = 'D',
	Base = 'E',
	Acid = 'F',

};


struct Map
{
	std::vector<char> mapData;

	glm::ivec2 size;

	void create(glm::ivec2 size)
	{
		this->size = size;
	
		mapData.clear();
		mapData.resize(size.x * size.y, Air);
	
		for (int i = 0; i < size.x; i++)
		{
			unsafeGet(i, 0) = Bedrock;
			unsafeGet(i, size.y-1) = Bedrock;
			unsafeGet(0, i) = Bedrock;
			unsafeGet(size.x-1, i) = Bedrock;
		}
		
	}
	
	char &unsafeGet(glm::ivec2 pos)
	{
		return unsafeGet(pos.x, pos.y);
	}

	char &unsafeGet(int x, int y)
	{
		return mapData[x + y * size.x];
	}

	void safeSet(int x, int y, char c)
	{
		if (x >= 0 && y >= 0 && x < size.x && y < size.y)
		{
			unsafeGet(x, y) = c;
		}
	}

	char getValue(glm::ivec2 pos)
	{
		return getValue(pos.x, pos.y);
	}

	char getValue(int x, int y)
	{
		return mapData[x + y * size.x];
	}

	std::string toString()
	{
		std::string result;
		for (int y = 0; y < this->size.y; y++)
		{
			for (int x = 0; x < this->size.x; x++)
			{
				result += this->unsafeGet(x, y);
			}
			result += "\n";
		}
		return result;
	}

	void blank(glm::ivec2 size, char element)
	{
		this->size = size;
		this->mapData.clear();
		this->mapData.resize(size.x * size.y, element);
	}

	struct Map clone()
	{
		struct Map c;
		c.blank(this->size, ' ');
		for (int y = 0; y < this->size.y; y++)
		{
			for (int x = 0; x < this->size.x; x++)
			{
				c.unsafeGet(x, y) = this->getValue(x, y);
			}
		}
		return c;
	}

	struct Map splat(glm::ivec2 size);

	void write_map(const char *path, struct Map *map)
	{
		auto file = fopen(path, "w");
		if (file != NULL)
		{
			auto s = map->toString();
			fputs(s.data(), file);
			fclose(file);
		}
		else
		{
			// big poo poo
		}
	}



	void render(gl2d::Renderer2D &renderer, gl2d::Texture &tiles,
		gl2d::TextureAtlasPadding &tilesAtlas, bool simulateFog,
		std::vector<int> viewLevel, std::vector<glm::ivec2> playerPos);


};

