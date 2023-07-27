#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platformInput.h"
#include "imgui.h"
#include <iostream>
#include <sstream>
#include "imfilebrowser.h"
#include <gl2d/gl2d.h>
#include <stuff.h>
#include <adons.h>
#include <fstream>
#include <filesystem>
#include <mapGenerator.h>
#include <thread>
#undef min

int currentFollow = -1;


gl2d::Renderer2D renderer;
gl2d::Texture roverTexture;
gl2d::TextureAtlasPadding roverAtlas;
gl2d::Texture spritesTexture;
gl2d::TextureAtlasPadding spritesAtlas;
gl2d::FrameBuffer fbo;

static int acidStartTime = 100;

struct GameplayState
{
	Map map;
	std::vector<Player> players;

	int waitingForPlayerIndex = 0;
	float waitCulldown = 0;
	bool firstTime = 1;

	int borderCulldown = acidStartTime; //acid
	int currentBorderAdvance = 0;

	bool closeGame = 0;
	
	bool evictUnresponsivePlayers = 0;
	float currentWaitingTime = 5;
	bool closeGameWhenWinning = 0;

}gameplayState;

struct WinState
{
	std::string winMessage;
}winState;

std::string panicError = "";
std::string state = "";

float culldownTime = 0.5;
bool followCurrentTurn = 0;

