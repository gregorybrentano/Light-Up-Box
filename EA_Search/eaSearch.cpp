#include <fstream>
#include <string>
#include <ctime>
#include <iostream>
#include "evolutionaryOperators.hpp"

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
	for (int x = 0; x < luBox.bulbCoordinates.size(); x++)
	{
		solutionOutput << luBox.bulbCoordinates[x].xCoord + 1 << " " << luBox.bulbCoordinates[x].yCoord + 1 << endl;
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
		else if (tempTag == "(Parent) Fitness Proportional Selection/k-Tournament Selection (f/k)")
		{
			if (tempString == "f")
				tempConfig.parentSelection = 1;
			else if(tempString == "k")
				tempConfig.parentSelection = 2;
			else
			{
				cout << "Invalid Configuration File" << endl;

				tempConfig.validConfig = false;
			}
		}
		else if (tempTag == "(Parent) Tournament Size")
			tempConfig.parentTournSize = stoi(tempString.c_str());
		else if (tempTag == "(Survival) Truncation/k-Tournament Selection (t/k)")
		{
			if (tempString == "t")
				tempConfig.survivalSelection = 1;
			else if(tempString == "k")
				tempConfig.survivalSelection = 2;
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
	luBox.fitness = 0;
	
	if(!(illuminateAllBulbs(luBox)))
		return;
	else
	{
		for (int y = 0; y < luBox.rows; y++)
		{
			for (int x = 0; x < luBox.columns; x++)
			{
				if (luBox.box[x][y] == 'B' || luBox.box[x][y] == 'L')
					luBox.fitness++;
			}
		}
	}

	luBox.fitness += luBox.numBlackBoxes;

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
	logOutput << "Population Size: " << configSettings.populationSize << endl;
	logOutput << "Number of Offspring per Generation: " << configSettings.numOffspring << endl;
	logOutput << "Mutation Rate: " << configSettings.mutationRate << endl;
	
	logOutput << "Parent Selection Operator: ";
	if(configSettings.parentSelection == 1) 
		logOutput << "Fitness Proportional Selection" << endl;
	else
		logOutput << "k-Tournament Selection with replacement" << endl;

	logOutput << "Survival Selection Operator: ";
	if(configSettings.survivalSelection == 1) 
		logOutput << "Truncation" << endl;
	else
		logOutput << "k-Tournament Selection without replacement" << endl;

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

		if (configSettings.adjacencyReq)
			{
				if (adjacencyChecker(population[i]))
					fitnessEvaluation(population[i]);
				else
					population[i].fitness = 0;
			}
			else
				fitnessEvaluation(population[i]);
	}

	return;
}

int main(int argc, char **argv)
{
	int numFitEvals, evalStuck;
	float averageFitness;
	bool stuck, converged = false;
	string configFileName;
	config configSettings;
	lightUpBox initialBox;
	lightUpBox tempBox, bestBox, runBestBox;
	ofstream logOutput, statOutput;
	vector<lightUpBox> population, offspring;
	vector<int> parents;
	vector<float> avgFitAtEvalsPerRun, bestFitAtEvalsPerRun;

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
	
	bestBox.fitness = -1;

	avgFitAtEvalsPerRun.resize((configSettings.numEvals - configSettings.populationSize) / configSettings.numOffspring + 2);
	bestFitAtEvalsPerRun.resize((configSettings.numEvals - configSettings.populationSize) / configSettings.numOffspring + 2);

	for(int i = 0; i < avgFitAtEvalsPerRun.size(); i++)
	{
		avgFitAtEvalsPerRun[i] = 0;
		bestFitAtEvalsPerRun[i] = 0;
	}
	
	for (int i = 0; i < configSettings.numRuns; i++)
	{
		stuck = true;
		numFitEvals = 0;
		evalStuck = 0;
		averageFitness = 0;
		runBestBox.fitness = -1;
		population.clear();
		converged = false;

		if (configSettings.puzzleGen)
			initialBox = generateRandomPuzzle(configSettings);

		getNumBlackBoxes(initialBox);

		logOutput << "Run " << i + 1 << endl;
		cout << "Run " << i + 1 << endl;

		populationInitialization(population, configSettings, initialBox);

		numFitEvals += population.size();

		for(int x = 0; x < population.size(); x++)
		{
			averageFitness += population[x].fitness;

			if (population[x].fitness > runBestBox.fitness)
			{
				runBestBox = population[x];

				if (runBestBox.fitness > bestBox.fitness)
					bestBox = runBestBox;
			}
		}

		averageFitness /= population.size();

		logOutput << numFitEvals << "\t" << averageFitness << "\t" << runBestBox.fitness << endl;

		do
		{
			stuck = true;
			averageFitness = 0;
			offspring.resize(configSettings.numOffspring);

			for(int x = 0; x < offspring.size(); x++)
				offspring[x] = initialBox;

			if(configSettings.parentSelection == 1)
				parents = fitnessProportionalSelection(population, configSettings);
			else
				parents = tournamentSelectionParent(population, configSettings);
			
			for(int x = 0; x < parents.size(); x += 2)
				recombination(population[parents[x]], population[parents[x + 1]], offspring[x], offspring[x + 1], configSettings);

			for(int x = 0; x < offspring.size(); x++)
			{
				mutateOffspring(offspring[x], configSettings);

				if (configSettings.adjacencyReq)
				{
					if (adjacencyChecker(offspring[x]))
						fitnessEvaluation(offspring[x]);
					else
						offspring[x].fitness = 0;
				}
				else
					fitnessEvaluation(offspring[x]);

				population.push_back(offspring[x]);

				numFitEvals++;
			}

			if(configSettings.survivalSelection == 1)
				truncation(population, configSettings);
			else
				tournamentSelectionSurvival(population, configSettings);

			for(int x = 0; x < population.size(); x++)
			{
				averageFitness += population[x].fitness;

				if (population[x].fitness > runBestBox.fitness)
				{
					runBestBox = population[x];
					evalStuck = 0;
					stuck = false;

					if (runBestBox.fitness > bestBox.fitness)
						bestBox = runBestBox;
				}
			}

			averageFitness /= population.size();

			logOutput << numFitEvals << "\t" << averageFitness << "\t" << runBestBox.fitness << endl;

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
	
	printSolution(bestBox, configSettings);

	return 0;
}
