#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#include "evolutionaryOperators.hpp"

const int INT_WIDTH = 6;
const int FLOAT_WIDTH = 10;

using namespace std;

//Prints the solution to the solution file
void printSolution(const vector<lightUpBox> & topFront, const config & configSettings)
{
	ofstream solutionOutput(configSettings.solutionFile);

	solutionOutput << topFront[0].columns << "\n" << topFront[0].rows << endl;
	
	//Prints puzzle
	for (int x = 0; x < topFront[0].columns; x++)
	{
		for (int y = 0; y < topFront[0].rows; y++)
		{
			if (topFront[0].box[x][y] >= '0' && topFront[0].box[x][y] <= '5')
				solutionOutput << x + 1 << " " << y + 1 << " " << topFront[0].box[x][y] << endl;
		}
	}

	for(int i = 0; i < topFront.size(); i++)
	{
		solutionOutput << topFront[i].fitness << "\t" << topFront[i].shineInvalids << "\t" << topFront[i].adjInvalids << "\t" << topFront.size() << endl;

		//Prints bulb locations
		for (int x = 0; x < topFront[i].bulbCoordinates.size(); x++)
		{
			solutionOutput << topFront[i].bulbCoordinates[x].xCoord + 1 << " " << topFront[i].bulbCoordinates[x].yCoord + 1 << endl;
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
		else if (tempTag == "Population Size")
			tempConfig.populationSize = stoi(tempString.c_str());
		else if (tempTag == "Number of Offspring")
			tempConfig.numOffspring = stoi(tempString.c_str());
		else if (tempTag == "Mutation Rate (.00 - 1.00)")
			tempConfig.mutationRate = stof(tempString.c_str());
		else if (tempTag == "(Initialization) Validity Forced (y/n)")
		{
			if (tempString == "y")
				tempConfig.validityForced = true;
			else
				tempConfig.validityForced = false;
		}
		else if (tempTag == "(Parent) Fitness Proportional Selection/k-Tournament Selection/Uniform Random (f/k/u)")
		{
			if (tempString == "f")
				tempConfig.parentSelection = 1;
			else if(tempString == "k")
				tempConfig.parentSelection = 2;
			else if(tempString == "u")
				tempConfig.parentSelection = 3;
			else
			{
				cout << "Invalid Configuration File" << endl;

				tempConfig.validConfig = false;
			}
		}
		else if (tempTag == "(Parent) Tournament Size")
			tempConfig.parentTournSize = stoi(tempString.c_str());
		else if(tempTag == "(Survival Strategy) Plus/Comma (p/c)")
		{
			if (tempString == "p")
				tempConfig.survivalStrategy = 1;
			else if(tempString == "c")
				tempConfig.survivalStrategy = 2;
			else
			{
				cout << "Invalid Configuration File" << endl;

				tempConfig.validConfig = false;
			}
		}
		else if (tempTag == "(Survival) Truncation/k-Tournament Selection/Fitness Proportional Selection/Uniform Random (t/k/f/u)")
		{
			if (tempString == "t")
				tempConfig.survivalSelection = 1;
			else if(tempString == "k")
				tempConfig.survivalSelection = 2;
			else if(tempString == "f")
				tempConfig.survivalSelection = 3;
			else if(tempString == "u")
				tempConfig.survivalSelection = 4;
			else
			{
				cout << "Invalid Configuration File" << endl;

				tempConfig.validConfig = false;
			}
		}
		else if (tempTag == "(Survival) Tournament Size")
			tempConfig.survivalTournSize = stoi(tempString.c_str());
		else if (tempTag == "(Termination) Convergence Termination (y/n)")
		{
			if (tempString == "y")
				tempConfig.convergenceTerm = true;
			else
				tempConfig.convergenceTerm = false;
		}
		else if (tempTag == "(Termination) Evaluations of No Improvement Before Termination")
			tempConfig.convergenceNum = stoi(tempString.c_str());
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
	tempLightUpBox.adjInvalids = 0;
	tempLightUpBox.shineInvalids = 0;
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

//Places bulbs where there is only one valid configuration of bulb placement for a black box
void validityForced(lightUpBox & luBox)
{
	int bulbsPossible, bulbsRequired;
	string tempString;
	bulbLocation tempLocation;

	for(int x = 0; x < luBox.columns; x++)
	{
		for(int y = 0; y < luBox.rows; y++)
		{
			if(luBox.box[x][y] == '0')
				placeBlockN(luBox, x, y, 1, 1, 1, 1);
		}
	}

	for(int x = 0; x < luBox.columns; x++)
	{
		for(int y = 0; y < luBox.rows; y++)
		{
			bulbsPossible = 0;

			if(luBox.box[x][y] >= '1' && luBox.box[x][y] <= '4')
			{
				tempString.clear();
				tempString.push_back(luBox.box[x][y]);
				bulbsRequired = stoi(tempString);

				if((y < luBox.rows - 1) && luBox.box[x][y + 1] == 'D')
					bulbsPossible++;
				if((x < luBox.columns - 1) && luBox.box[x + 1][y] == 'D')
					bulbsPossible++;
				if((y > 0) && luBox.box[x][y - 1] == 'D')
					bulbsPossible++;
				if((x > 0) && luBox.box[x - 1][y] == 'D')
					bulbsPossible++;
				
				if(bulbsPossible == bulbsRequired)
				{
					if((y < luBox.rows - 1) && luBox.box[x][y + 1] == 'D')
					{
						luBox.box[x][y + 1] = 'B';

						tempLocation.xCoord = x;
						tempLocation.yCoord = y + 1;

						luBox.bulbCoordinates.push_back(tempLocation);
					}	
					if((x < luBox.columns - 1) && luBox.box[x + 1][y] == 'D')
					{
						luBox.box[x + 1][y] = 'B';

						tempLocation.xCoord = x + 1;
						tempLocation.yCoord = y;
						
						luBox.bulbCoordinates.push_back(tempLocation);
					}	
					if((y > 0) && luBox.box[x][y - 1] == 'D')
					{
						luBox.box[x][y - 1] = 'B';

						tempLocation.xCoord = x;
						tempLocation.yCoord = y - 1;
						
						luBox.bulbCoordinates.push_back(tempLocation);
					}	
					if((x > 0) && luBox.box[x - 1][y] == 'D')
					{
						luBox.box[x - 1][y] = 'B';

						tempLocation.xCoord = x - 1;
						tempLocation.yCoord = y;
						
						luBox.bulbCoordinates.push_back(tempLocation);
					}	
				}
			}	
		}
	}

	for(int x = 0; x < luBox.columns; x++)
	{
		for(int y = 0; y < luBox.rows; y++)
		{
			if(luBox.box[x][y] == 'N')
				luBox.box[x][y] = 'D';
		}
	}

	randomBulbPlacement(luBox);

	return;
}

//Checks and returns whether or not all adjacency requirements are met
bool adjacencyChecker(lightUpBox & luBox, const bool constraintChecker)
{
	int numBulbsAdj, tempInvalids = luBox.adjInvalids;
	string tempString;

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
				{
					if(constraintChecker)
					{
						tempString = luBox.box[x][y];
						luBox.adjInvalids += abs(stoi(tempString) - numBulbsAdj);
					}
					else
						return false;
				}	
				else if (luBox.box[x][y] == '2' && numBulbsAdj != 2)
				{
					if(constraintChecker)
					{
						tempString = luBox.box[x][y];
						luBox.adjInvalids += abs(stoi(tempString) - numBulbsAdj);
					}
					else
						return false;
				}
				else if (luBox.box[x][y] == '3' && numBulbsAdj != 3)
				{
					if(constraintChecker)
					{
						tempString = luBox.box[x][y];
						luBox.adjInvalids += abs(stoi(tempString) - numBulbsAdj);
					}
					else
						return false;
				}
				else if (luBox.box[x][y] == '4' && numBulbsAdj != 4)
				{
					if(constraintChecker)
					{
						tempString = luBox.box[x][y];
						luBox.adjInvalids += abs(stoi(tempString) - numBulbsAdj);
					}
					else
						return false;
				}
				else if (luBox.box[x][y] == '0' && numBulbsAdj != 0)
				{
					if(constraintChecker)
					{
						tempString = luBox.box[x][y];
						luBox.adjInvalids += abs(stoi(tempString) - numBulbsAdj);
					}
					else
						return false;
				}
			}
		}
	}

	if(tempInvalids < luBox.adjInvalids)
		return false;

	return true;
}