void gameStep(float deltaTime)
{

	if (gameplayState.waitCulldown > 0)
	{
		gameplayState.waitCulldown -= deltaTime;
		state = "Culldown";
	}
	else
	{
		auto sendNextMessage = [&]()
		{
			std::string fileName = "game/" "s" +
				std::to_string(gameplayState.players[gameplayState.waitingForPlayerIndex].id) + "_" +
				std::to_string(gameplayState.players[gameplayState.waitingForPlayerIndex].currentRound)
				+ ".txt";

			std::ofstream f(fileName);

			if (!f)
			{
				panicError = "The server couldn't create a server file: " + fileName;
			}
			else
			{
				f << gameplayState.map.size.x << ' ' << gameplayState.map.size.y << "\n";

				auto &p = gameplayState.players[gameplayState.waitingForPlayerIndex];

				for (int j = 0; j < gameplayState.map.size.y; j++)
				{
					for (int i = 0; i < gameplayState.map.size.x; i++)
					{
						char c = gameplayState.map.unsafeGet({i,j});

						if (!calculateView(p.position, {i,j}, p.cameraLevel))
						{
							if (p.scannedThisTurn)
							{
								int size = 4;

								if (p.cameraLevel == 2) { size = 5; }
								if (p.cameraLevel == 3) { size = 6; }

								glm::ivec2 scanPos = p.position;
								if (p.scannedThisTurn == 1) { scanPos += glm::ivec2{0,-1} *size; }
								if (p.scannedThisTurn == 2) { scanPos += glm::ivec2{0,1} *size; }
								if (p.scannedThisTurn == 3) { scanPos += glm::ivec2{-1,0} *size; }
								if (p.scannedThisTurn == 4) { scanPos += glm::ivec2{1,0} *size; }

								if (glm::distance(glm::vec2(scanPos), glm::vec2(i, j))
									< std::sqrt(2.f) + 0.1)
								{
									//good
								}
								else
								{
									c = '?';
								}
							}
							else
							{
								c = '?';
							}
						}
						else
						{
							for (auto &p : gameplayState.players)
							{
								if (p.position == glm::ivec2{i, j})
								{
									c = '0' + p.id;
								}
							}
						}

						f << c << " ";
					}
					f << "\n";
				}
				
				f << p.position.x << " ";
				f << p.position.y << "\n";
				f << p.life << " ";
				f << p.drilLevel << " ";
				f << p.gunLevel << " ";
				f << p.wheelLevel << " ";
				f << p.cameraLevel << " ";
				f << (int)p.hasAntena << " ";
				f << (int)p.hasBatery << "\n";
				f << p.stones << " ";
				f << p.iron << " ";
				f << p.osmium << " ";

				gameplayState.waitCulldown = culldownTime;
				f.close();

				p.scannedThisTurn = false;
			}
		};


		if (gameplayState.firstTime)
		{
		
			sendNextMessage();
			gameplayState.firstTime = 0;
		}
		else
		{
			//lets try to open the file
			state = "waiting for player: " + 
				std::to_string(gameplayState.players[gameplayState.waitingForPlayerIndex].id);

			std::string fileName = "game/" "c" +
				std::to_string(gameplayState.players[gameplayState.waitingForPlayerIndex].id) + "_" +
				std::to_string(gameplayState.players[gameplayState.waitingForPlayerIndex].currentRound)
				+ ".txt";

			std::ifstream f(fileName);

			//server
			if (f)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(5));

				if (followCurrentTurn)
				{
					currentFollow = gameplayState.waitingForPlayerIndex;
				}

				gameplayState.currentWaitingTime = 5.f;
				//got the input

				auto movePlayer = [&](int index, glm::ivec2 delta)
				{
					glm::ivec2 newPos = gameplayState.players[gameplayState.waitingForPlayerIndex].position +
						delta;

					for (auto i = 0; i < gameplayState.players.size(); i++)
					{
						if (gameplayState.players[i].position == newPos) { return; }
					}

					if (newPos.x < 0 || newPos.y < 0 ||
						newPos.x >= gameplayState.map.size.x || newPos.y >= gameplayState.map.size.y)
					{
						return;
					}

					if (gameplayState.map.unsafeGet(newPos.x, newPos.y) == Tiles::Air
						|| gameplayState.map.unsafeGet(newPos.x, newPos.y) == Tiles::Base
						|| gameplayState.map.unsafeGet(newPos.x, newPos.y) == Tiles::Acid
						)
					{
						gameplayState.players[gameplayState.waitingForPlayerIndex].position =
							newPos;
					}
				};

				char c = ' ';

				auto &p = gameplayState.players[gameplayState.waitingForPlayerIndex];

				int movementsRemaining = p.wheelLevel;
				int miningRemaining = p.drilLevel;
				bool didAction = 0;
				bool didMine = 0;

				int phaze = 0;
				while (f >> c)
				{
					switch (std::toupper(c))
					{
					case 'U':
					if (phaze == 0 && movementsRemaining)
					{
						movePlayer(gameplayState.waitingForPlayerIndex, {0,-1});
						movementsRemaining--;
					}
					break;

					case 'D':
					if (phaze == 0 && movementsRemaining)
					{
						movePlayer(gameplayState.waitingForPlayerIndex, {0,1});
						movementsRemaining--;
					}
					break;

					case 'L':
					if (phaze == 0 && movementsRemaining)
					{
						movePlayer(gameplayState.waitingForPlayerIndex, {-1,0});
						movementsRemaining--;
					}
					break;

					case 'R':
					if (phaze == 0 && movementsRemaining)
					{
						movePlayer(gameplayState.waitingForPlayerIndex, {1,0});
						movementsRemaining--;
					}
					break;

					case 'A':
					if ((phaze == 0 || phaze == 1) && !didAction)
					{
						phaze = 1;
						didAction = 1;
						//attack

						if (f >> c)
						{
							glm::ivec2 attackDirection = {};
							switch (std::toupper(c))
							{
							case 'U':
							attackDirection += glm::ivec2(0, -1);
							break;

							case 'D':
							attackDirection += glm::ivec2(0, 1);
							break;

							case 'L':
							attackDirection += glm::ivec2(-1, 0);
							break;

							case 'R':
							attackDirection += glm::ivec2(1, 0);
							break;

							}

							if (attackDirection != glm::ivec2{})
							{
								glm::ivec2 bulletPos = p.position;
								for (int i = p.gunLevel; i > 0; i--)
								{
									bulletPos += attackDirection;

									bool found = 0;
									for (auto &p : gameplayState.players)
									{
										if (p.position == bulletPos)
										{
											p.life -= i;
											found = true;
											break;
										}
									}
									
									if (found) { break; }
									
									if (bulletPos.x >= 0 && bulletPos.y >= 0
										&& bulletPos.x < gameplayState.map.size.x
										&& bulletPos.y < gameplayState.map.size.y
										)
									{
										auto &b = gameplayState.map.unsafeGet(bulletPos.x, bulletPos.y);

										if (b != Tiles::Air && b != Tiles::Base &&
											b!=Tiles::Acid
											)
										{
											break; //bullet hit a wall
										}
									}
								}

								


							}
						}
					}
					break;

					case 'S':
					if ((phaze == 0 || phaze == 1) && !didAction)
					{
						phaze = 1;
						didAction = 1;
						//scan

						if (f >> c)
						{
							if (p.hasAntena)
							{
								switch (std::toupper(c))
								{
								case 'U':
								p.scannedThisTurn = 1;
								break;

								case 'D':
								p.scannedThisTurn = 2;
								break;

								case 'L':
								p.scannedThisTurn = 3;
								break;

								case 'R':
								p.scannedThisTurn = 4;
								break;
								}
							}
						}
					}
					break;

					case 'M':
					//mine
					if ((phaze == 0 || phaze == 1) && 
						(!didAction || didMine))
					{
						phaze = 1;
						didAction = 1;
						didMine = 1;

						if(miningRemaining>0)
						if (f >> c)
						{
							auto playerPos = gameplayState.players[gameplayState.waitingForPlayerIndex].position;
							auto minePos = playerPos;
							switch (std::toupper(c))
							{
							case 'U':
							minePos += glm::ivec2(0, -1);
							break;

							case 'D':
							minePos += glm::ivec2(0, 1);
							break;

							case 'L':
							minePos += glm::ivec2(-1, 0);
							break;

							case 'R':
							minePos += glm::ivec2(1, 0);
							break;

							default:minePos = glm::ivec2(-1, -1);
							}

							if (minePos.x >= 0 && minePos.y >= 0
								&& minePos.x < gameplayState.map.size.x
								&& minePos.y < gameplayState.map.size.y
								)
							{
								auto &b = gameplayState.map.unsafeGet(minePos.x, minePos.y);

								if (b == Tiles::Stone || b == Tiles::Cobble_stone)
								{
									b = Tiles::Air;
									gameplayState.players[gameplayState.waitingForPlayerIndex].stones++;
								}
								else if (b == Tiles::Iron)
								{
									b = Tiles::Air;
									gameplayState.players[gameplayState.waitingForPlayerIndex].iron++;
								}
								else if (b == Tiles::Osmium)
								{
									b = Tiles::Air;
									gameplayState.players[gameplayState.waitingForPlayerIndex].osmium++;
								}
							}
						}
						miningRemaining--;
					}
					break;

					case 'P':
					if (phaze == 0 || phaze == 1)
					{
						//place
						phaze = 1;
						if (f >> c)
						{
							auto playerPos = gameplayState.players[gameplayState.waitingForPlayerIndex].position;
							auto placePos = playerPos;
							switch (std::toupper(c))
							{
							case 'U':
							placePos += glm::ivec2(0, -1);
							break;

							case 'D':
							placePos += glm::ivec2(0, 1);
							break;

							case 'L':
							placePos += glm::ivec2(-1, 0);
							break;

							case 'R':
							placePos += glm::ivec2(1, 0);
							break;

							default:placePos = glm::ivec2(-1, -1);
							}

							if (placePos.x >= 0 && placePos.y >= 0
								&& placePos.x < gameplayState.map.size.x
								&& placePos.y < gameplayState.map.size.y
								)
							{
								bool found = 0;
								for (auto &p : gameplayState.players)
								{
									if (p.position == placePos)
									{
										found = 1;
										break;
									}
								}

								if (!found)
								{
									auto &b = gameplayState.map.unsafeGet(placePos.x, placePos.y);
									if (b == Tiles::Air
										&& gameplayState.players[gameplayState.waitingForPlayerIndex].stones > 0
										)
									{
										b = Tiles::Cobble_stone;
										gameplayState.players[gameplayState.waitingForPlayerIndex].stones--;
									}
								}

								
							}
						}
					}
					break;

					case 'B':
					{
						phaze = 2;

						if (f >> c)
						{
							if (p.hasAntena || p.position == p.spawnPoint)
							{


								switch (std::toupper(c))
								{
								case 'S':
									if (p.cameraLevel < 3)
								{
									if (p.cameraLevel == 1)
									{
										if (p.iron >= 3)
										{
											p.iron -= 3;
											p.cameraLevel++;
										}
									}
									else if (p.cameraLevel == 2)
									{
										if (p.iron >= 6 && p.osmium >= 1)
										{
											p.iron -= 6;
											p.osmium -= 1;
											p.cameraLevel++;
										}
									}
								}
								break;

								case 'A':
								if (p.gunLevel < 3)
							{
								if (p.gunLevel == 1)
								{
									if (p.iron >= 3)
									{
										p.iron -= 3;
										p.gunLevel++;
									}
								}
								else if (p.gunLevel == 2)
								{
									if (p.iron >= 6 && p.osmium >= 1)
									{
										p.iron -= 6;
										p.osmium -= 1;
										p.gunLevel++;
									}
								}
							}
								break;

								case 'D':
								if (p.drilLevel < 3)
							{
								if (p.drilLevel == 1)
								{
									if (p.iron >= 3)
									{
										p.iron -= 3;
										p.drilLevel++;
									}
								}
								else if (p.drilLevel == 2)
								{
									if (p.iron >= 6 && p.osmium >= 1)
									{
										p.iron -= 6;
										p.osmium -= 1;
										p.drilLevel++;
									}
								}
							}
								break;

								case 'M':
								if (p.wheelLevel < 3)
							{
								if (p.wheelLevel == 1)
								{
									if (p.iron >= 3)
									{
										p.iron -= 3;
										p.wheelLevel++;
									}
								}
								else if (p.wheelLevel == 2)
								{
									if (p.iron >= 6 && p.osmium >= 1)
									{
										p.iron -= 6;
										p.osmium -= 1;
										p.wheelLevel++;
									}
								}
							}
								break;

								case 'R':
									if(!p.hasAntena)
								{
									if (p.iron >= 2 && p.osmium >= 1)
									{
										p.iron -= 2;
										p.osmium -= 1;
										p.hasAntena = 1;
									}
								}
								break;

								case 'B':
								if (!p.hasBatery)
							{
								if (p.iron >= 1 && p.osmium >= 1)
								{
									p.iron -= 1;
									p.osmium -= 1;
									p.hasBatery = 1;
								}
							}
								break;

								case 'H':
								if (p.life != MAX_ROVER_LIFE)
							{
								if (p.osmium >= 1)
								{
									p.osmium -= 1;
									p.life += 5;
									p.life = std::min(p.life, MAX_ROVER_LIFE);
								}
							}
								break;

								}

							};

						}

					}

					break;

					}
				}

				f.close();

				//advance this players turn since we got the input
				gameplayState.players[gameplayState.waitingForPlayerIndex].currentRound++;

				//next player please
				gameplayState.waitingForPlayerIndex++;
				gameplayState.waitingForPlayerIndex %= gameplayState.players.size();

				if (gameplayState.waitingForPlayerIndex == 0)
				{
					gameplayState.borderCulldown--;

					if (gameplayState.borderCulldown <= 0)
					{
						gameplayState.borderCulldown = 2;

						if (gameplayState.currentBorderAdvance <
							std::min(gameplayState.map.size.x, gameplayState.map.size.y) / 2 - 1)
						{
							for (int i = 0; i < gameplayState.map.size.x; i++)
							{
								gameplayState.map.unsafeGet(i, gameplayState.currentBorderAdvance) = Tiles::Acid;
								gameplayState.map.unsafeGet(i, gameplayState.map.size.y-1 - gameplayState.currentBorderAdvance) = Tiles::Acid;
							}

							for (int i = 0; i < gameplayState.map.size.y; i++)
							{
								gameplayState.map.unsafeGet(gameplayState.currentBorderAdvance, i) = Tiles::Acid;
								gameplayState.map.unsafeGet(gameplayState.map.size.y - 1 - gameplayState.currentBorderAdvance, i) = Tiles::Acid;
							}

							gameplayState.currentBorderAdvance++;
						}
					}
				}

				sendNextMessage();

				for (int i = 0; i < gameplayState.players.size(); i++)
				{
					if (gameplayState.map.unsafeGet(gameplayState
						.players[i].position) == Tiles::Acid)
					{
						gameplayState.players[i].life--;
					}

				}
				

			}else
			if (gameplayState.evictUnresponsivePlayers)
			{
				gameplayState.currentWaitingTime -= deltaTime;
				if (gameplayState.currentWaitingTime < 0)
				{
					gameplayState.currentWaitingTime = 5;
					
					{
						gameplayState.players.erase(gameplayState.players.begin() + gameplayState.waitingForPlayerIndex);
						if (gameplayState.players.size())
							gameplayState.waitingForPlayerIndex %= gameplayState.players.size();
						gameplayState.currentWaitingTime = 5;
						sendNextMessage();
					}

				}
			};

			//kill players
			bool killedAPlayer = 0;
			for (int i = 0; i < gameplayState.players.size(); i++)
			{
				if (gameplayState.players[i].life <= 0)
				{
					winState.winMessage += std::to_string(gameplayState.players[i].id) +
						" died ";
					killedAPlayer = true;

					if (gameplayState.waitingForPlayerIndex == i)
					{
						gameplayState.players.erase(gameplayState.players.begin() + gameplayState.waitingForPlayerIndex);
						i--;
						if (gameplayState.players.size())
							gameplayState.waitingForPlayerIndex %= gameplayState.players.size();
						gameplayState.currentWaitingTime = 5;

						sendNextMessage();
					}
					else if (gameplayState.waitingForPlayerIndex > i)
					{
						gameplayState.players.erase(gameplayState.players.begin() + gameplayState.waitingForPlayerIndex);
						i--;
						gameplayState.waitingForPlayerIndex--;
					}
					else
					{
						gameplayState.players.erase(gameplayState.players.begin() + gameplayState.waitingForPlayerIndex);
						i--;
					}
				}
			}

			if (killedAPlayer)
			{
				winState.winMessage += "\n";
			}


		}

		

	}

}

