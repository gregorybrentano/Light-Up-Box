#include "puzzleGeneration.hpp"
#include <algorithm>

//Returns true if a dominates b, else returns false
bool dominates(const lightUpBox & a, const lightUpBox & b)
{
	if(a.adjInvalids > b.adjInvalids && a.fitness >= b.fitness && a.shineInvalids >= b.shineInvalids)
		return true;
	else if(a.adjInvalids >= b.adjInvalids && a.fitness > b.fitness && a.shineInvalids >= b.shineInvalids)
		return true;
	else if(a.adjInvalids >= b.adjInvalids && a.fitness >= b.fitness && a.shineInvalids > b.shineInvalids)
		return true;
	else 
		return false;
}

//Adds an individual to the Pareto Front
void addToParetoFront(std::vector<std::vector<lightUpBox> > & paretoFront, lightUpBox & individual)
{
	lightUpBox tempIndividual;
	bool domination, placed = false;

	for(int x = 0; x < paretoFront.size(); x++)
	{
		if(placed)
		{
			return;
		}

    //Checks to see if any current members dominate the individual, if not add to front
		for(int y = 0; y < paretoFront[x].size(); y++)
		{
			domination = dominates(paretoFront[x][y], individual);
			if(domination)
			{
				break;
			}
			else if(!(domination) && y == paretoFront[x].size() - 1)
			{
				paretoFront[x].push_back(individual);
				placed = true;
				break;
			}
		}
		
    //If individual is added to the front, then it checks to see if it dominates any of the current members and will kick them out if it does
		if(placed)
		{
			for(int y = 0; y < paretoFront[x].size(); y++)
			{
				domination = dominates(individual, paretoFront[x][y]);

				if(domination)
				{
					tempIndividual = paretoFront[x][y];
					paretoFront[x].erase(paretoFront[x].begin() + y);
					y--;

					addToParetoFront(paretoFront, tempIndividual);
				}
			}
		}
	}

  //If individual cannot fit in an existing front then a new front is created
	if(!(placed))
	{
		paretoFront.resize(paretoFront.size() + 1);

		paretoFront[paretoFront.size() - 1].push_back(individual);
	}

	return;
}

//Takes the members of a population and ranks them in a Pareto Front
std::vector<std::vector<lightUpBox> > setParetoFront(std::vector<lightUpBox> & population)
{
	std::vector<std::vector<lightUpBox> > paretoFront;

	for(int i = 0; i < population.size(); i++)
	{
		addToParetoFront(paretoFront, population[i]);
	}

	return paretoFront;
}

//Randomly selects an individual
int uniformRandomSelection(const std::vector<std::vector<lightUpBox> > population)
{
  int popSize = 0;
  for(int x = 0; x < population.size(); x++)
    popSize += population[x].size();

  return rand() % popSize;
}

//Selects parents for reproduction or survivors using fitness proportional selection
std::vector<int> fitnessProportionalSelection(const config configSettings, const bool parent, std::vector<std::vector<lightUpBox> > paretoFront)
{
  std::vector<int> tempFitness, parentIndex;
  float sumFitness, selector;
  int numSelections;

  tempFitness.clear();
  parentIndex.clear();

  selector = 0;
  sumFitness = 0;

  if(parent)
    numSelections = configSettings.numOffspring;
  else
    numSelections = configSettings.populationSize;

  for(int x = 0; x < paretoFront.size(); x++)
  {
    for(int y = 0; y < paretoFront[x].size(); y++)
    {
      tempFitness.push_back(paretoFront.size() - x);
    }
  }

  //Collects the total fitness level of the population
  for(int i = 0; i < tempFitness.size(); i++)
    sumFitness += tempFitness[i];

  //Chooses parents until the desired offspring size is met
  for(int i = 0; i < numSelections; i++)
  {
    //Selects a value between 0 and the total fitness for selecting a parent
    selector = rand() % static_cast<int>(sumFitness);

    //Selects a parent by subtracting an individuals fitness from selector until selector is less than or equal to 0 and then that individual is designated as the parent
    for(int j = 0; j < tempFitness.size(); j++)
    {
      selector -= tempFitness[j];

      if(selector <= 0)
      {
        selector -= tempFitness[j];

        //When doing survival selection, the surviving individual is removed so that it can't take more than one survival slot
        if(!(parent))
        {
          sumFitness-= tempFitness[j];
          tempFitness[j] = 0;
        }
          

        parentIndex.push_back(j);

        break;
      }
    }
  }

  return parentIndex;
}