//Checks to see if there are bulbs shining on each other
void shineChecker(lightUpBox & luBox, const config configSettings)
{
	luBox.shineInvalids = 0;
	luBox.adjInvalids = 0;

	for(int x = 0; x < luBox.columns; x++)
	{
		for(int y = 0; y < luBox.rows; y++)
		{
			if (y < luBox.rows - 1 && luBox.box[x][y] == 'B')
			{
				for (int i = y + 1; i < luBox.rows; i++)
				{
					if (luBox.box[x][i] == 'B')
					{
						luBox.shineInvalids++;
						break;
					}	
					else if(luBox.box[x][i] != 'D' && luBox.box[x][i] != 'L')
						break;
				}
			}

			if (x < luBox.columns - 1 && luBox.box[x][y] == 'B')
			{
				for (int i = x + 1; i < luBox.columns; i++)
				{
					if (luBox.box[i][y] == 'B')
					{
						luBox.shineInvalids++;
						break;
					}	
					else if (luBox.box[i][y] != 'D' && luBox.box[i][y] != 'L')
						break;
				}
			}
		}
	}

	return;
}

//Gets the fitness value of the light up box by counting the number of lit boxes, 'L', and bulbs, 'B'
void fitnessEvaluation(lightUpBox & luBox, const config configSettings)
{
	luBox.fitness = 0;
	
	illuminateAllBulbs(luBox);
	for (int y = 0; y < luBox.rows; y++)
	{
		for (int x = 0; x < luBox.columns; x++)
		{
			if (luBox.box[x][y] == 'B' || luBox.box[x][y] == 'L')
				luBox.fitness++;
		}
	}

	luBox.fitness += luBox.numBlackBoxes;

	shineChecker(luBox, configSettings);
	adjacencyChecker(luBox, true);

	luBox.shineInvalids *= -1;
	luBox.adjInvalids *= -1;

	return;
}

