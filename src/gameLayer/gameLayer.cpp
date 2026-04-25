#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platformInput.h"
#include "imgui.h"
#include <iostream>
#include <sstream>
#include <cctype>
#include "imfilebrowser.h"
#include <gl2d/gl2d.h>
#include <stuff.h>
#include <adons.h>
#include <fstream>
#include <filesystem>
#include <mapGenerator.h>
#include <thread>
#ifdef _WIN32 
#include <raudio.h>
#endif
#undef min

constexpr int centerCameraFollow = -1;
constexpr int barycenterCameraFollow = -2;
constexpr int freeCameraFollow = -3;

int currentFollow = centerCameraFollow;
glm::vec2 freeCameraPosition = {};


gl2d::Renderer2D renderer;
gl2d::Texture roverTexture;
gl2d::TextureAtlasPadding roverAtlas;
gl2d::Texture spritesTexture;
gl2d::TextureAtlasPadding spritesAtlas;
gl2d::FrameBuffer fbo;
gl2d::Font font;

#ifdef _WIN32 
Sound killSound;
Sound susSound;
Sound meetingSound;
Sound startSound;
#endif


static int acidStartTime = 300;
static constexpr float maxRoverResponseWaitTime = 5.f;
static constexpr const char *gameRecordFolder = "game record";
static constexpr const char *gameRecordMagic = "MARS_MISSION_RECORD_V1";

enum SessionMode
{
	Session_None,
	Session_LiveGame,
	Session_Playback,
};

struct RecordedFrame
{
	Map map;
	std::vector<Player> players;
	int waitingForPlayerIndex = 0;
	int borderCulldown = 0;
	bool firstTimeAcid = 1;
	int currentBorderAdvance = 0;
};

struct PlaybackState
{
	std::vector<RecordedFrame> frames;
	size_t currentFrame = 0;
	std::string title;
	int seed = 0;
};


struct GameplayState
{
	Map map;
	std::vector<Player> players;

	int waitingForPlayerIndex = 0;
	float waitCulldown = 0;
	bool firstTime = 1;

	int borderCulldown = acidStartTime; //acid
	bool firstTimeAcid = 1;
	int currentBorderAdvance = 0;

	bool closeGame = 0;
	
	bool automaticallyKickUnrespondingRovers = 0;
	bool currentRoverIsUnresponsive = 0;
	float currentWaitingTime = maxRoverResponseWaitTime;
	bool closeGameWhenWinning = 0;
	bool pause = 1;


}gameplayState;

struct WinState
{
	std::string winMessage;
}winState;

std::string panicError = "";
std::string state = "";

float culldownTime = 0;
bool followCurrentTurn = 0;
float manualCameraMoveSpeed = 900.f;

SessionMode currentSessionMode = Session_None;
PlaybackState playbackState = {};
std::filesystem::path currentRecordingPath = {};
std::string currentRecordingTitle = {};

void resetCurrentRoverWaitingState();

std::string sanitizeRecordingTitle(std::string title)
{
	while (!title.empty() && std::isspace((unsigned char)title.front()))
	{
		title.erase(title.begin());
	}

	while (!title.empty() && std::isspace((unsigned char)title.back()))
	{
		title.pop_back();
	}

	if (title.empty())
	{
		title = "untitled";
	}

	const std::string invalidCharacters = "<>:\"/\\|?*";
	for (auto &c : title)
	{
		if ((unsigned char)c < 32 || invalidCharacters.find(c) != std::string::npos)
		{
			c = '_';
		}
	}

	return title;
}

std::filesystem::path getRecordingPathForTitle(const std::string &title)
{
	return std::filesystem::path(gameRecordFolder) /
		(sanitizeRecordingTitle(title) + ".txt");
}

void closeCurrentSession()
{
	currentSessionMode = Session_None;
	playbackState = {};
	currentRecordingPath.clear();
	currentRecordingTitle.clear();
	gameplayState = {};
	state.clear();
	currentFollow = centerCameraFollow;
	freeCameraPosition = {};
}

std::string buildPlaybackStateText()
{
	if (playbackState.frames.empty())
	{
		return "Playback";
	}

	return "Playback: " + playbackState.title + " [" +
		std::to_string(playbackState.currentFrame + 1) + "/" +
		std::to_string(playbackState.frames.size()) + "]";
}

