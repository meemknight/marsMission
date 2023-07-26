#include<iostream> 
#include <fstream>
#include <string>
using namespace std;

int main()
{
	int id = 0;
	std::cout << "enter id: ";
	std::cin >> id;

	int round = 0;

	while (true)
	{
		std::string serverFileName = "game/s" + std::to_string(id) + "_" + std::to_string(round) + 
			".txt";

		std::ifstream input(serverFileName);

		if (input)
		{
			//yes our turn to move
			//read the file...

			input.close();

			//write the response back
			std::string ourFileName = "game/c" + std::to_string(id) + "_" + std::to_string(round) + 
				".txt";
			std::ofstream response(ourFileName);
			//..
			response.close();

			//increment the round
			round++;
		}
		else
		{
			//waiting
		}
	}
	return 0;
}