//Chooses a group of indivduals to place in a tournament and then selects the inidividual with the highest fitness and returns the index of the tournament winner
int tournamentSelection(const int tournSize, const config configSettings, std::vector<std::vector<lightUpBox> > paretoFront)
{
  std::vector<int> kParentIndex;
  std::vector<int> tempFitness;
  int tempIndex, winner;
  bool usingParent;

  for(int x = 0; x < paretoFront.size(); x++)
  {
    for(int y = 0; y < paretoFront[x].size(); y++)
    {
      tempFitness.push_back(paretoFront.size() - x);
    }
  }

  kParentIndex.clear();
  winner = 0;

  //Selects all of the individuals that will be used in the tournament
  while(kParentIndex.size() < tournSize)
  {
    do
    {
      usingParent = false;

      tempIndex = rand() % tempFitness.size();

      for(int i = 0; i < kParentIndex.size(); i++)
      {
        if(kParentIndex[i] == tempIndex)
        {
          usingParent = true;
          break;
        }
      }

      if(!(usingParent))
        kParentIndex.push_back(tempIndex);

    } while(usingParent);
  }

  //Selects the individual with the highest fitness to be the tournament winner
  for(int i = 0; i < kParentIndex.size(); i++)
  {
    if(tempFitness[kParentIndex[i]] > winner)
    {
      tempIndex = kParentIndex[i];
      winner = tempFitness[tempIndex];
    }
    
  }

  return tempIndex;
}

//Tournament selection for parent selection
std::vector<int> tournamentSelectionParent(const config configSettings, std::vector<std::vector<lightUpBox> > paretoFront)
{
  std::vector<int> parentIndex;
  int tempIndex;

  parentIndex.clear();

  //Chooses parents until the desired offspring size is met
  for(int i = 0; i < configSettings.numOffspring; i++)
  {
    tempIndex = tournamentSelection(configSettings.parentTournSize, configSettings, paretoFront);

    parentIndex.push_back(tempIndex);
  }

  return parentIndex;
}

//Performs N-Point Crossover for recombination
void recombination(const lightUpBox parent1, const lightUpBox parent2, lightUpBox & offspring1, lightUpBox & offspring2, const config configSettings)
{
  int tempNum, pointsPerSwap, tempY, tempRotation, currentIndex, tempMutationRate, crossoverN = 2;
  std::vector<int> swapPoints, tempPoints;
  bool parentSwap;

  offspring1.bulbCoordinates.clear();
  offspring2.bulbCoordinates.clear();

  pointsPerSwap = parent1.bulbCoordinates.size() / (crossoverN + 1);

  //Calculates the number of bulbs to put in the offspring from one parent before switching to the other parents
  if(parent2.bulbCoordinates.size() / (crossoverN + 1) < pointsPerSwap)
    pointsPerSwap = parent2.bulbCoordinates.size() / (crossoverN + 1);

  if(pointsPerSwap == 0)
    pointsPerSwap++;

  parentSwap = true;

  //Places the bulb coordinates from the parents into the first offspring
  for(int i = 0; i < parent1.bulbCoordinates.size(); i++)
  { 
    if(i % pointsPerSwap == 0)
      parentSwap = !(parentSwap);

    if(parentSwap && i < parent2.bulbCoordinates.size())
      offspring1.bulbCoordinates.push_back(parent2.bulbCoordinates[i]);
    else
      offspring1.bulbCoordinates.push_back(parent1.bulbCoordinates[i]);
  }

  parentSwap = true;

  //Places the bulb coordinates from the parents into the first offspring
  for(int i = 0; i < parent2.bulbCoordinates.size(); i++)
  {
    if(i % pointsPerSwap == 0)
      parentSwap = !(parentSwap);

    if(parentSwap && i < parent1.bulbCoordinates.size())
      offspring2.bulbCoordinates.push_back(parent1.bulbCoordinates[i]);
    else
      offspring2.bulbCoordinates.push_back(parent2.bulbCoordinates[i]);
  }

  //Adds the bulb locations to the bulbCoordinates array for future use
  for(int i = 0; i < offspring1.bulbCoordinates.size(); i++)
    offspring1.box[offspring1.bulbCoordinates[i].xCoord][offspring1.bulbCoordinates[i].yCoord] = 'B';
  
  //Adds the bulb locations to the bulbCoordinates array for future use
  for(int i = 0; i < offspring2.bulbCoordinates.size(); i++)
    offspring2.box[offspring2.bulbCoordinates[i].xCoord][offspring2.bulbCoordinates[i].yCoord] = 'B';
  return;
}

