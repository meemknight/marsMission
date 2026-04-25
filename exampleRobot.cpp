#include <iostream> 
#include <fstream>
#include <string>
#include <thread>
#include <filesystem>

using namespace std;

int main()
{
	int id = 0;
	std::cout << "enter rover id: ";
	std::cin >> id;

	int round = 0;

	while (true)
	{
		std::string serverFileName = "game/s" + std::to_string(id) + "_" + std::to_string(round) +
			".txt";

		std::ifstream input(serverFileName);

		if (input)
		{

			//it is our turn to move
			//read the file...
			int mapSizeX, mapSizeY;
			input >> mapSizeX;
			input >> mapSizeY;

			input.close();

			//write the response back into a temporary file
			std::string ourFileNameTemp = "game/c" + std::to_string(id) + "_" + std::to_string(round) + "temp.txt";
			std::string ourFileName = "game/c" + std::to_string(id) + "_" + std::to_string(round) + ".txt";
			std::ofstream response(ourFileNameTemp);

			//..
			//write your rover commands here
			// 
			response << "U M U\n";


			//close the file
			response.close();

			//rename the temporary file to the actual final file name.
			std::error_code errorCode;
			std::filesystem::rename(ourFileNameTemp, ourFileName, errorCode);


			//increment the round
			round++;
		}
		else
		{
			//waiting for other players to make their moves
		}

	}
	return 0;
}

