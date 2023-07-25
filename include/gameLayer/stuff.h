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
	glm::ivec2 position = {1,1};
	int life = MAX_ROVER_LIFE - 5;
	bool hasAntena = 0;
	bool hasBatery = 0;
	int wheelLevel = 1;
	int cameraLevel = 1;
	int gunLevel = 1;
	int drilLevel = 1;

	int stones = 0;
	int iron = 0;
	int osmium = 0;

	int currentRound = 0;
	int id = 0;

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

	void render(gl2d::Renderer2D &renderer, gl2d::Texture &tiles,
		gl2d::TextureAtlasPadding &tilesAtlas, bool simulateFog,
		int viewLevel, glm::ivec2 playerPos);


};