//Mutation step of reproduction
void mutateOffspring(lightUpBox & offspring, const config configSettings)
{
  int mutationCheck, mutatedBulb;
  bulbLocation tempLocation;

  mutationCheck = rand() % 100;

  //Mutates bulbs if thr random number generated is less than the mutation rate
  while(mutationCheck < (configSettings.mutationRate * 100))
  {
    int removeBulb;

    //Chooses one of the placed bulbs to mutate
    if(offspring.bulbCoordinates.size() != 0)
      mutatedBulb = rand() % offspring.bulbCoordinates.size();
    else
      mutatedBulb = 0;

    removeBulb = rand() % 100;

    if(removeBulb < 50 && offspring.bulbCoordinates.size() != 0)
    {
      offspring.box[offspring.bulbCoordinates[mutatedBulb].xCoord][offspring.bulbCoordinates[mutatedBulb].yCoord] = 'D';
      offspring.bulbCoordinates.erase(offspring.bulbCoordinates.begin() + mutatedBulb);
    }
    else
    {
      //Finds a location to mutate the choosen bulb that isn't a black box
      do
      {
        tempLocation.xCoord = rand() % offspring.columns;
        tempLocation.yCoord = rand() % offspring.rows;
      } while((offspring.box[tempLocation.xCoord][tempLocation.yCoord] >= '0' && offspring.box[tempLocation.xCoord][tempLocation.yCoord] <= '5'));

      //Places the newly mutated bulb
      if(offspring.bulbCoordinates.size() != 0)
      {
        offspring.box[offspring.bulbCoordinates[mutatedBulb].xCoord][offspring.bulbCoordinates[mutatedBulb].yCoord] = 'D';
        offspring.bulbCoordinates[mutatedBulb] = tempLocation;
        offspring.box[offspring.bulbCoordinates[mutatedBulb].xCoord][offspring.bulbCoordinates[mutatedBulb].yCoord] = 'B';
      }
      else
      {
        offspring.bulbCoordinates.push_back(tempLocation);
        offspring.box[offspring.bulbCoordinates[mutatedBulb].xCoord][offspring.bulbCoordinates[mutatedBulb].yCoord] = 'B';
      }
    }

    //Gets a new randomly generated number to see if another bulb will be mutated  
    mutationCheck = rand() % 100;
  }

  return;
}

//Tournament selection for survival selection
void tournamentSelectionSurvival(std::vector<lightUpBox> & population, const config configSettings, std::vector<std::vector<lightUpBox> > paretoFront)
{
  std::vector<lightUpBox> tempPop;
  int tempIndex;

  //Chooses survivors until the desired population size is met
  while(tempPop.size() < configSettings.populationSize)
  {
    tempIndex = tournamentSelection(configSettings.survivalTournSize, configSettings, paretoFront);

    for(int x = 0; x < paretoFront.size(); x++)
    {
      if(tempIndex < paretoFront[x].size())
      {
        tempPop.push_back(paretoFront[x][tempIndex]);
        paretoFront[x].erase(paretoFront[x].begin() + tempIndex);
        break;
      }
      else
      {
        tempIndex -= paretoFront[x].size();
      }
    }
  }

  population.clear();
  paretoFront.clear();

  population = tempPop;

  paretoFront = setParetoFront(population);

  return;
}

//Truncation survival selection
void truncation(std::vector<lightUpBox> & population, const config configSettings, std::vector<std::vector<lightUpBox> > paretoFront)
{
  int popSize = 0;

  population.clear();

  for(int x = 0; x < paretoFront.size(); x++)
    popSize += paretoFront[x].size();

  //Removes the least fit individuals
  while(popSize > configSettings.populationSize)
  {
    if(paretoFront[paretoFront.size() - 1].size() > 0)
      paretoFront[paretoFront.size() - 1].pop_back();
    else
    {
      paretoFront.resize(paretoFront.size() - 1);
      paretoFront[paretoFront.size() - 1].pop_back();
    }

    popSize--;
  }

  for(int x = 0; x < paretoFront.size(); x++)
  {
    for(int y = 0; y < paretoFront[x].size(); y++)
    {
      population.push_back(paretoFront[x][y]);
    }
  }

  return;
}

//Randomly chooses survivors
void randomSurvival(std::vector<lightUpBox> & population, const config configSettings, std::vector<std::vector<lightUpBox> > paretoFront)
{
  std::vector<lightUpBox> tempPop;
  int tempIndex;

  //Chooses a survivor, adds it to the new population, and then deletes it from the old population so it can't be added to the new population again
	while(tempPop.size() < configSettings.populationSize)
  {
    tempIndex = uniformRandomSelection(paretoFront);

    for(int x = 0; x < paretoFront.size(); x++)
    {
      if(tempIndex < paretoFront[x].size())
      {
        tempPop.push_back(paretoFront[x][tempIndex]);
        paretoFront[x].erase(paretoFront[x].begin() + tempIndex);
        break;
      }
      else
      {
        tempIndex -= paretoFront[x].size();
      }
    }
  }

  population.clear();
  paretoFront.clear();

  population = tempPop;

  paretoFront = setParetoFront(population);
		
  return;
}

//Chooses survivors using fitness proportional selection
void fitnessProportionalSurvivalSelection(std::vector<lightUpBox> & population, const config configSettings, std::vector<std::vector<lightUpBox> > paretoFront)
{
  std::vector<std::vector<lightUpBox> > tempFront;
  std::vector<lightUpBox> tempPop;
  std::vector<int> survivors;
  int tempIndex;

  //Selects all of the survivors
  survivors = fitnessProportionalSelection(configSettings, false, paretoFront);

  //Adds survivors to the new population
	for(int i = 0; i < survivors.size(); i++)
  {
    for(int x = 0; x < paretoFront.size(); x++)
    {
      if(survivors[i] < paretoFront[x].size())
      {
        tempPop.push_back(paretoFront[x][survivors[i]]);
        break;
      }
      else
      {
        survivors[i] -= paretoFront[x].size();
      }
    }
  }

  population.clear();
  paretoFront.clear();

  population = tempPop;

  paretoFront = setParetoFront(population);

  return;
}