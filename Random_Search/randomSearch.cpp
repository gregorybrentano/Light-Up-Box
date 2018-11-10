#include "puzzleGeneration.hpp"
#include <fstream>
#include <ctime>
#include <iostream>

using namespace std;


//Prints the solution to the solution file
void printSolution(lightUpBox & luBox, const config & configSettings)
{
	ofstream solutionOutput(configSettings.solutionFile);

	solutionOutput << luBox.columns << "\n" << luBox.rows << endl;
	
	//Prints puzzle
	for (int x = 0; x < luBox.columns; x++)
	{
		for (int y = 0; y < luBox.rows; y++)
		{
			if (luBox.box[x][y] >= '0' && luBox.box[x][y] <= '5')
				solutionOutput << x + 1 << " " << y + 1 << " " << luBox.box[x][y] << endl;
		}
	}

	solutionOutput << luBox.fitness << endl;

	//Prints bulb locations
	for (int x = 0; x < luBox.columns; x++)
	{
		for (int y = 0; y < luBox.rows; y++)
		{
			if (luBox.box[x][y] == 'B')
				solutionOutput << x + 1 << " " << y + 1 << endl;
		}
	}

	solutionOutput.close();

	return;
}

//Determines whether a character is alphanumeric or not; Periods and forward slashes are acceptable
bool isAlphaNumeric(const char letter)
{
	if((letter >= 'a' && letter <= 'z') || (letter >= 'A' && letter <= 'Z') || (letter >= '0' && letter <= '9') || letter == '.' || letter == '/')
		return true;
	else
		return false;
}

//Sets the configuration settings from the given configuration file and seeds the random number generator
config readConfig(const string& fileName)
{
	ifstream input;
	vector<string> lines;
	string line, tempTag, tempString;
	config tempConfig;
	bool inTag;

	input.open(fileName.c_str());

	while (getline(input, line))
	{
		lines.push_back(line);
	}

	tempConfig.validConfig = true;

	for (int i = 0; i < lines.size(); i++)
	{
		inTag = true;

		//Reads in the config tag to tempTag and then the config value to tempString
		for (int x = 0; x < lines[i].length(); x++)
		{
			if (lines[i][x] != '<')
			{
				if (inTag && lines[i][x] != '>')
					tempTag.push_back(lines[i][x]);
				else if (!(inTag) && isAlphaNumeric(lines[i][x]) && x < lines[i].length())
				{
					tempString.push_back(lines[i][x]);
				}
				else if(inTag && lines[i][x] == '>')
					inTag = false;
			}
		}

		//Sets the config settings that were just read in
		if (tempTag == "Random Puzzle Generation (y/n)")
		{
			if (tempString == "y")
				tempConfig.puzzleGen = true;
			else
				tempConfig.puzzleGen = false;		
		}
		else if (tempTag == "Puzzle File")
			tempConfig.puzzleFile = tempString;
		else if (tempTag == "Puzzle Generation Rows")
			tempConfig.puzzleGenRow = stoi(tempString.c_str());
		else if (tempTag == "Puzzle Generation Columns")
			tempConfig.puzzleGenColumn = stoi(tempString.c_str());
		else if (tempTag == "Adjacency Requirement (y/n)")
		{
			if (tempString == "y")
				tempConfig.adjacencyReq = true;
			else
				tempConfig.adjacencyReq = false;
		}
		else if (tempTag == "Time Based Seed (y/n)")
		{
			if (tempString == "y")
				tempConfig.timeSeed = true;
			else
				tempConfig.timeSeed = false;
		}
		else if (tempTag == "Manual Seed")
			tempConfig.seedNum = stoi(tempString.c_str());
		else if (tempTag == "Number of Runs")
			tempConfig.numRuns = stoi(tempString.c_str());
		else if (tempTag == "Number of Fitness Evaluations")
			tempConfig.numEvals = stoi(tempString.c_str());
		else if (tempTag == "Log File")
			tempConfig.logFile = tempString;
		else if (tempTag == "Solution File")
			tempConfig.solutionFile = tempString;
		else
		{
			cout << "Invalid Configuration File" << endl;

			tempConfig.validConfig = false;
		}

		tempString.clear();
		tempTag.clear();
	}

	if (tempConfig.timeSeed)
		tempConfig.seedNum = time(NULL);

	srand(tempConfig.seedNum);

	input.close();

	return tempConfig;
}