void applyRecordedFrameToGameplayState(const RecordedFrame &frame)
{
	gameplayState.map = frame.map;
	gameplayState.players = frame.players;
	gameplayState.borderCulldown = frame.borderCulldown;
	gameplayState.firstTimeAcid = frame.firstTimeAcid;
	gameplayState.currentBorderAdvance = frame.currentBorderAdvance;
	gameplayState.waitingForPlayerIndex = frame.waitingForPlayerIndex;

	if (gameplayState.players.empty())
	{
		gameplayState.waitingForPlayerIndex = 0;
	}
	else
	{
		gameplayState.waitingForPlayerIndex = std::clamp(
			gameplayState.waitingForPlayerIndex, 0,
			(int)gameplayState.players.size() - 1);
	}

	resetCurrentRoverWaitingState();

	if (currentFollow >= 0 &&
		currentFollow >= (int)gameplayState.players.size())
	{
		currentFollow = centerCameraFollow;
	}

	state = buildPlaybackStateText();
}

void jumpToPlaybackFrame(size_t frameIndex, bool restartDelay = true)
{
	if (playbackState.frames.empty())
	{
		return;
	}

	playbackState.currentFrame = std::min(frameIndex, playbackState.frames.size() - 1);
	applyRecordedFrameToGameplayState(playbackState.frames[playbackState.currentFrame]);

	if (followCurrentTurn && !gameplayState.players.empty())
	{
		currentFollow = gameplayState.waitingForPlayerIndex;
	}

	if (restartDelay)
	{
		gameplayState.waitCulldown = culldownTime;
	}
}

bool startRecordingFile(const std::string &title, int seed, int initialPlayers)
{
	currentRecordingTitle = sanitizeRecordingTitle(title);
	currentRecordingPath = getRecordingPathForTitle(currentRecordingTitle);

	std::error_code error = {};
	std::filesystem::create_directories(std::filesystem::path(gameRecordFolder), error);

	std::ofstream record(currentRecordingPath, std::ios::trunc);
	if (!record)
	{
		panicError = "Couldn't create recording file: " + currentRecordingPath.string();
		return false;
	}

	record << gameRecordMagic << "\n";
	record << "SEED " << seed << "\n";
	record << "INITIAL_PLAYERS " << initialPlayers << "\n";
	return true;
}

bool appendCurrentFrameToRecording()
{
	if (currentSessionMode != Session_LiveGame || currentRecordingPath.empty())
	{
		return true;
	}

	std::ofstream record(currentRecordingPath, std::ios::app);
	if (!record)
	{
		panicError = "Couldn't append recording frame: " + currentRecordingPath.string();
		return false;
	}

	record << "FRAME\n";
	record << "WAITING_PLAYER_INDEX " << gameplayState.waitingForPlayerIndex << "\n";
	record << "BORDER_CULLDOWN " << gameplayState.borderCulldown << "\n";
	record << "FIRST_TIME_ACID " << (int)gameplayState.firstTimeAcid << "\n";
	record << "CURRENT_BORDER_ADVANCE " << gameplayState.currentBorderAdvance << "\n";
	record << "MAP_SIZE " << gameplayState.map.size.x << " " << gameplayState.map.size.y << "\n";
	record << "PLAYER_COUNT " << gameplayState.players.size() << "\n";
	record << "MAP\n";

	for (int y = 0; y < gameplayState.map.size.y; y++)
	{
		for (int x = 0; x < gameplayState.map.size.x; x++)
		{
			record << gameplayState.map.unsafeGet(x, y);
		}
		record << "\n";
	}

	record << "PLAYERS\n";
	for (auto &p : gameplayState.players)
	{
		record
			<< p.id << " "
			<< p.position.x << " " << p.position.y << " "
			<< p.life << " "
			<< (int)p.hasAntena << " " << (int)p.hasBatery << " "
			<< p.wheelLevel << " " << p.cameraLevel << " "
			<< p.gunLevel << " " << p.drilLevel << " "
			<< p.stones << " " << p.iron << " " << p.osmium << " "
			<< p.currentRound << " " << p.scannedThisTurn << " "
			<< p.spawnPoint.x << " " << p.spawnPoint.y << " "
			<< p.color.r << " " << p.color.g << " " << p.color.b << "\n";
	}

	record << "END_FRAME\n";
	return true;
}