bool initGame()
{
	//initializing stuff for the renderer
	gl2d::init();

	
	fbo.create(500, 500);

	renderer.create(fbo.fbo);
	
	roverTexture.loadFromFileWithPixelPadding(RESOURCES_PATH "rover.png", 128, true);
	roverAtlas = gl2d::TextureAtlasPadding(5, 5, roverTexture.GetSize().x, roverTexture.GetSize().y);

	spritesTexture.loadFromFileWithPixelPadding(RESOURCES_PATH "sprites.png", 128, true);
	spritesAtlas = gl2d::TextureAtlasPadding(10, 1, spritesTexture.GetSize().x, spritesTexture.GetSize().y);

	
	return true;
}

void gameWindow()
{

	ImGui::PushID(69);

	ImGui::SetNextWindowSize(ImVec2{(float)500, (float)500}, ImGuiCond_Appearing);

	if (!ImGui::Begin("Gameplay"))
	{
		ImGui::End();
		ImGui::PopID();
		return;
	}

	auto size = ImGui::GetWindowSize();

	auto fboSize = fbo.texture.GetSize();
	if (size.x != fboSize.x || size.y != fboSize.y)
	{
		fbo.resize(size.x, size.y);
	}

	{
		ImGui::Separator();

		if (ImGui::BeginChild(6996, {}, false, ImGuiWindowFlags_HorizontalScrollbar))
		{
			ImGui::Image((void *)fbo.texture.id, size,
				{0, 1}, {1, 0}, {1,1,1,1}, {0.2,0.2,0.2,0.9});

			ImGui::EndChild();
		}
	}

	ImGui::End();
	ImGui::PopID();

}

