#include "puzzleGeneration.hpp"
#include <algorithm>

//Randomly selects an individual
int uniformRandomSelection(const std::vector<lightUpBox> population)
{
  return rand() % population.size();
}

//Selects parents for reproduction or survivors using fitness proportional selection
std::vector<int> fitnessProportionalSelection(const std::vector<lightUpBox> population, const config configSettings, const bool parent)
{
  std::vector<int> tempFitness, parentIndex;
  float sumFitness, minFitness, selector;
  int numSelections;

  tempFitness.clear();
  parentIndex.clear();

  selector = 0;
  minFitness = population[0].rows * population[0].columns;
  sumFitness = 0;

  if(parent)
    numSelections = configSettings.numOffspring;
  else
    numSelections = configSettings.populationSize;

  //Gets the fitness value of the least fit individual
  for(int i = 0; i < population.size(); i++)
  {
    if(configSettings.fitnessFunction == 1)
    {
      if(population[i].fitness < minFitness)
      {
        minFitness = population[i].fitness;
      }
    }
    else
    {
      if(population[i].penaltyFitness < minFitness)
      {
        minFitness = population[i].penaltyFitness;
      }
    }
  } 

  //Scales down the fitness of all members of the population by the value of the least fit individual and then adds a value of 1
  for(int i = 0; i < population.size(); i++)
  {
    if(configSettings.fitnessFunction == 1)
      tempFitness.push_back(population[i].fitness - minFitness + 1);
    else
      tempFitness.push_back(population[i].penaltyFitness - minFitness + 1);
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
    for(int j = 0; j < population.size(); j++)
    {
      selector -= tempFitness[j];

      if(selector <= 0)
      {
        selector -= tempFitness[j];

        //When doing survival selection, the surviving individual is removed so that it can't take more than one survival slot
        if(!(parent))
          tempFitness[j] = 0;

        parentIndex.push_back(j);

        break;
      }
    }
  }

  return parentIndex;
}

//Chooses a group of indivduals to place in a tournament and then selects the inidividual with the highest fitness and returns the index of the tournament winner
int tournamentSelection(const std::vector<lightUpBox> population, const int tournSize, const config configSettings)
{
  std::vector<int> kParentIndex;
  int tempIndex, winner;
  bool usingParent;

  kParentIndex.clear();
  winner = -(population[0].rows * population[0].columns * configSettings.penaltyCoefficient * 4);

  //Selects all of the individuals that will be used in the tournament
  while(kParentIndex.size() < tournSize)
  {
    do
    {
      usingParent = false;

      tempIndex = rand() % population.size();

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
    if(configSettings.fitnessFunction == 1)
    {
      if(population[kParentIndex[i]].fitness > winner)
      {
        tempIndex = kParentIndex[i];
        winner = population[kParentIndex[i]].fitness;
      }
    }
    else
    {
      if(population[kParentIndex[i]].penaltyFitness > winner)
      {
        tempIndex = kParentIndex[i];
        winner = population[kParentIndex[i]].penaltyFitness;
      }
    }
    
  }

  return tempIndex;
}

//Tournament selection for parent selection
std::vector<int> tournamentSelectionParent(const std::vector<lightUpBox> population, const config configSettings)
{
  std::vector<int> parentIndex;
  int tempIndex;

  parentIndex.clear();

  //Chooses parents until the desired offspring size is met
  for(int i = 0; i < configSettings.numOffspring; i++)
  {
    tempIndex = tournamentSelection(population, configSettings.parentTournSize, configSettings);

    parentIndex.push_back(tempIndex);
  }

  return parentIndex;
}

//Performs N-Point Crossover for recombination
void recombination(const lightUpBox parent1, const lightUpBox parent2, lightUpBox & offspring1, lightUpBox & offspring2, const config configSettings)
{
  int tempNum, pointsPerSwap, tempY, tempRotation, currentIndex, tempMutationRate, crossoverN = 1;
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
    //Chooses one of the placed bulbs to mutate
    if(offspring.bulbCoordinates.size() != 0)
      mutatedBulb = rand() % offspring.bulbCoordinates.size();
    else
      mutatedBulb = 0;

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

    //Gets a new randomly generated number to see if another bulb will be mutated  
    mutationCheck = rand() % 100;
  }

  return;
}

//Tournament selection for survival selection
void tournamentSelectionSurvival(std::vector<lightUpBox> & population, const config configSettings)
{
  std::vector<lightUpBox> tempPop;
  int tempIndex;

  //Chooses survivors until the desired population size is met
  while(tempPop.size() < configSettings.populationSize)
  {
    tempIndex = tournamentSelection(population, configSettings.survivalTournSize, configSettings);

    tempPop.push_back(population[tempIndex]);

    population.erase(population.begin() + tempIndex);
  }

  population.clear();

  population = tempPop;

  return;
}

//Comparing original fitness values for ordering the vector of offspring
bool compareOriginalFitness(const lightUpBox & a, const lightUpBox & b)
{
  return a.fitness > b.fitness;
}

//Comparing constraint fitness values for ordering the vector of offspring
bool compareConstraintFitness(const lightUpBox & a, const lightUpBox & b)
{
  return a.penaltyFitness > b.penaltyFitness;
}

//Truncation survival selection
void truncation(std::vector<lightUpBox> & population, const config configSettings)
{
  //Sorts the population from most fit to least fit
  if(configSettings.fitnessFunction == 1)
    sort(population.begin(), population.end(), compareOriginalFitness);
  else
    sort(population.begin(), population.end(), compareConstraintFitness);

  //Removes the least fit individuals
  while(population.size() > configSettings.populationSize)
  {
    population.pop_back();
  }

  return;
}

//Randomly chooses survivors
void randomSurvival(std::vector<lightUpBox> & population, const config configSettings)
{
  std::vector<lightUpBox> tempPop;
  int tempIndex;

  //Chooses a survivor, adds it to the new population, and then deletes it from the old population so it can't be added to the new population again
	while(tempPop.size() < configSettings.populationSize)
  {
    tempIndex = uniformRandomSelection(population);

    tempPop.push_back(population[tempIndex]);

    population.erase(population.begin() + tempIndex);
  }

  population = tempPop;
		
  return;
}

//Chooses survivors using fitness proportional selection
void fitnessProportionalSurvivalSelection(std::vector<lightUpBox> & population, const config configSettings)
{
  std::vector<lightUpBox> tempPop;
  std::vector<int> surviors;
  int tempIndex;

  //Selects all of the survivors
  surviors = fitnessProportionalSelection(population, configSettings, false);

  //Adds survivors to the new population
	for(int i = 0; i < surviors.size(); i++)
    tempPop.push_back(population[surviors[i]]);

  population = tempPop;

  return;
}