#include <stuff.h>

bool calculateView(glm::ivec2 playerPos, glm::ivec2 blockPos, int level)
{

	if (level == 1) { return glm::distance(glm::vec2(playerPos), glm::vec2(blockPos)) < std::sqrt(5.f) + 0.1; }
	if (level == 2) { return glm::distance(glm::vec2(playerPos), glm::vec2(blockPos)) < std::sqrt(12.f) + 0.1; }
	if (level == 3) { return glm::distance(glm::vec2(playerPos), glm::vec2(blockPos)) < std::sqrt(20.f) + 0.1; }

	return 0;
}

void renderRover(gl2d::Renderer2D &renderer, 
	gl2d::Texture &roverTexture, gl2d::TextureAtlasPadding &roverAtlas,
	glm::vec2 pos, glm::vec3 color,
	bool hasAntena, int wheelLevel, int drilLevel, int gunLevel, int life, bool hasBatery,
	int cameraLevel)
{

	glm::vec2 size(100, 100);

	float lifeFloat = (float)life / MAX_ROVER_LIFE;

	renderer.renderRectangle({pos,size.x * 1,10}, Colors_Black);
	renderer.renderRectangle({pos,size.x * lifeFloat,10}, Colors_Red);

	renderer.renderRectangle({pos,size}, roverTexture,
		glm::vec4(color, 1), {}, 0, roverAtlas.get(0, 0));


	renderer.renderRectangle({pos,size}, roverTexture,
		glm::vec4(1, 1, 1, 1), {}, 0, roverAtlas.get(1, 0));

	renderer.renderRectangle({pos,size}, roverTexture,
		glm::vec4(1, 1, 1, 1), {}, 0, roverAtlas.get(gunLevel-1, 3));

	if (hasAntena)
	{
		renderer.renderRectangle({pos,size}, roverTexture,
			glm::vec4(1, 1, 1, 1), {}, 0, roverAtlas.get(2, 0));
	}

	if (hasBatery)
	{
		renderer.renderRectangle({pos,size}, roverTexture,
			glm::vec4(1, 1, 1, 1), {}, 0, roverAtlas.get(3, 0));
	}

	renderer.renderRectangle({pos,size}, roverTexture,
		glm::vec4(1, 1, 1, 1), {}, 0, roverAtlas.get(drilLevel-1, 2));

	renderer.renderRectangle({pos,size}, roverTexture,
		glm::vec4(1, 1, 1, 1), {}, 0, roverAtlas.get(wheelLevel-1, 1));

	renderer.renderRectangle({pos,size}, roverTexture,
		glm::vec4(1, 1, 1, 1), {}, 0, roverAtlas.get(cameraLevel - 1, 4));

}

void renderRover(gl2d::Renderer2D &renderer, gl2d::Texture &roverTexture, 
	gl2d::TextureAtlasPadding &roverAtlas, Player &player)
{
	renderRover(renderer, roverTexture, roverAtlas, player.position * 100,
		player.color, player.hasAntena, player.wheelLevel,
		player.drilLevel, player.gunLevel, player.life, player.hasBatery, player.cameraLevel);
}


struct Map splat(glm::ivec2 size)
{
	struct Map map;
	map.create(size);
	return map;
}

void Map::render(gl2d::Renderer2D & renderer, gl2d::Texture & tiles,
	gl2d::TextureAtlasPadding &tilesAtlas, bool simulateFog,
	std::vector<int> viewLevel, std::vector<glm::ivec2> playerPos)
{

	glm::vec2 drawSize(100, 100);



	for (int j = 0; j < size.y; j++)
	{
		for (int i = 0; i < size.x; i++)
		{
			int tileType = 0;

			switch (unsafeGet({i,j}))
			{
			case Air:
			tileType = 0; break;
			case Stone:
			tileType = 1; break;
			case Cobble_stone:
			tileType = 2; break;
			case Bedrock:
			tileType = 5; break;
			case Osmium:
			tileType = 4; break;
			case Iron:
			tileType = 3; break;
			case Base:
			tileType = 6; break;
			case Acid:
			tileType = 7; break;


			default: tileType = 9;
			}

			glm::vec4 color = Colors_White;

			if (simulateFog)
			{
				bool yes = 0;
				for (int p=0;p<playerPos.size();p++)
				{
					if (calculateView(playerPos[p], {i,j}, viewLevel[p]))
					{
						yes = 1;
						break;
					}
				}
				if (!yes)
				{
					color = glm::vec4(0.5, 0.5, 0.5, 1.f);
				}
			}

			renderer.renderRectangle({drawSize * glm::vec2(i,j), drawSize}, tiles,
				color, {}, 0, tilesAtlas.get(tileType, 0));

		}
	}
}