void sendManualCommand()
{
	std::string lastErrState = "";

	ImGui::PushID(6991);

	ImGui::SetNextWindowSize(ImVec2{(float)500, (float)500}, ImGuiCond_Appearing);

	if (!ImGui::Begin("Send Manual Command"))
	{
		ImGui::End();
		ImGui::PopID();
		return;
	}

	ImGui::Text(lastErrState.c_str());
	ImGui::Separator();

	static int currentPlayerId = 0;
	ImGui::SliderInt("Current Player id", &currentPlayerId, 0, 4);

	int foundIndex = -1;
	for (int i = 0; i < gameplayState.players.size(); i++)
	{
		if (gameplayState.players[i].id == currentPlayerId)
		{
			foundIndex = i;
			break;
		}
	}

	if (foundIndex >= 0)
	{
		ImGui::Separator();
		ImGui::Text("Movement Phaze:");
		
		static int move1 = 0;
		static int move2 = 0;
		static int move3 = 0;
		ImGui::Combo("Move1", &move1, "none\0up\0down\0left\0right\0");
		if (gameplayState.players[foundIndex].wheelLevel >= 2)
		{
			ImGui::Combo("Move2", &move2, "none\0up\0down\0left\0right\0");
		}
		if (gameplayState.players[foundIndex].wheelLevel >= 3)
		{
			ImGui::Combo("Move3", &move3, "none\0up\0down\0left\0right\0");
		}

		ImGui::Separator();
		static int action = 0;

		static int mine1 = 0;
		static int mine2 = 0;
		static int mine3 = 0;

		static int place = 0;
		static int attack = 0;
		static int scan = 0;

		if (!gameplayState.players[foundIndex].hasAntena)
		{
			ImGui::Combo("Action", &action, "none\0mine\0place\0attack\0");
		}
		else
		{
			ImGui::Combo("Action", &action, "none\0mine\0place\0attack\0scan\0");
		}

		if (action == 1)
		{
			ImGui::Combo("Mine1", &mine1, "none\0up\0down\0left\0right\0");
			if (gameplayState.players[foundIndex].drilLevel >= 2)
			{
				ImGui::Combo("Mine2", &mine2, "none\0up\0down\0left\0right\0");
			}
			if (gameplayState.players[foundIndex].drilLevel >= 3)
			{
				ImGui::Combo("Mine3", &mine3, "none\0up\0down\0left\0right\0");
			}
		}else
		if (action == 2) //place
		{
			ImGui::Combo("Place", &place, "up\0down\0left\0right\0");
		}
		else if (action == 3)
		{
			ImGui::Combo("Attack", &attack, "up\0down\0left\0right\0");
		}
		else if (action == 4)
		{
			ImGui::Combo("Scan", &scan, "up\0down\0left\0right\0");
		}


		ImGui::Separator();
		if(ImGui::Button("send"))
		{
			std::string ourFileName = "game/c" + std::to_string(currentPlayerId) + "_" +
				std::to_string(gameplayState.players[foundIndex].currentRound) +
				".txt";
			std::ofstream response(ourFileName);

			if (response)
			{
				const char *letters = " udlr";
				if (move1) { response << letters[move1] << " "; }
				if (move2) { response << letters[move2] << " "; }
				if (move3) { response << letters[move3] << " "; }

				//mine
				if (action == 1)
				{
					if (mine1) { response << "m " << letters[mine1] << " "; }
					if (mine2) { response << "m " << letters[mine2] << " "; }
					if (mine3) { response << "m " << letters[mine3] << " "; }
				}else if(action == 2)
				{
					response << "p " << letters[place+1] << " ";
				}
				else if (action == 3)
				{
					response << "a " << letters[attack + 1] << " ";
				}
				else if (action == 4)
				{
					response << "s " << letters[scan + 1] << " ";
				}

				response << '\n';
				response.close();
				lastErrState = "Command written";
			}
			else
			{
				lastErrState = "Couldn't open file";
			}


		}

	}

	ImGui::End();
	ImGui::PopID();
}