//Sets up the initial light up box from the given puzzle file
lightUpBox readPuzzleFile(const string& fileName)
{
    ifstream input;
	vector<string> lines;
	string line, tempStringToInt;
	int tempX, tempY, tempCount;
	lightUpBox tempLightUpBox;

	tempLightUpBox.fitness = 0;
	tempLightUpBox.numBlackBoxes = 0;
	tempLightUpBox.bulbCoordinates.clear();

    input.open(fileName.c_str());
  
    while (getline(input, line))
    {
      lines.push_back(line);
    }

	for (int i = 0; i < lines.size(); i++)
	{
		tempX = 0;
		tempY = 0;
		tempCount = 0;

		if (i == 0)
		{
			//Gets the number of columns for the light up box
			for (int x = 0; x < lines[i].length(); x++)
			{
				tempStringToInt.push_back(lines[i][x]);
			}
			
			tempLightUpBox.columns = stoi(tempStringToInt.c_str());

			tempStringToInt.clear();
		}
		else if (i == 1)
		{   
			//Gets the number of rows for the light up box and creates the dimensions of the box
			for (int x = 0; x < lines[i].length(); x++)
			{
				tempStringToInt.push_back(lines[i][x]);
			}

			tempLightUpBox.rows = stoi(tempStringToInt.c_str());

			tempLightUpBox.box.resize(tempLightUpBox.columns);

			for(int x = 0; x < tempLightUpBox.box.size(); x++)
				tempLightUpBox.box[x].resize(tempLightUpBox.rows);

			tempStringToInt.clear();
		}
		//Places all of the black squares with their adjacent bulb requirement in the box
		else
		{
			while (lines[i][tempCount] != ' ')
			{
				tempStringToInt.push_back(lines[i][tempCount]);

				tempCount++;
			}
			
			tempCount++;	
			tempX = stoi(tempStringToInt.c_str()) - 1;

			tempStringToInt.clear();

			while (lines[i][tempCount] != ' ')
			{
				tempStringToInt.push_back(lines[i][tempCount]);

				tempCount++;
			}

			tempCount++;
			tempY = stoi(tempStringToInt.c_str()) - 1;

			tempStringToInt.clear();
			
			tempLightUpBox.box[tempX][tempY] = lines[i][tempCount];
		}
	}

	//Assigns all white squares to being 'D' for dark
	for (int x = 0; x < tempLightUpBox.box.size(); x++)
	{
		for (int y = 0; y < tempLightUpBox.box[x].size(); y++)
		{
			if (tempLightUpBox.box[x][y] < '0' || tempLightUpBox.box[x][y] > '5')
				tempLightUpBox.box[x][y] = 'D';
		}
	}

	tempLightUpBox.columns = tempLightUpBox.box.size();
	tempLightUpBox.rows = tempLightUpBox.box[0].size();

    return tempLightUpBox;
}

//Randomly places bulbs in the light up box
void randomBulbPlacement(lightUpBox & luBox)
{
	int bulbX, bulbY;
	bulbLocation tempBulbLocation;
	bool failedPlacement = false;

	do
	{
		bulbX = rand() % luBox.columns;
		bulbY = rand() % luBox.rows;

		tempBulbLocation.xCoord = bulbX;
		tempBulbLocation.yCoord = bulbY;

		if ((luBox.box[bulbX][bulbY] >= '0' && luBox.box[bulbX][bulbY] <= '5'))
			failedPlacement = true;
		else
		{
			luBox.box[bulbX][bulbY] = 'B';

			luBox.bulbCoordinates.push_back(tempBulbLocation);
		}

	} while(!(failedPlacement));
	
	return;
}