//Sets the number of black boxes in the light up box
void getNumBlackBoxes(lightUpBox & luBox)
{
	luBox.numBlackBoxes = 0;

	for(int x = 0; x < luBox.box.size(); x++)
	{
		for(int y = 0; y < luBox.box[x].size(); y++)
		{
			if(luBox.box[x][y] != 'D')
				luBox.numBlackBoxes++;
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
	logOutput << "Evolutionary Algorithm Paramenters: " << endl;

	if(configSettings.validityForced)
		logOutput << "Initialization: Validity Forced plus Uniform Random" << endl;
	else
		logOutput << "Initialization: Uniform Random" << endl;

	logOutput << "Population Size: " << configSettings.populationSize << endl;
	logOutput << "Number of Offspring per Generation: " << configSettings.numOffspring << endl;
	logOutput << "Mutation Rate: " << configSettings.mutationRate << endl;
		
	logOutput << "Parent Selection Operator: ";
	if(configSettings.parentSelection == 1) 
		logOutput << "Fitness Proportional Selection" << endl;
	else if(configSettings.parentSelection == 2)
		logOutput << "k-Tournament Selection with replacement" << endl;
	else if(configSettings.parentSelection == 3)
		logOutput << "Uniform Random Selection" << endl;

	if(configSettings.survivalStrategy == 1)
		logOutput << "Survival Strategy: Plus" << endl;
	else
		logOutput << "Survival Strategy: Comma" << endl;

	logOutput << "Survival Selection Operator: ";
	if(configSettings.survivalSelection == 1) 
		logOutput << "Truncation" << endl;
	else if(configSettings.survivalSelection == 2) 
		logOutput << "k-Tournament Selection without replacement" << endl;
	else if(configSettings.survivalSelection == 3) 
		logOutput << "Fitness Proportional Selection" << endl;
	else if(configSettings.survivalSelection == 4) 
		logOutput << "Uniform Random Selection" << endl;

	logOutput << "\nResult Log\n";

	return;
}

//Sets the initial population of each run
void populationInitialization(vector<lightUpBox> & population, const config configSettings, const lightUpBox initialBox)
{
	for(int i = 0; i < configSettings.populationSize; i++)
	{
		population.push_back(initialBox);

		if(configSettings.validityForced)
			validityForced(population[i]);
		else
			randomBulbPlacement(population[i]);

		fitnessEvaluation(population[i], configSettings);
	}

	return;
}

//Compares two fronts to see which one has a higher proportion that dominates at least one member of the other front. Returns true if a is better than b
bool compareFronts(const vector<lightUpBox> & a, const vector<lightUpBox> & b)
{
	int numADominates = 0;
	int numBDominates = 0;
	float propA, propB;

	if(b.size() == 0)
		return true;

	for(int i = 0; i < a.size(); i++)
	{
		for(int j = 0; j < b.size(); j++)
		{
			if(dominates(a[i], b[j]))
			{
				numADominates++;
				break;
			}
		}
	}

	for(int i = 0; i < b.size(); i++)
	{
		for(int j = 0; j < a.size(); j++)
		{
			if(dominates(b[i], a[j]))
			{
				numBDominates++;
				break;
			}
		}
	}

	propA = static_cast<float>(numADominates) / static_cast<float>(a.size());
	propB = static_cast<float>(numBDominates) / static_cast<float>(b.size());

	if(propA > propB)
		return true;
	else
		return false;
}

int main(int argc, char **argv)
{
	int numFitEvals, evalStuck, tempX, tempY;
	float averageFitness, averageAdj, averageShine;
	bool stuck, converged = false;
	string configFileName;
	config configSettings;
	lightUpBox initialBox;
	lightUpBox tempBox, bestBox, runBestBox;
	ofstream logOutput, statOutput;
	vector<lightUpBox> population, offspring, localTopFront, bestTopFront;
	vector<vector<lightUpBox> > paretoFront;
	vector<int> parents;

	if (argc > 1)
		configFileName = argv[1];
	else
		configFileName = "./configs/default.cfg";

	configSettings = readConfig(configFileName);

	if(!(configSettings.validConfig))
		return 0;
	
	if(!(configSettings.puzzleGen))
		initialBox = readPuzzleFile(configSettings.puzzleFile);

	logOutput.open(configSettings.logFile);
	setLogHeader(logOutput, configSettings);

	bestTopFront.clear();
	
	for (int i = 0; i < configSettings.numRuns; i++)
	{
		stuck = true;
		numFitEvals = 0;
		evalStuck = 0;
		averageFitness = 0;
		averageAdj = 0;
		averageShine = 0;
		runBestBox.fitness = -1;
		runBestBox.adjInvalids = -1 * (runBestBox.columns * runBestBox.rows);
		runBestBox.shineInvalids = runBestBox.adjInvalids;
		population.clear();
		converged = false;
		
		if (configSettings.puzzleGen)
			initialBox = generateRandomPuzzle(configSettings);
		
		getNumBlackBoxes(initialBox);

		logOutput << "Run " << i + 1 << endl;
		cout << "Run " << i + 1 << endl;
		
		populationInitialization(population, configSettings, initialBox);
		paretoFront = setParetoFront(population);
		localTopFront = paretoFront[0];

		numFitEvals += population.size();

		for(int x = 0; x < population.size(); x++)
		{
			averageFitness += population[x].fitness;
			averageAdj += population[x].adjInvalids;
			averageShine += population[x].shineInvalids;

			if (population[x].fitness > runBestBox.fitness)
				runBestBox.fitness = population[x].fitness;

			if (population[x].adjInvalids > runBestBox.adjInvalids)
				runBestBox.adjInvalids = population[x].adjInvalids;
			
			if (population[x].shineInvalids > runBestBox.shineInvalids)
				runBestBox.shineInvalids = population[x].shineInvalids;
		}

		if(compareFronts(paretoFront[0], bestTopFront))
			bestTopFront = paretoFront[0];

		averageFitness /= population.size();
		averageAdj /= population.size();
		averageShine /= population.size();

		logOutput << setw(INT_WIDTH) <<numFitEvals << setw(FLOAT_WIDTH) << averageFitness << setw(INT_WIDTH) << runBestBox.fitness;
		logOutput << setw(FLOAT_WIDTH) << averageShine << setw(INT_WIDTH) << runBestBox.shineInvalids;
		logOutput << setw(FLOAT_WIDTH) << averageAdj << setw(INT_WIDTH) << runBestBox.adjInvalids << endl;

		do
		{
			stuck = true;
			averageFitness = 0;
			averageAdj = 0;
			averageShine = 0;
			offspring.resize(configSettings.numOffspring);
			parents.clear();

			for(int x = 0; x < offspring.size(); x++)
				offspring[x] = initialBox;

			if(configSettings.parentSelection == 1)
				parents = fitnessProportionalSelection(configSettings, true, paretoFront);
			else if(configSettings.parentSelection == 2)
				parents = tournamentSelectionParent(configSettings, paretoFront);
			else
			{
				for(int x = 0; x < configSettings.numOffspring; x++)
					parents.push_back(uniformRandomSelection(paretoFront));

			}

			for(int x = 0; x < parents.size(); x += 2)
			{
				for(int y = 0; y < paretoFront.size(); y++)
    			{
      				if(parents[x] < paretoFront[y].size())
      				{
						tempX = y;
						tempY = parents[x];
						break;
      				}
      				else
      				{
        				parents[x] -= paretoFront[y].size();
      				}
    			}

				for(int y = 0; y < paretoFront.size(); y++)
    			{
      				if(parents[x + 1] < paretoFront[y].size())
      				{
						recombination(paretoFront[tempX][tempY], paretoFront[y][parents[x + 1]], offspring[x], offspring[x + 1], configSettings);
						break;
      				}
      				else
      				{
        				parents[x + 1] -= paretoFront[y].size();
      				}
    			}
			}
				
			if(configSettings.survivalStrategy == 2)
				population.clear();

			for(int x = 0; x < offspring.size(); x++)
			{
				mutateOffspring(offspring[x], configSettings);

				fitnessEvaluation(offspring[x], configSettings);

				addToParetoFront(paretoFront, offspring[x]);

				numFitEvals++;
			}

			if(configSettings.survivalSelection == 1)
				truncation(population, configSettings, paretoFront);
			else if (configSettings.survivalSelection == 2)
				tournamentSelectionSurvival(population, configSettings, paretoFront);
			else if (configSettings.survivalSelection == 3)
				fitnessProportionalSurvivalSelection(population, configSettings, paretoFront);
			else
				randomSurvival(population, configSettings, paretoFront);

			for(int x = 0; x < population.size(); x++)
			{
				averageFitness += population[x].fitness;
				averageAdj += population[x].adjInvalids;
				averageShine += population[x].shineInvalids;

				if (population[x].fitness > runBestBox.fitness)
					runBestBox.fitness = population[x].fitness;

				if (population[x].adjInvalids > runBestBox.adjInvalids)
					runBestBox.adjInvalids = population[x].adjInvalids;
			
				if (population[x].shineInvalids > runBestBox.shineInvalids)
					runBestBox.shineInvalids = population[x].shineInvalids;	
			}

			for(int x = 0; x < localTopFront.size(); x++)
			{
				if(x < paretoFront[0].size())
				{
					if(localTopFront[x].box != paretoFront[0][x].box)
					{
						evalStuck = 0;
						stuck = false;
						localTopFront = paretoFront[0];
						break;
					}
				}
			}

			if(compareFronts(paretoFront[0], bestTopFront))
				bestTopFront = paretoFront[0];

			averageFitness /= population.size();
			averageAdj /= population.size();
			averageShine /= population.size();

			logOutput << setw(INT_WIDTH) <<numFitEvals << setw(FLOAT_WIDTH) << averageFitness << setw(INT_WIDTH) << runBestBox.fitness;
			logOutput << setw(FLOAT_WIDTH) << averageShine << setw(INT_WIDTH) << runBestBox.shineInvalids;
			logOutput << setw(FLOAT_WIDTH) << averageAdj << setw(INT_WIDTH) << runBestBox.adjInvalids << endl;

			if(configSettings.convergenceTerm && stuck)
			{
				evalStuck += offspring.size();

				if(evalStuck >= configSettings.convergenceNum)
					converged = true;
			}

		}while((numFitEvals < configSettings.numEvals) && !(converged));

		logOutput << endl;
	}
	
	logOutput.close();
	
	printSolution(bestTopFront, configSettings);

	return 0;
}