float cameraZoom = 1;

//vector pos not id
bool simulateFog = false;

ImVec4 colors[] = {
		ImVec4{1,0,0,1},
		ImVec4{1,1,0,1},
		ImVec4{1,0,1,1},
		ImVec4{1,1,1,1},
		ImVec4{0,1,1,1},
		ImVec4{0,0,1,1},
};
int selected[100] = {};


void sideWindow()
{
	ImGui::PushID(100);

	ImGui::SetNextWindowSize(ImVec2{(float)450, (float)250}, ImGuiCond_Appearing);

	if (!ImGui::Begin("Game Controller"))
	{
		ImGui::End();
		ImGui::PopID();
		return;
	}

	ImGui::Text(state.c_str());
	ImGui::Separator();

	ImGui::SliderFloat("camera zoom", &cameraZoom, 0.05, 3);

	ImGui::Checkbox("simulate fog", &simulateFog);

	ImGui::Checkbox("Evict players after 5 secconds", &gameplayState.evictUnresponsivePlayers);

	ImGui::Checkbox("Close Game When Someone Won", &gameplayState.closeGameWhenWinning);
	
	ImGui::Checkbox("Follow current player", &followCurrentTurn);


	ImGui::SliderFloat("Simulation Delay", &culldownTime, 0.1, 2);



	ImGui::Separator();

	if (ImGui::Button("Set In center"))
	{
		currentFollow = -1;
	}

	static bool colorControols = 1;
	static bool allowChangePlayerStats = 0;

	ImGui::Checkbox("color controls", &colorControols);
	ImGui::Checkbox("allow change player stats", &allowChangePlayerStats);

	ImGui::Separator();

	for (int i = 0; i < gameplayState.players.size(); i++)
	{
		ImGui::PushID(i);
		
		auto &p = gameplayState.players[i];

		if(colorControols)
			palettePanel(colors, 6, {20,20}, &selected[p.id]);
		
		ImGui::Text("Player id: %d", p.id);

		if (allowChangePlayerStats)
		{
			ImGui::SliderInt("Player life: ", &p.life, 0, MAX_ROVER_LIFE, "%d", ImGuiSliderFlags_NoInput);
			ImGui::SliderInt("Player wheel level: ", &p.wheelLevel, 1, 3, "%d", ImGuiSliderFlags_NoInput);
			ImGui::SliderInt("Player drill level: ", &p.drilLevel, 1, 3, "%d", ImGuiSliderFlags_NoInput);
			ImGui::SliderInt("Player gun level: ", &p.gunLevel, 1, 3, "%d", ImGuiSliderFlags_NoInput);
			ImGui::SliderInt("Player camera level: ", &p.cameraLevel, 1, 3, "%d", ImGuiSliderFlags_NoInput);
			ImGui::Checkbox("Player has antena: ", &p.hasAntena);
			ImGui::Checkbox("Player has batery: ", &p.hasBatery);

			ImGui::InputInt("Player rock %d", &p.stones);
			ImGui::InputInt("Player iron %d", &p.iron);
			ImGui::InputInt("Player osmium %d", &p.osmium);
		}
		else
		{
			auto p2 = p;
			ImGui::SliderInt("Player life: ", &p2.life, 0, MAX_ROVER_LIFE, "%d", ImGuiSliderFlags_NoInput);
			ImGui::SliderInt("Player wheel level: ", &p2.wheelLevel, 1, 3, "%d", ImGuiSliderFlags_NoInput);
			ImGui::SliderInt("Player drill level: ", &p2.drilLevel, 1, 3, "%d", ImGuiSliderFlags_NoInput);
			ImGui::SliderInt("Player gun level: ", &p2.gunLevel, 1, 3, "%d", ImGuiSliderFlags_NoInput);
			ImGui::SliderInt("Player camera level: ", &p2.cameraLevel, 1, 3, "%d", ImGuiSliderFlags_NoInput);
			ImGui::Text("Player has antena: %s", p.hasAntena ? "Yes" : "No");
			ImGui::Text("Player has batery: %s", p.hasBatery ? "Yes" : "No");

			ImGui::Text("Player rock %d", p.stones);
			ImGui::Text("Player iron %d", p.iron);
			ImGui::Text("Player osmium %d", p.osmium);
		}


		auto c = colors[selected[p.id]];
		p.color = glm::vec3(c.x,c.y,c.z);

		ImGui::PushStyleColor(ImGuiCol_Button, c);
		if (ImGui::Button("Follow Player"))
		{
			currentFollow = i;
		}
		ImGui::PopStyleColor();

		ImGui::Separator();
		ImGui::PopID();
	}

	ImGui::Separator();
		
	ImGui::Checkbox("Close Game", &gameplayState.closeGame);

	if (gameplayState.closeGame)
	{
		if (ImGui::Button("Are you sure: Yes"))
		{
			gameplayState = {};
		}
	}

	ImGui::End();
	ImGui::PopID();
}

