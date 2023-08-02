#include <iostream> 
#include <fstream>
#include <string>
#include <thread>
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
			//sleep a little to make sure that the file was written bt the server
			std::this_thread::sleep_for(std::chrono::milliseconds(5));

			//it is our turn to move
			//read the file...

			input.close();

			//write the response back
			std::string ourFileName = "game/c" + std::to_string(id) + "_" + std::to_string(round) + 
				".txt";
			std::ofstream response(ourFileName);
			//..
			//response << "M U\n";

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

