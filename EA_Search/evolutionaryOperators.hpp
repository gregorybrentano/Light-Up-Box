#include "puzzleGeneration.hpp"
#include <algorithm>

//Selects parents for reproduction using fitness proportional selection
std::vector<int> fitnessProportionalSelection(const std::vector<lightUpBox> population, const config configSettings)
{
  std::vector<int> tempFitness, parentIndex;
  int sumFitness, minFitness, selector;

  tempFitness.clear();
  parentIndex.clear();

  selector = 0;
  minFitness = 0;
  sumFitness = 0;

  for(int i = 0; i < population.size(); i++)
    if(population[i].fitness< minFitness)
      minFitness = population[i].fitness;

  for(int i = 0; i < population.size(); i++)
    tempFitness.push_back(population[i].fitness - minFitness + 1);

  for(int i = 0; i < tempFitness.size(); i++)
    sumFitness += tempFitness[i];

  for(int i = 0; i < configSettings.numOffspring; i++)
  {
    selector = rand() % sumFitness;

    for(int j = 0; j < configSettings.populationSize; j++)
    {
      selector -= tempFitness[j];

      if(selector <= 0)
      {
        sumFitness -= tempFitness[j];

        tempFitness[j] = 0;

        parentIndex.push_back(j);

        break;
      }
    }
  }

  return parentIndex;
}

//Tournament selection for parent selection
std::vector<int> tournamentSelectionParent(const std::vector<lightUpBox> population, const config configSettings)
{
  std::vector<int> parentIndex;
  std::vector<int> kParentIndex;
  int tempIndex, winner;
  bool usingParent;

  parentIndex.clear();

  while(parentIndex.size() < configSettings.numOffspring)
  {
    kParentIndex.clear();
    winner = -1;

    while(kParentIndex.size() < configSettings.parentTournSize)
    {
      do
      {
        usingParent = false;

        tempIndex = rand() % configSettings.populationSize;

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

    usingParent = false;

    for(int i = 0; i < kParentIndex.size(); i++)
    {
      if(population[kParentIndex[i]].fitness > winner)
      {
        tempIndex = kParentIndex[i];
        winner = population[kParentIndex[i]].fitness;
      }
    }

    for(int i = 0; i < parentIndex.size(); i++)
    {
      if(parentIndex[i] == tempIndex)
      {
        usingParent = true;
        
        break;
      }
    }

    if(!(usingParent))
      parentIndex.push_back(tempIndex);
  }

  return parentIndex;
}

//Recombination step of reproduction
void recombination(const lightUpBox parent1, const lightUpBox parent2, lightUpBox & offspring1, lightUpBox & offspring2, const config configSettings)
{
  int tempNum, pointsPerSwap, tempY, tempRotation, currentIndex, tempMutationRate, crossoverN = 1;
  std::vector<int> swapPoints, tempPoints;
  bool parentSwap;

  offspring1.bulbCoordinates.clear();
  offspring2.bulbCoordinates.clear();

  pointsPerSwap = parent1.bulbCoordinates.size() / (crossoverN + 1);

  if(parent2.bulbCoordinates.size() / (crossoverN + 1) < pointsPerSwap)
    pointsPerSwap = parent2.bulbCoordinates.size() / (crossoverN + 1);

  if(pointsPerSwap == 0)
    pointsPerSwap++;

  parentSwap = true;

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

  for(int i = 0; i < parent2.bulbCoordinates.size(); i++)
  {
    if(i % pointsPerSwap == 0)
      parentSwap = !(parentSwap);

    if(parentSwap && i < parent1.bulbCoordinates.size())
      offspring2.bulbCoordinates.push_back(parent1.bulbCoordinates[i]);
    else
      offspring2.bulbCoordinates.push_back(parent2.bulbCoordinates[i]);
  }

  for(int i = 0; i < offspring1.bulbCoordinates.size(); i++)
    offspring1.box[offspring1.bulbCoordinates[i].xCoord][offspring1.bulbCoordinates[i].yCoord] = 'B';
    
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

  while(mutationCheck < (configSettings.mutationRate * 100))
  {
    if(offspring.bulbCoordinates.size() != 0)
      mutatedBulb = rand() % offspring.bulbCoordinates.size();
    else
      mutatedBulb = 0;

    do
    {
      tempLocation.xCoord = rand() % offspring.columns;
      tempLocation.yCoord = rand() % offspring.rows;
    } while((offspring.box[tempLocation.xCoord][tempLocation.yCoord] >= '0' && offspring.box[tempLocation.xCoord][tempLocation.yCoord] <= '5'));

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
      
    mutationCheck = rand() % 100;
  }

  return;
}

//Tournament selection for survival selection
void tournamentSelectionSurvival(std::vector<lightUpBox> & population, const config configSettings)
{
  std::vector<int> kIndex(configSettings.survivalTournSize);
  int tempIndex, loser, tempNumber;
  bool usingIndex;

  while(population.size() > configSettings.populationSize)
  {
    kIndex.clear();
    loser = (population[0].rows * population[0].columns) + 1;

    while(kIndex.size() < configSettings.survivalTournSize)
    {
      do
      {
        usingIndex = false;

        tempIndex = rand() % population.size();

        for(int i = 0; i < kIndex.size(); i++)
        {
          if(kIndex[i] == tempIndex)
          {
            usingIndex = true;

            break;
          }
        }

        if(!(usingIndex))
          kIndex.push_back(tempIndex);

      } while(usingIndex);
    }

    for(int i = 0; i < kIndex.size(); i++)
    {
      if(population[kIndex[i]].fitness < loser)
      {
        tempIndex = kIndex[i];
        loser = population[kIndex[i]].fitness;
      }
    }

    population.erase(population.begin() + tempIndex);
  }

  return;
}

//Comparing fitness values for ordering the vector of offspring
bool compare(const lightUpBox & a, const lightUpBox & b)
{
  return a.fitness > b.fitness;
}

//Truncation survival selection
void truncation(std::vector<lightUpBox> & population, const config configSettings)
{
  sort(population.begin(), population.end(), compare);

  while(population.size() > configSettings.populationSize)
  {
    population.pop_back();
  }

  return;
}