bool expectRecordingToken(std::istream &input, const char *token)
{
	std::string readToken;
	if (!(input >> readToken))
	{
		return false;
	}

	return readToken == token;
}

bool readRecordedFrame(std::istream &input, RecordedFrame &frame)
{
	if (!expectRecordingToken(input, "WAITING_PLAYER_INDEX")) { return false; }
	if (!(input >> frame.waitingForPlayerIndex)) { return false; }

	if (!expectRecordingToken(input, "BORDER_CULLDOWN")) { return false; }
	if (!(input >> frame.borderCulldown)) { return false; }

	if (!expectRecordingToken(input, "FIRST_TIME_ACID")) { return false; }
	int firstTimeAcid = 0;
	if (!(input >> firstTimeAcid)) { return false; }
	frame.firstTimeAcid = firstTimeAcid != 0;

	if (!expectRecordingToken(input, "CURRENT_BORDER_ADVANCE")) { return false; }
	if (!(input >> frame.currentBorderAdvance)) { return false; }

	if (!expectRecordingToken(input, "MAP_SIZE")) { return false; }
	glm::ivec2 size = {};
	if (!(input >> size.x >> size.y)) { return false; }
	frame.map.blank(size, Air);

	if (!expectRecordingToken(input, "PLAYER_COUNT")) { return false; }
	int playerCount = 0;
	if (!(input >> playerCount)) { return false; }

	if (!expectRecordingToken(input, "MAP")) { return false; }
	for (int y = 0; y < size.y; y++)
	{
		std::string row;
		if (!(input >> row) || row.size() != size.x)
		{
			return false;
		}

		for (int x = 0; x < size.x; x++)
		{
			frame.map.unsafeGet(x, y) = row[x];
		}
	}

	if (!expectRecordingToken(input, "PLAYERS")) { return false; }
	frame.players.clear();
	frame.players.reserve(playerCount);
	for (int i = 0; i < playerCount; i++)
	{
		Player player = {};
		int hasAntena = 0;
		int hasBatery = 0;

		if (!(input
			>> player.id
			>> player.position.x >> player.position.y
			>> player.life
			>> hasAntena >> hasBatery
			>> player.wheelLevel >> player.cameraLevel
			>> player.gunLevel >> player.drilLevel
			>> player.stones >> player.iron >> player.osmium
			>> player.currentRound >> player.scannedThisTurn
			>> player.spawnPoint.x >> player.spawnPoint.y
			>> player.color.r >> player.color.g >> player.color.b))
		{
			return false;
		}

		player.hasAntena = hasAntena != 0;
		player.hasBatery = hasBatery != 0;
		frame.players.push_back(player);
	}

	if (!expectRecordingToken(input, "END_FRAME")) { return false; }
	return true;
}

bool loadPlaybackRecording(const std::string &title, std::string &error)
{
	auto sanitizedTitle = sanitizeRecordingTitle(title);
	auto playbackPath = getRecordingPathForTitle(sanitizedTitle);

	std::ifstream input(playbackPath);
	if (!input)
	{
		error = "Couldn't open playback: " + playbackPath.string();
		return false;
	}

	std::string token;
	if (!(input >> token) || token != gameRecordMagic)
	{
		error = "Invalid playback file: " + playbackPath.string();
		return false;
	}

	PlaybackState loadedPlayback = {};
	loadedPlayback.title = sanitizedTitle;

	while (input >> token)
	{
		if (token == "SEED")
		{
			if (!(input >> loadedPlayback.seed))
			{
				error = "Invalid playback seed in: " + playbackPath.string();
				return false;
			}
		}
		else if (token == "INITIAL_PLAYERS")
		{
			int initialPlayers = 0;
			if (!(input >> initialPlayers))
			{
				error = "Invalid initial player count in: " + playbackPath.string();
				return false;
			}
		}
		else if (token == "FRAME")
		{
			RecordedFrame frame = {};
			if (!readRecordedFrame(input, frame))
			{
				error = "Playback file is corrupted: " + playbackPath.string();
				return false;
			}

			loadedPlayback.frames.push_back(std::move(frame));
		}
		else
		{
			error = "Unexpected playback token in: " + playbackPath.string();
			return false;
		}
	}

	if (loadedPlayback.frames.empty())
	{
		error = "Playback has no frames: " + playbackPath.string();
		return false;
	}

	playbackState = std::move(loadedPlayback);
	playbackState.currentFrame = 0;

	gameplayState = {};
	gameplayState.pause = 1;
	gameplayState.waitCulldown = 0;
	currentSessionMode = Session_Playback;
	currentRecordingPath.clear();
	currentRecordingTitle.clear();
	currentFollow = centerCameraFollow;
	freeCameraPosition = {};
	applyRecordedFrameToGameplayState(playbackState.frames[0]);
	return true;
}