int seed = 0;

void mainMenuScreen()
{
	ImGui::PushID(101);

	ImGui::SetNextWindowSize(ImVec2{(float)450, (float)250}, ImGuiCond_Appearing);

	if (!ImGui::Begin("Game Creator"))
	{
		ImGui::End();
		ImGui::PopID();
		return;
	}

	static int nrOfPlayers = 1;

	static bool smallMap = 0;

	ImGui::SliderInt("Nr of players", &nrOfPlayers, 1, 5);

	ImGui::InputInt("Seed (0 for random): ", &seed);
	ImGui::Checkbox("Small map", &smallMap);

	ImGui::InputInt("Acid start time", &acidStartTime);

	if (ImGui::Button("Start Game"))
	{
		std::error_code error = {};
		std::filesystem::remove_all("game", error);
		std::filesystem::create_directory("game");

		winState = {};

		int s = seed;
		if (!s)s = time(0);
		if (smallMap)
		{
			gameplayState.map = generate_world({30,30}, s, false);
		}
		else
		{
			gameplayState.map = generate_world({45,45}, s, true);
		}

		std::vector<glm::vec2> spawnPoints;

		for (int j = 0; j < gameplayState.map.size.y; j++)
		{
			for (int i = 0; i < gameplayState.map.size.x; i++)
			{
				if (gameplayState.map.unsafeGet(i, j) == Tiles::Base)
				{
					spawnPoints.push_back({i,j});
				}
			}
		}

		std::shuffle(spawnPoints.begin(), spawnPoints.end(), std::default_random_engine(seed));

		for (int i = 0; i < nrOfPlayers; i++)
		{
			gameplayState.players.push_back(Player(spawnPoints[i]));
			gameplayState.players.back().id = i;
		}

	}

	if (!winState.winMessage.empty())
	{
		ImGui::Separator();
		ImGui::Text("LAST GAME:");
		ImGui::Text(winState.winMessage.c_str());
	}

	ImGui::End();
	ImGui::PopID();
}


