#include <stuff.h>


void renderRover(gl2d::Renderer2D &renderer, 
	gl2d::Texture &roverTexture, gl2d::TextureAtlasPadding &roverAtlas,
	glm::vec2 pos, glm::vec3 color,
	bool hasAntena, int wheelLevel, int drilLevel, int gunLevel, int life, bool hasBatery)
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

}

void renderRover(gl2d::Renderer2D &renderer, gl2d::Texture &roverTexture, 
	gl2d::TextureAtlasPadding &roverAtlas, Player &player)
{
	renderRover(renderer, roverTexture, roverAtlas, player.position * 100,
		player.color, player.hasAntena, player.wheelLevel,
		player.drilLevel, player.gunLevel, player.life, player.hasBatery);
}


void Map::render(gl2d::Renderer2D & renderer, gl2d::Texture & tiles,
	gl2d::TextureAtlasPadding &tilesAtlas)
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

			default: tileType = 9;
			}

			renderer.renderRectangle({drawSize * glm::vec2(i,j), drawSize}, tiles,
				Colors_White, {}, 0, tilesAtlas.get(tileType, 0));

		}
	}
}