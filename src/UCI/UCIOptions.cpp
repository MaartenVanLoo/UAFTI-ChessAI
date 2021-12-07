#include "UCIOptions.h"

UCI::UCIOptions::UCIOptions()
{

}


std::string UCI::UCIOptions::listOptions()
{
	return "No Options listed yet";
}

void UCI::UCIOptions::setoption(std::istringstream& is, std::ofstream& logFile)
{
	std::string token;
	//logFile << "Recieved: " ;
	while (is >> std::skipws >> token) // skipws = skip leading white space
	{
		//logFile << token + " ";
	}
	//logFile << std::endl;
	return;
}
