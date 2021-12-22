#pragma once
#include <string>
#include <ios>
#include <sstream>
#include <iostream>
#include <fstream>
namespace UCI {
	class UCIOptions
	{
	public:
	    bool ponder = true;
		UCIOptions();

		std::string listOptions();
		void setoption(std::istringstream& is, std::ofstream& logFile);
	};
}