void playbackStep(float deltaTime)
{
	if (playbackState.frames.empty())
	{
		return;
	}

	state = buildPlaybackStateText();

	if (gameplayState.pause)
	{
		return;
	}

	if (gameplayState.waitCulldown > 0)
	{
		gameplayState.waitCulldown -= deltaTime;
		return;
	}

	if (playbackState.currentFrame + 1 < playbackState.frames.size())
	{
		jumpToPlaybackFrame(playbackState.currentFrame + 1);
	}
	else
	{
		gameplayState.pause = 1;
		state = buildPlaybackStateText() + " finished";
	}
}

void resetCurrentRoverWaitingState()
{
	gameplayState.currentWaitingTime = maxRoverResponseWaitTime;
	gameplayState.currentRoverIsUnresponsive = 0;
}

void sendNextMessageToCurrentRover()
{
	if (gameplayState.players.empty())
	{
		resetCurrentRoverWaitingState();
		return;
	}

	std::string fileNameTemp = "game/" "s" +
		std::to_string(gameplayState.players[gameplayState.waitingForPlayerIndex].id) + "_" +
		std::to_string(gameplayState.players[gameplayState.waitingForPlayerIndex].currentRound)
		+ "temp.txt";

	std::string fileName = "game/" "s" +
		std::to_string(gameplayState.players[gameplayState.waitingForPlayerIndex].id) + "_" +
		std::to_string(gameplayState.players[gameplayState.waitingForPlayerIndex].currentRound)
		+ ".txt";

	std::ofstream f(fileNameTemp);

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

		std::error_code ec = {};
		std::filesystem::rename(fileNameTemp, fileName, ec);

		p.scannedThisTurn = false;
		resetCurrentRoverWaitingState();
	}
}

void kickCurrentWaitingRover()
{
	if (gameplayState.players.empty())
	{
		resetCurrentRoverWaitingState();
		return;
	}

	gameplayState.players.erase(gameplayState.players.begin() + gameplayState.waitingForPlayerIndex);

	if (gameplayState.players.size())
	{
		gameplayState.waitingForPlayerIndex %= gameplayState.players.size();
	}
	else
	{
		gameplayState.waitingForPlayerIndex = 0;
	}

	resetCurrentRoverWaitingState();
	sendNextMessageToCurrentRover();
}

