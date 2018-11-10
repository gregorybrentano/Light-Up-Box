#include <vector>
#include <string>

//Holds all of the configurations settings
struct config
{
	bool validConfig;
	bool puzzleGen;
	bool adjacencyReq;
	int puzzleGenRow;
	int puzzleGenColumn;
	std::string puzzleFile;
	bool timeSeed;
	int seedNum;
	int numRuns;
	int numEvals;
	std::string logFile;
	std::string solutionFile;
};

//Holds the x and y coordinates for a single bulb
struct bulbLocation
{
	int xCoord;
	int yCoord;
};

//Holds all important information for each light up box
struct lightUpBox
{
	std::vector<std::vector<char> > box;
	std::vector<bulbLocation>  bulbCoordinates;
	int numBlackBoxes;
	int rows;
	int columns;
	int fitness;
	int invalids;
};