//Checks and returns whether or not all adjacency requirements are met
bool adjacencyChecker(lightUpBox & luBox)
{
	int numBulbsAdj;

	for (int y = 0; y < luBox.rows; y++)
	{
		for (int x = 0; x < luBox.columns; x++)
		{
			numBulbsAdj = 0;
			if (luBox.box[x][y] >= '0' && luBox.box[x][y] <= '4')
			{
				if (y < luBox.rows - 1)
				{
					if (luBox.box[x][y + 1] == 'B')
						numBulbsAdj++;
				}
				
				if (y > 0)
				{
					if (luBox.box[x][y - 1] == 'B')
						numBulbsAdj++;
				}
				
				if (x < luBox.columns - 1)
				{
					if (luBox.box[x + 1][y] == 'B')
						numBulbsAdj++;
				}
				
				if (x > 0)
				{
					if (luBox.box[x - 1][y] == 'B')
						numBulbsAdj++;
				}

				if (luBox.box[x][y] == '1' && numBulbsAdj != 1)
					return false;
				else if (luBox.box[x][y] == '2' && numBulbsAdj != 2)
					return false;
				else if (luBox.box[x][y] == '3' && numBulbsAdj != 3)
					return false;
				else if (luBox.box[x][y] == '4' && numBulbsAdj != 4)
					return false;
				else if (luBox.box[x][y] == '0' && numBulbsAdj != 0)
					return false;
			}
		}
	}

	return true;
}

//Gets the fitness value of the light up box by counting the number of lit boxes, 'L', and bulbs, 'B'
void fitnessEvaluation(lightUpBox & luBox)
{
	for (int y = 0; y < luBox.rows; y++)
	{
		for (int x = 0; x < luBox.columns; x++)
		{
			if (luBox.box[x][y] == 'B' || luBox.box[x][y] == 'L')
				luBox.fitness++;
		}
	}

	return;
}

//Puts the header at the top of the log file
void setLogHeader(ofstream & logOutput, const config configSettings)
{
	if (configSettings.puzzleGen)
	{
		logOutput << "Randomly Generated Puzzle Boxes \n";
		logOutput << "Number of Rows: " << configSettings.puzzleGenRow << "\n";
		logOutput << "Number of Columns: " << configSettings.puzzleGenColumn << "\n";
	}
	else
		logOutput << "Puzzle File: " << configSettings.puzzleFile << "\n";

	logOutput << "Random Number Generator Seed: " << configSettings.seedNum << endl;
	logOutput << "\nResult Log\n";

	return;
}

int main(int argc, char **argv)
{
	string configFileName;
	config configSettings;
	lightUpBox initialBox;
	lightUpBox tempBox, bestBox, runBestBox;
	ofstream logOutput;

	if (argc > 1)
		configFileName = argv[1];
	else
		configFileName = "./configs/default.cfg";

	configSettings = readConfig(configFileName);
	initialBox = readPuzzleFile(configSettings.puzzleFile);

	logOutput.open(configSettings.logFile);

	bestBox.fitness = -1;

	setLogHeader(logOutput, configSettings);

	for (int i = 0; i < configSettings.numRuns; i++)
	{
		if (configSettings.puzzleGen)
			initialBox = generateRandomPuzzle(configSettings);

		runBestBox.fitness = -1;

		logOutput << "Run " << i + 1 << endl;
		cout << "Run " << i + 1 << endl;

		for (int x = 0; x < configSettings.numEvals; x++)
		{
			tempBox = initialBox;

			randomBulbPlacement(tempBox);

			if (configSettings.adjacencyReq)
			{
				if (adjacencyChecker(tempBox))
					fitnessEvaluation(tempBox);
			}
			else
				fitnessEvaluation(tempBox);

			if (tempBox.fitness > runBestBox.fitness)
			{
				runBestBox = tempBox;

				logOutput << x + 1 << "\t" << runBestBox.fitness << endl;

				if (runBestBox.fitness > bestBox.fitness)
					bestBox = runBestBox;
			}
		}

		logOutput << endl;
	}

	logOutput.close();

	printSolution(bestBox, configSettings);

	return 0;
}