void gameStep(float deltaTime)
{
	if (gameplayState.pause)return;

	if (gameplayState.waitCulldown > 0)
	{
		gameplayState.waitCulldown -= deltaTime;
		state = "Culldown";
	}
	else
	{
		bool processedTurn = false;

		if (gameplayState.firstTime)
		{
		
			sendNextMessageToCurrentRover();
			gameplayState.firstTime = 0;

		#ifdef _WIN32 
			PlaySound(startSound);
		#endif
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
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				processedTurn = true;

				if (followCurrentTurn)
				{
					currentFollow = gameplayState.waitingForPlayerIndex;
				}

				resetCurrentRoverWaitingState();
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
							if (p.hasBatery || p.position == p.spawnPoint)
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


				if (gameplayState.players.size())
				{

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
							if (gameplayState.firstTimeAcid)
							{
							#ifdef _WIN32 
								PlaySound(meetingSound);
							#endif
								gameplayState.firstTimeAcid = 0;
							}

							gameplayState.borderCulldown = 2;

							if (gameplayState.currentBorderAdvance <
								std::min(gameplayState.map.size.x, gameplayState.map.size.y) / 2 - 1)
							{
								for (int i = 0; i < gameplayState.map.size.x; i++)
								{
									gameplayState.map.unsafeGet(i, gameplayState.currentBorderAdvance) = Tiles::Acid;
									gameplayState.map.unsafeGet(i, gameplayState.map.size.y - 1 - gameplayState.currentBorderAdvance) = Tiles::Acid;
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

					sendNextMessageToCurrentRover();

					for (int i = 0; i < gameplayState.players.size(); i++)
					{
						if (gameplayState.map.unsafeGet(gameplayState
							.players[i].position) == Tiles::Acid)
						{
							gameplayState.players[i].life--;
						}

					}

				}

			}else
			{
				if (gameplayState.currentRoverIsUnresponsive)
				{
					if (gameplayState.automaticallyKickUnrespondingRovers)
					{
						kickCurrentWaitingRover();
					}
				}
				else
				{
					gameplayState.currentWaitingTime -= deltaTime;
					if (gameplayState.currentWaitingTime < 0)
					{
						if (gameplayState.automaticallyKickUnrespondingRovers)
						{
							kickCurrentWaitingRover();
						}
						else
						{
							gameplayState.currentWaitingTime = 0;
							gameplayState.currentRoverIsUnresponsive = 1;
						}
					}
				}
			}

			//kill players
			bool killedAPlayer = 0;
			for (int i = 0; i < gameplayState.players.size(); i++)
			{
				if (gameplayState.players[i].life <= 0)
				{
					winState.winMessage += std::to_string(gameplayState.players[i].id) +
						" died ";
				#ifdef _WIN32 
					PlaySound(killSound);
				#endif

					killedAPlayer = true;

					if (gameplayState.waitingForPlayerIndex == i)
					{
						gameplayState.players.erase(gameplayState.players.begin() + i);
						i--;
						if (gameplayState.players.size())
							gameplayState.waitingForPlayerIndex %= gameplayState.players.size();
						resetCurrentRoverWaitingState();

						sendNextMessageToCurrentRover();
					}
					else if (gameplayState.waitingForPlayerIndex > i)
					{
						gameplayState.players.erase(gameplayState.players.begin() + i);
						i--;
						gameplayState.waitingForPlayerIndex--;
					}
					else
					{
						gameplayState.players.erase(gameplayState.players.begin() + i);
						i--;
					}
				}
			}

			if (killedAPlayer)
			{
				winState.winMessage += "\n";
			}

			if (processedTurn)
			{
				appendCurrentFrameToRecording();
			}


		}

		

	}

}


bool initGame()
{
	//initializing stuff for the renderer
	gl2d::init();

#ifdef _WIN32 
	killSound = LoadSound(RESOURCES_PATH "kill.ogg");
	susSound = LoadSound(RESOURCES_PATH "sus.mp3");
	meetingSound = LoadSound(RESOURCES_PATH "meeting.ogg");
	startSound = LoadSound(RESOURCES_PATH "start.ogg");
	SetSoundVolume(killSound, 0.2);
	SetSoundVolume(susSound, 0.2);
	SetSoundVolume(startSound, 0.2);
	SetSoundVolume(meetingSound, 0.2);
#endif


	font.createFromFile(RESOURCES_PATH "roboto_black.ttf");

	fbo.create(500, 500);

	renderer.create(fbo.fbo);
	
	roverTexture.loadFromFileWithPixelPadding(RESOURCES_PATH "rover.png", 128, true);
	roverAtlas = gl2d::TextureAtlasPadding(5, 5, roverTexture.GetSize().x, roverTexture.GetSize().y);

	spritesTexture.loadFromFileWithPixelPadding(RESOURCES_PATH "sprites.png", 128, true);
	spritesAtlas = gl2d::TextureAtlasPadding(10, 1, spritesTexture.GetSize().x, spritesTexture.GetSize().y);

	std::error_code error = {};
	std::filesystem::remove_all("game", error);
	std::filesystem::create_directory("game");

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


float cameraZoom = 0.198;

//vector pos not id
bool simulateFog = true;

ImVec4 colors[] = {
		ImVec4{0,0,1,1},
		ImVec4{1,1,0,1},
		ImVec4{0,1,0,1},
		ImVec4{131/255.f, 52 / 255.f, 235 / 255.f,1},
		ImVec4{1,0,0,1},
		ImVec4{1,0,1,1},
		ImVec4{5.f / 255.f, 31.f / 255.f, 32.f / 255.f, 1},
		ImVec4{29.f/255.f, 68.f / 255.f, 40.f / 255.f, 1},

};
int selected[100] = {0,1,2,3,4};


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
	if (gameplayState.firstTimeAcid)
		ImGui::Text("ACID: %d", gameplayState.borderCulldown);

	ImGui::Separator();

	ImGui::SliderFloat("camera zoom", &cameraZoom, 0.05, 3);

	ImGui::Checkbox("simulate fog", &simulateFog);

	ImGui::Checkbox("Automatically kick unresponding rovers", &gameplayState.automaticallyKickUnrespondingRovers);

	ImGui::Checkbox("Close Game When Someone Won", &gameplayState.closeGameWhenWinning);
	
	ImGui::Checkbox("Follow current player", &followCurrentTurn);
	ImGui::Text("WASD moves the camera");


	ImGui::SliderFloat("Simulation Delay", &culldownTime, 0, 2);

	ImGui::Checkbox("PAUSE", &gameplayState.pause);

	if (currentSessionMode == Session_Playback && !playbackState.frames.empty())
	{
		int playbackFrame = (int)playbackState.currentFrame;
		if (ImGui::SliderInt("Playback frame", &playbackFrame, 0,
			(int)playbackState.frames.size() - 1))
		{
			jumpToPlaybackFrame(playbackFrame);
		}

		ImGui::Text("Frame %d / %d", playbackFrame + 1,
			(int)playbackState.frames.size());
	}



	ImGui::Separator();

	if (ImGui::Button("Set In center"))
	{
		currentFollow = centerCameraFollow;
	}

	if (ImGui::Button("Set In baricenter"))
	{
		currentFollow = barycenterCameraFollow;
	}

	static bool colorControols = 1;
	static bool allowChangePlayerStats = 0;

	ImGui::Checkbox("color controls", &colorControols);
	ImGui::Checkbox("allow change player stats", &allowChangePlayerStats);

	if (!gameplayState.automaticallyKickUnrespondingRovers &&
		gameplayState.currentRoverIsUnresponsive &&
		!gameplayState.players.empty())
	{
		ImGui::Separator();
		ImGui::TextColored({1,0,0,1}, "Rover %d is unresponsive",
			gameplayState.players[gameplayState.waitingForPlayerIndex].id);

		if (ImGui::Button("Kick unresponsive rover"))
		{
			kickCurrentWaitingRover();
		}
	}

	ImGui::Separator();

	for (int i = 0; i < gameplayState.players.size(); i++)
	{
		ImGui::PushID(i);
		
		auto &p = gameplayState.players[i];

		if(colorControols)
			palettePanel(colors, sizeof(colors)/sizeof(colors[0]), {20,20}, &selected[p.id]);
		
		ImGui::Text("Player id: %d", p.id);

		if (allowChangePlayerStats)
		{
			ImGui::SliderInt("life: ", &p.life, 0, MAX_ROVER_LIFE, "%d", ImGuiSliderFlags_NoInput);
			ImGui::SliderInt("wheel level: ", &p.wheelLevel, 1, 3, "%d", ImGuiSliderFlags_NoInput);
			ImGui::SliderInt("drill level: ", &p.drilLevel, 1, 3, "%d", ImGuiSliderFlags_NoInput);
			ImGui::SliderInt("gun level: ", &p.gunLevel, 1, 3, "%d", ImGuiSliderFlags_NoInput);
			ImGui::SliderInt("camera level: ", &p.cameraLevel, 1, 3, "%d", ImGuiSliderFlags_NoInput);
			ImGui::Checkbox("antena: ", &p.hasAntena);
			ImGui::Checkbox("batery: ", &p.hasBatery);

			ImGui::InputInt("rock %d", &p.stones);
			ImGui::InputInt("iron %d", &p.iron);
			ImGui::InputInt("osmium %d", &p.osmium);
		}
		else
		{
			auto p2 = p;
			ImGui::SliderInt("life: ", &p2.life, 0, MAX_ROVER_LIFE, "%d", ImGuiSliderFlags_NoInput);
			ImGui::SliderInt("wheel level: ", &p2.wheelLevel, 1, 3, "%d", ImGuiSliderFlags_NoInput);
			ImGui::SliderInt("drill level: ", &p2.drilLevel, 1, 3, "%d", ImGuiSliderFlags_NoInput);
			ImGui::SliderInt("gun level: ", &p2.gunLevel, 1, 3, "%d", ImGuiSliderFlags_NoInput);
			ImGui::SliderInt("camera level: ", &p2.cameraLevel, 1, 3, "%d", ImGuiSliderFlags_NoInput);
			ImGui::Text("antena: %s", p.hasAntena ? "Yes" : "No");
			ImGui::Text("batery: %s", p.hasBatery ? "Yes" : "No");

			ImGui::Text("rock %d", p.stones);
			ImGui::Text("iron %d", p.iron);
			ImGui::Text("osmium %d", p.osmium);
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
			closeCurrentSession();
		}
	}

	ImGui::End();
	ImGui::PopID();
}

int seed = 0;

bool startLiveGameSession(int nrOfPlayers, bool smallMap, const std::string &title, std::string &error)
{
	std::error_code filesystemError = {};
	std::filesystem::remove_all("game", filesystemError);
	std::filesystem::create_directory("game");

	winState = {};
	playbackState = {};
	gameplayState = {};
	state.clear();
	currentFollow = centerCameraFollow;
	freeCameraPosition = {};
	currentRecordingPath.clear();
	currentRecordingTitle.clear();
	currentSessionMode = Session_None;

	int s = seed;
	if (!s) { s = time(0); }

	if (smallMap)
	{
		gameplayState.map = generate_world({35,35}, s, false);
	}
	else
	{
		gameplayState.map = generate_world({45,45}, s, true);
	}

	std::ofstream seedFile(RESOURCES_PATH "game/seed.txt");
	seedFile << s;
	seedFile.close();

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

	std::shuffle(spawnPoints.begin(), spawnPoints.end(), std::default_random_engine(time(0)));

	for (int i = 0; i < nrOfPlayers; i++)
	{
		gameplayState.players.push_back(Player(spawnPoints[i]));
		gameplayState.players.back().id = i;
	}

	if (!startRecordingFile(title, s, nrOfPlayers))
	{
		error = panicError.empty() ? "Couldn't start recording file" : panicError;
		panicError.clear();
		closeCurrentSession();
		return false;
	}

	currentSessionMode = Session_LiveGame;

	if (!appendCurrentFrameToRecording())
	{
		error = panicError.empty() ? "Couldn't write initial recording frame" : panicError;
		panicError.clear();
		closeCurrentSession();
		return false;
	}

	return true;
}

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

	static bool smallMap = 1;
	static char newGameTitle[128] = "match";
	static char playbackTitle[128] = "";
	static std::string playbackError = "";

	ImGui::SliderInt("Nr of players", &nrOfPlayers, 1, 5);

	ImGui::InputInt("Seed (0 for random): ", &seed);
	ImGui::Checkbox("Small map", &smallMap);
	ImGui::InputText("Record title", newGameTitle, IM_ARRAYSIZE(newGameTitle));

	ImGui::InputInt("Acid start time", &acidStartTime);

	//todo sa afisez ca nu se poate
	if (ImGui::Button("Start Game"))
	{
		playbackError.clear();
		startLiveGameSession(nrOfPlayers, smallMap, newGameTitle, playbackError);
	}

	ImGui::Separator();
	ImGui::InputText("Playback title", playbackTitle, IM_ARRAYSIZE(playbackTitle));

	if (ImGui::Button("Start Playback"))
	{
		playbackError.clear();
		if (!loadPlaybackRecording(playbackTitle, playbackError))
		{
			closeCurrentSession();
		}
	}

	if (!playbackError.empty())
	{
		ImGui::TextColored({1,0,0,1}, playbackError.c_str());
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

	glm::vec2 manualCameraMove = {};
	const bool allowManualCameraControl =
		currentSessionMode != Session_None;

	if (allowManualCameraControl)
	{
		if (platform::isKeyHeld(platform::Button::W))
		{
			manualCameraMove.y -= 1.f;
		}
		if (platform::isKeyHeld(platform::Button::S))
		{
			manualCameraMove.y += 1.f;
		}
		if (platform::isKeyHeld(platform::Button::A))
		{
			manualCameraMove.x -= 1.f;
		}
		if (platform::isKeyHeld(platform::Button::D))
		{
			manualCameraMove.x += 1.f;
		}

		if (manualCameraMove.x != 0.f || manualCameraMove.y != 0.f)
		{
			if (currentFollow != freeCameraFollow)
			{
				freeCameraPosition = renderer.currentCamera.position;
			}

			manualCameraMove = glm::normalize(manualCameraMove);
			freeCameraPosition +=
				manualCameraMove * (manualCameraMoveSpeed * deltaTime / cameraZoom);

			currentFollow = freeCameraFollow;
			followCurrentTurn = false;
		}
	}

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
		if (currentSessionMode != Session_None)
		{
			if (currentSessionMode == Session_LiveGame)
			{
				gameStep(deltaTime);
			}
			else if (currentSessionMode == Session_Playback)
			{
				playbackStep(deltaTime);
			}

		#pragma region camera logic

			{

				renderer.currentCamera.zoom = cameraZoom;

				if (currentFollow == freeCameraFollow)
				{
					renderer.currentCamera.position = freeCameraPosition;
				}
				else
				{
					glm::vec2 followPos = glm::vec2(gameplayState.map.size) * 50.f;

					if (currentFollow >= 0 && currentFollow < gameplayState.players.size())
					{
						followPos = glm::vec2(gameplayState.players[currentFollow].position) * 100.f + glm::vec2(50, 50);
					}
					else if (currentFollow == barycenterCameraFollow && !gameplayState.players.empty())
					{
						followPos = {};
						for (auto &i : gameplayState.players)
						{
							followPos += i.position;
						}
						followPos /= gameplayState.players.size();
						followPos *= 100.f;
					}

					renderer.currentCamera.follow(followPos, deltaTime*15000.f, 0.0, 0.0, fboSize.x, fboSize.y);
					freeCameraPosition = renderer.currentCamera.position;
				}


			}


		#pragma endregion

		#pragma region render stuff

			if (currentFollow >= 0 && currentFollow < gameplayState.players.size())
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

			if (renderer.currentCamera.zoom < 0.5)
			{
				for (int i = 0; i < gameplayState.players.size();i++)
				{
					renderer.renderText(
						glm::vec2{gameplayState.players[i].position * 100} + glm::vec2(100, -400),
						std::to_string(gameplayState.players[i].id).c_str(), font,
						glm::vec4(gameplayState.players[i].color, 0.3f), 8, 4, 3, true, {0,0,0,0.1}
					);
				}
			}

			glViewport(0, 0, fboSize.x, fboSize.y);
			renderer.flush();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, w, h);

		
		#pragma endregion

			gameWindow();

			sideWindow();

			if (currentSessionMode == Session_LiveGame)
			{
				sendManualCommand();
			}

			if (currentSessionMode == Session_LiveGame &&
				gameplayState.closeGameWhenWinning)
			{
				if (gameplayState.players.size() <= 1)
				{
					if (gameplayState.players.size())
					{
						winState.winMessage += 
						 	std::to_string(gameplayState.players[0].id) + " Won\n";
					#ifdef _WIN32 
						PlaySound(susSound);
					#endif

					}
					closeCurrentSession();
				}
			}
		}
		else
		{
			mainMenuScreen();
		}
	}

	if (platform::isKeyReleased(platform::Button::Space))
	{
		gameplayState.pause = !gameplayState.pause;
	}
	for (int i = 0; i < gameplayState.players.size(); i++)
	{
		if (platform::isKeyReleased(platform::Button::NR0 + gameplayState.players[i].id))
		{
			currentFollow = i;
		}
	}
	if (platform::isKeyReleased(platform::Button::C))
	{
		currentFollow = centerCameraFollow;
	}

	if (platform::isKeyHeld(platform::Button::Q))
	{
		cameraZoom -= 1.2*deltaTime;
	}else if (platform::isKeyHeld(platform::Button::E))
	{
		cameraZoom += 1.2 * deltaTime;
	}

	cameraZoom = glm::clamp(cameraZoom, 0.05f, 3.f);

	return true;
#pragma endregion

}

//This function might not be be called if the program is forced closed
void closeGame()
{


}