bool gameLogic(float deltaTime)
{
#pragma region init stuff
	int w = 0; int h = 0;
	w = platform::getFrameBufferSizeX(); //window w
	h = platform::getFrameBufferSizeY(); //window h

	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT); //clear screen

	auto fboSize = fbo.texture.GetSize();
	fbo.clear();
	renderer.updateWindowMetrics(fboSize.x, fboSize.y);

#pragma endregion

	if (!panicError.empty())
	{
		ImGui::PushID(404);

		ImGui::SetNextWindowSize(ImVec2{(float)450, (float)250}, ImGuiCond_Appearing);

		if (!ImGui::Begin("Error :("))
		{
			ImGui::End();
			ImGui::PopID();
			return 0;
		}

		ImGui::PushStyleColor(ImGuiCol_Text, {1,0,0,1});
		ImGui::Text("This error is signaled because\nsome illegal action\nwas detected\nthat disturbed the normal\nflow of the program.");
		ImGui::Separator();
		ImGui::Text("If you get this error\nyou probably read a wrong file or something.\nFix it please.");
		ImGui::Separator();
		ImGui::Text(panicError.c_str());
		ImGui::PopStyleColor();

		ImGui::End();
		ImGui::PopID();
	}
	else
	{
		//during gameplay
		if (gameplayState.players.size())
		{
			gameStep(deltaTime);

		#pragma region camera logic

			{

				renderer.currentCamera.zoom = cameraZoom;

				glm::vec2 followPos = glm::vec2(gameplayState.map.size) * 50.f;

				if (currentFollow >= 0 && currentFollow < gameplayState.players.size())
				{
					followPos = glm::vec2(gameplayState.players[currentFollow].position) * 100.f + glm::vec2(50, 50);
				}

				renderer.currentCamera.follow(followPos, 1, 0.1, 0.1, fboSize.x, fboSize.y);


			}


		#pragma endregion

		#pragma region render stuff

			if (currentFollow >= 0)
			{
				gameplayState.map.render(renderer, spritesTexture, spritesAtlas,
					simulateFog, {gameplayState.players[currentFollow].cameraLevel},
					{gameplayState.players[currentFollow].position});
			}
			else
			{
				std::vector<int> view;
				std::vector<glm::ivec2> pos;

				for (auto &p : gameplayState.players)
				{
					view.push_back(p.cameraLevel);
					pos.push_back(p.position);
				}

				gameplayState.map.render(renderer, spritesTexture, spritesAtlas,
					simulateFog, view, pos);
			}

			

			for (int i = 0; i < gameplayState.players.size(); i++)
			{
				renderRover(renderer, roverTexture, roverAtlas, gameplayState.players[i]);
			}

			glViewport(0, 0, fboSize.x, fboSize.y);
			renderer.flush();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, w, h);

		
		#pragma endregion

			gameWindow();

			sideWindow();

			sendManualCommand();

			if (gameplayState.closeGameWhenWinning)
			{
				if (gameplayState.players.size() <= 1)
				{
					if (gameplayState.players.size())
					{
						winState.winMessage += 
						 	std::to_string(gameplayState.players[0].id) + " Won\n";
					}
					gameplayState = {};
				}
			}
		}
		else
		{
			mainMenuScreen();
		}
	}





	return true;
#pragma endregion

}

//This function might not be be called if the program is forced closed
void closeGame()
{


}
