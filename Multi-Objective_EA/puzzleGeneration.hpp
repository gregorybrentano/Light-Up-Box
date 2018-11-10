#include "puzzleStructures.hpp"
#include <iostream>

//Illuminates in a line in all directions from a specific bulb. Returns false if a bulb shines on another bulb.
bool illuminateFromBulb(lightUpBox & luBox, const int blackX, const int blackY)
{
	//Illuminates up
	if (blackY < luBox.rows - 1 && !(luBox.box[blackX][blackY + 1] >= '0' && luBox.box[blackX][blackY + 1] <= '5'))
	{
		for (int y = blackY + 1; y < luBox.rows; y++)
		{
			if (luBox.box[blackX][y] == 'P' || luBox.box[blackX][y] == 'D')
				luBox.box[blackX][y] = 'L';
			else if (luBox.box[blackX][y] == 'B')
				return false;
			else if(luBox.box[blackX][y] != 'N' && luBox.box[blackX][y] != 'L')
				break;
		}
	}	

	//Illuminates down
	if (blackY > 0 && !(luBox.box[blackX][blackY - 1] >= '0' && luBox.box[blackX][blackY - 1] <= '5'))
	{
		for (int y = blackY - 1; y >= 0; y--)
		{
			if (luBox.box[blackX][y] == 'P' || luBox.box[blackX][y] == 'D')
				luBox.box[blackX][y] = 'L';
			else if (luBox.box[blackX][y] == 'B')
				return false;
			else if(luBox.box[blackX][y] != 'N' && luBox.box[blackX][y] != 'L')
				break;
		}
	}
	
	//Illuminates right
	if (blackX < luBox.columns - 1 && !(luBox.box[blackX + 1][blackY] >= '0' && luBox.box[blackX + 1][blackY] <= '5'))
	{
		for (int x = blackX + 1; x < luBox.columns; x++)
		{
			if (luBox.box[x][blackY] == 'P' || luBox.box[x][blackY] == 'D')
				luBox.box[x][blackY] = 'L';
			else if (luBox.box[x][blackY] == 'B')
				return false;
			else if (luBox.box[x][blackY] != 'N' && luBox.box[x][blackY] != 'L')
				break;
		}
	}

	//Illuminates left
	if (blackX > 0 && !(luBox.box[blackX - 1][blackY] >= '0' && luBox.box[blackX - 1][blackY] <= '5'))
	{
		for (int x = blackX - 1; x >= 0; x--)
		{
			if (luBox.box[x][blackY] == 'P' || luBox.box[x][blackY] == 'D')
				luBox.box[x][blackY] = 'L';
			else if (luBox.box[x][blackY] == 'B')
				return false;
			else if (luBox.box[x][blackY] != 'N' && luBox.box[x][blackY] != 'L')
				break;
		}
	}

	return true;
}

//Illuminates from all bulbs. 
void illuminateAllBulbs(lightUpBox & luBox)
{
	for(int x = 0; x < luBox.box.size(); x++)
	{
		for(int y = 0; y < luBox.box[x].size(); y++)
		{
			if(luBox.box[x][y] == 'B')
				illuminateFromBulb(luBox, x, y);
		}
	}

	return;
}

//Deluminates the whole box
void deluminateWholeBox(lightUpBox & luBox)
{
	for(int x = 0; x < luBox.box.size(); x++)
	{
		for(int y = 0; y < luBox.box[x].size(); y++)
		{
			if(luBox.box[x][y] == 'L')
				luBox.box[x][y] = 'P';
		}
	}

	return;
}

//Deluminates whole box and then re-illuminates from all bulbs
void illumationReset(lightUpBox & luBox)
{
	deluminateWholeBox(luBox);
	illuminateAllBulbs(luBox);
	
	return;
}

//Places a 'N' or No Bulb block down at the specified locations
void placeBlockN(lightUpBox & luBox, const int blackX, const int blackY, const bool up, const bool right, const bool down, const bool left)
{
	//Right
	if(right && blackX < luBox.columns - 1 && (luBox.box[blackX + 1][blackY] == 'P' || luBox.box[blackX + 1][blackY] == 'L' || luBox.box[blackX + 1][blackY] == 'D'))
		luBox.box[blackX + 1][blackY] = 'N';

	//Left
	if(left && blackX > 0 && (luBox.box[blackX - 1][blackY] == 'P' || luBox.box[blackX - 1][blackY] == 'L' || luBox.box[blackX - 1][blackY] == 'D'))
		luBox.box[blackX - 1][blackY] = 'N';

	//Up
	if(up && blackY < luBox.rows - 1 && (luBox.box[blackX][blackY + 1] == 'P' || luBox.box[blackX][blackY + 1] == 'L' || luBox.box[blackX][blackY + 1] == 'D'))
		luBox.box[blackX][blackY + 1] = 'N';

	//Down
	if(down && blackY > 0 && (luBox.box[blackX][blackY - 1] == 'P' || luBox.box[blackX][blackY - 1] == 'L' || luBox.box[blackX][blackY - 1] == 'D'))
		luBox.box[blackX][blackY - 1] = 'N';

	return;
}

//Places a 'B' or Bulb block down at the specified locations. If any of those locations are invalid it returns false.
bool placeBlockB(lightUpBox & luBox, const int blackX, const int blackY, const bool up, const bool right, const bool down, const bool left)
{
	//Places a bulb down to the right and if the bulb shines on another bulb or is out of bounds then the function returns false
	if(right && blackX < luBox.columns - 1 && luBox.box[blackX + 1][blackY] == 'P')
	{
		luBox.box[blackX + 1][blackY] = 'B';

		placeBlockN(luBox, blackX + 1, blackY, 1, 1, 1, 0);

		if(!(illuminateFromBulb(luBox, blackX + 1, blackY)))
			return false;
	}
	else if(right)
		return false;
	
	//Places a bulb down to the left and if the bulb shines on another bulb or is out of bounds then the function returns false
	if(left && blackX > 0 && luBox.box[blackX - 1][blackY] == 'P')
	{
		luBox.box[blackX - 1][blackY] = 'B';

		placeBlockN(luBox, blackX - 1, blackY, 1, 0, 1, 1);

		if(!(illuminateFromBulb(luBox, blackX - 1, blackY)))
			return false;
	}
	else if(left)
		return false;
	
	//Places a bulb above and if the bulb shines on another bulb or is out of bounds then the function returns false
	if(up && blackY < luBox.rows - 1 && luBox.box[blackX][blackY + 1] == 'P')
	{
		luBox.box[blackX][blackY + 1] = 'B';

		placeBlockN(luBox, blackX, blackY + 1, 1, 1, 0, 1);

		if(!(illuminateFromBulb(luBox, blackX, blackY + 1)))
			return false;
	}
	else if(up)
		return false;
	
	//Places a bulb below and if the bulb shines on another bulb or is out of bounds then the function returns false
	if(down && blackY > 0 && luBox.box[blackX][blackY - 1] == 'P')
	{
		luBox.box[blackX][blackY - 1] = 'B';

		placeBlockN(luBox, blackX, blackY - 1, 0, 1, 1, 1);

		if(!(illuminateFromBulb(luBox, blackX, blackY - 1)))
			return false;
	}
	else if(down)
		return false;

	return true;
}

//Places a black box at blackX,blackY with a value of 0 and designates surrounding blocks as 'N' for no bulb
bool placeBlackZero(lightUpBox & luBox, const int blackX, const int blackY)
{
	//Places new black box and deluminates
	luBox.box[blackX][blackY] = '0';
	illumationReset(luBox);
		
	//Designates surrounding blocks as 'N'
	placeBlockN(luBox, blackX, blackY, 1, 1, 1, 1);

	return true;
}

//Places a black box at blackX,blackY with a value of 1 and designates certain surrounding blocks as 'N' for no bulb
bool placeBlackOne(lightUpBox & luBox, const int blackX, const int blackY)
{
	int orientation, badOrientation = 0;
	bool goodOrientation = false;
	lightUpBox tempBox1, tempBox2 = luBox;
	std::vector<int> notTested{1, 2, 3, 4};

	//Places new black box and deluminates
	tempBox2.box[blackX][blackY] = '1';
	illumationReset(tempBox2);

	tempBox1 = tempBox2;

	//Chooses one of all possible '1' orientations for bulb placement and places in luBox if it fits
	//If it doesn't fit, then it tries other orientations, if it fails all orientations it rejects placement
	do {
		orientation = (rand() % notTested.size());

		switch (notTested[orientation])
		{
		case 1:
			if(placeBlockB(tempBox1, blackX, blackY, 1, 0, 0, 0))
				goodOrientation = true;
			else
			{
				notTested.erase(notTested.begin() + orientation);
				goodOrientation = false;
			}

			break;
		case 2:
			if(placeBlockB(tempBox1, blackX, blackY, 0, 	1, 0, 0))
				goodOrientation = true;
			else
			{
				notTested.erase(notTested.begin() + orientation);
				goodOrientation = false;
			}

			break;
		case 3:
			if(placeBlockB(tempBox1, blackX, blackY, 0, 0, 1, 0))
				goodOrientation = true;
			else
			{
				notTested.erase(notTested.begin() + orientation);
				goodOrientation = false;
			}

			break;
		case 4:
			if(placeBlockB(tempBox1, blackX, blackY, 0, 0, 0, 1))
				goodOrientation = true;
			else
			{
				notTested.erase(notTested.begin() + orientation);
				goodOrientation = false;
			}

			break;
		}

		if (!(goodOrientation))
			tempBox1 = tempBox2;

	} while (!(goodOrientation) && notTested.size() > 0);

	if (notTested.size() == 0)
		return false;

	luBox = tempBox1;

	return true;
}

//Places a black box at blackX,blackY with a value of 2 and changes surrounding blocks
bool placeBlackTwo(lightUpBox & luBox, const int blackX, const int blackY)
{
	int orientation, badOrientation = 0;
	bool goodOrientation = false;
	lightUpBox tempBox1, tempBox2 = luBox;
	std::vector<int> notTested{1, 2, 3, 4, 5, 6};

	//Places new black box and deluminates
	tempBox2.box[blackX][blackY] = '2';
	illumationReset(tempBox2);

	tempBox1 = tempBox2;

	//Chooses one of all possible '2' orientations for bulb placement and places in luBox if it fits
	//If it doesn't fit, then it tries other orientations, if it fails all orientations it rejects placement
	do {
		orientation = (rand() % notTested.size());

		switch (notTested[orientation])
		{
		case 1:
			if(placeBlockB(tempBox1, blackX, blackY, 1, 0, 0, 1))
				goodOrientation = true;
			else
			{
				notTested.erase(notTested.begin() + orientation);
				goodOrientation = false;
			}

			break;
		case 2:
			if(placeBlockB(tempBox1, blackX, blackY, 1, 1, 0, 0))
				goodOrientation = true;
			else
			{
				notTested.erase(notTested.begin() + orientation);
				goodOrientation = false;
			}

			break;
		case 3:
			if(placeBlockB(tempBox1, blackX, blackY, 1, 0, 1, 0))
				goodOrientation = true;
			else
			{
				notTested.erase(notTested.begin() + orientation);
				goodOrientation = false;
			}

			break;
		case 4:
			if(placeBlockB(tempBox1, blackX, blackY, 0, 1, 1, 0))
				goodOrientation = true;
			else
			{
				notTested.erase(notTested.begin() + orientation);
				goodOrientation = false;
			}

			break;
		case 5:
			if(placeBlockB(tempBox1, blackX, blackY, 0, 1, 0, 1))
				goodOrientation = true;
			else
			{
				notTested.erase(notTested.begin() + orientation);
				goodOrientation = false;
			}

			break;
		case 6:
			if(placeBlockB(tempBox1, blackX, blackY, 0, 0, 1, 1))
				goodOrientation = true;
			else
			{
				notTested.erase(notTested.begin() + orientation);
				goodOrientation = false;
			}

			break;
		}

		if (!(goodOrientation))
			tempBox1 = tempBox2;

	} while (!(goodOrientation) && notTested.size() > 0);

	if (notTested.size() == 0)
		return false;

	luBox = tempBox1;

	return true;
}

//Places a black box at blackX,blackY with a value of 3 and changes surrounding blocks
bool placeBlackThree(lightUpBox & luBox, const int blackX, const int blackY)
{
	int orientation, badOrientation = 0;
	bool goodOrientation = false;
	lightUpBox tempBox1, tempBox2 = luBox;
	std::vector<int> notTested{1, 2, 3, 4};

	//Places new black box and deluminates
	tempBox2.box[blackX][blackY] = '3';
	illumationReset(tempBox2);

	tempBox1 = tempBox2;

	//Chooses one of all possible '3' orientations for bulb placement and places in luBox if it fits
	//If it doesn't fit, then it tries other orientations, if it fails all orientations it rejects placement
	do {
		orientation = (rand() % notTested.size());

		switch (notTested[orientation])
		{
			case 1:
				if(placeBlockB(tempBox1, blackX, blackY, 1, 1, 1, 0))
					goodOrientation = true;
				else
				{
					notTested.erase(notTested.begin() + orientation);
					goodOrientation = false;
				}

				break;
			case 2:
				if(placeBlockB(tempBox1, blackX, blackY, 1, 1, 0, 1))
					goodOrientation = true;
				else
				{
					notTested.erase(notTested.begin() + orientation);
					goodOrientation = false;
				}

				break;
			case 3:
				if(placeBlockB(tempBox1, blackX, blackY, 1, 0, 1, 1))
					goodOrientation = true;
				else
				{
					notTested.erase(notTested.begin() + orientation);
					goodOrientation = false;
				}

				break;
			case 4:
				if(placeBlockB(tempBox1, blackX, blackY, 0, 1, 1, 1))
					goodOrientation = true;
				else
				{
					notTested.erase(notTested.begin() + orientation);
					goodOrientation = false;
				}

				break;
		}

		if (!(goodOrientation))
			tempBox1 = tempBox2;	

	} while (!(goodOrientation) && notTested.size() > 0);

	if (notTested.size() == 0)
		return false;

	luBox = tempBox1;

	return true;
}

//Places a black box at blackX,blackY with a value of 4 and changes surrounding blocks
bool placeBlackFour(lightUpBox & luBox, const int blackX, const int blackY)
{
	lightUpBox tempBox = luBox;

	//Places new black box and deluminates
	tempBox.box[blackX][blackY] = '4';
	illumationReset(luBox);

	//Places the '4' black box and turns surrounding blocks to bulbs, 'B', and no bulbs, 'N', outside of the bulbs
	if(placeBlockB(tempBox, blackX, blackY, 1, 1, 1, 1))
	{
		luBox = tempBox;

		return true;
	}
	else
		return false;
}

//Places a black box at blackX,blackY with a value of 5
bool placeBlackFive(lightUpBox & luBox, const int blackX, const int blackY)
{
    bool up, right, down, left;
	//Places new black box and deluminates
	luBox.box[blackX][blackY] = '5';
	illumationReset(luBox);

    up = rand() % 2;
    right = rand() % 2;
    down = rand() % 2;
    left = rand() % 2;

	//Bulbs are placed individually so if one fails then the others are still able to be placed
    placeBlockB(luBox, blackX, blackY, up, 0, 0, 0);
    placeBlockB(luBox, blackX, blackY, 0, right, 0, 0);
    placeBlockB(luBox, blackX, blackY, 0, 0, down, 0);
    placeBlockB(luBox, blackX, blackY, 0, 0, 0, left);

	return true;
}

//Checks to see if there are places a bulb cannot be placed, and if there are already bulbs touching the black box location
void validBulbCheck(const lightUpBox & luBox, const int blackX, const int blackY, int & impossiblePlaces, int & requiredPlaces, const bool up, const bool right, const bool down, const bool left)
{
	if(up)
	{
		if (luBox.box[blackX][blackY + 1] == 'N' || luBox.box[blackX][blackY + 1] == 'L')
			impossiblePlaces++;

		if (luBox.box[blackX][blackY + 1] == 'B')
			requiredPlaces++;
	}

	if(right)
	{
		if (luBox.box[blackX + 1][blackY] == 'N' || luBox.box[blackX + 1][blackY] == 'L')
			impossiblePlaces++;

		if (luBox.box[blackX + 1][blackY] == 'B')
			requiredPlaces++;
	}

	if(down)
	{
		if (luBox.box[blackX][blackY - 1] == 'N' || luBox.box[blackX][blackY - 1] == 'L')
			impossiblePlaces++;

		if (luBox.box[blackX][blackY - 1] == 'B')
			requiredPlaces++;
	}

	if(left)
	{
		if (luBox.box[blackX - 1][blackY] == 'N' || luBox.box[blackX - 1][blackY] == 'L')
			impossiblePlaces++;

		if (luBox.box[blackX - 1][blackY] == 'B')
			requiredPlaces++;
	}

	return;		
}

//Used for making sure a bulb is not touching an existing Black box - Bulb pairing
bool notTouching(const lightUpBox & luBox, const int blackX, const int blackY)
{
	if(blackY < luBox.rows - 1)
	{
		if(luBox.box[blackX][blackY + 1] >= '0' && luBox.box[blackX][blackY + 1] <= '3')
			return false;
	}

	if(blackY > 0)
	{
		if(luBox.box[blackX][blackY - 1] >= '0' && luBox.box[blackX][blackY - 1] <= '3')
			return false;
	}

	if(blackX < luBox.columns - 1)
	{
		if(luBox.box[blackX + 1][blackY] >= '0' && luBox.box[blackX + 1][blackY] <= '3')
			return false;
	}

	if(blackX > 0)
	{
		if(luBox.box[blackX - 1][blackY] >= '0' && luBox.box[blackX - 1][blackY] <= '3')
			return false;
	}

	return true;
}

//Randomly generates a light up puzzle using the dimensions given in the configuration file
lightUpBox generateRandomPuzzle(const config configSettings)
{
	lightUpBox luBox;
	int blackX, blackY, blackValue, numFails = 0;
	int impossiblePlaces = 0;
	int requiredPlaces = 0;
	const int MAX_BLACK_VALUE = 5;
	const int MAX_FAILS = 10;

	luBox.rows = configSettings.puzzleGenRow;
	luBox.columns = configSettings.puzzleGenColumn;
	luBox.fitness = 0;
	luBox.adjInvalids = 0;
	luBox.shineInvalids = 0;
	luBox.numBlackBoxes = 0;

	luBox.box.resize(luBox.columns);

	//Creates the appropriately sized 2D vector for the board
	for (int x = 0; x < luBox.box.size(); x++)
		luBox.box[x].resize(luBox.rows);

	//Fills the board with 'P' for possible location
	for (int x = 0; x < luBox.box.size(); x++)
		for (int y = 0; y < luBox.box[x].size(); y++)
			luBox.box[x][y] = 'P';

	//Attempts to place a black box until it fails a certain number of times
	while (numFails <= MAX_FAILS)
	{
		impossiblePlaces = 0;
		requiredPlaces = 0;
		blackX = rand() % luBox.columns;
		blackY = rand() % luBox.rows;

		if (luBox.box[blackX][blackY] != 'B' && (luBox.box[blackX][blackY] < '0' || luBox.box[blackX][blackY] > '5'))
		{
			//Sees if a '5' will be placed, if not then
			//Checks to see if there are places a bulb cannot be placed, and if there are already bulbs touching the black box location
            if ((rand() % 100) < 20)
				blackValue = 5;
			else if (blackX == luBox.columns - 1 || blackX == 0)
			{
				if (blackX == luBox.columns - 1)
					validBulbCheck(luBox, blackX, blackY, impossiblePlaces, requiredPlaces, 0, 0, 0, 1);
				else
					validBulbCheck(luBox, blackX, blackY, impossiblePlaces, requiredPlaces, 0, 1, 0, 0);


				if (blackY == luBox.rows - 1 || blackY == 0)
				{
					if (blackY == luBox.rows - 1)
						validBulbCheck(luBox, blackX, blackY, impossiblePlaces, requiredPlaces, 0, 0, 1, 0);
					else
						validBulbCheck(luBox, blackX, blackY, impossiblePlaces, requiredPlaces, 1, 0, 0, 0);

					blackValue = (rand() % (MAX_BLACK_VALUE - (2 + impossiblePlaces))) + requiredPlaces;
				}
				else
				{
					validBulbCheck(luBox, blackX, blackY, impossiblePlaces, requiredPlaces, 1, 0, 1, 0);

					blackValue = (rand() % (MAX_BLACK_VALUE - (1 + impossiblePlaces))) + requiredPlaces;
				}
			}
			else if (blackY == luBox.rows - 1 || blackY == 0)
			{
				if (blackY == luBox.rows - 1)
					validBulbCheck(luBox, blackX, blackY, impossiblePlaces, requiredPlaces, 0, 1, 1, 1);
				else
					validBulbCheck(luBox, blackX, blackY, impossiblePlaces, requiredPlaces, 1, 1, 0, 1);

				blackValue = (rand() % (MAX_BLACK_VALUE - (1 + impossiblePlaces))) + requiredPlaces;
			}
			else
			{
				if (blackX < luBox.columns - 1)
					validBulbCheck(luBox, blackX, blackY, impossiblePlaces, requiredPlaces, 1, 1, 1, 1);

				blackValue = (rand() % (MAX_BLACK_VALUE - impossiblePlaces)) + requiredPlaces;
			}

			//Attempts to place the black box with the given blackValue
			switch (blackValue)
			{
			case 0:
				if (!(placeBlackZero(luBox, blackX, blackY)))
					numFails++;

				break;
			case 1:
				if (!(placeBlackOne(luBox, blackX, blackY)))
					numFails++;

				break;
			case 2:
				if (!(placeBlackTwo(luBox, blackX, blackY)))
					numFails++;

				break;
			case 3:
				if (!(placeBlackThree(luBox, blackX, blackY)))
					numFails++;

				break;
			case 4:
				if (!(placeBlackFour(luBox, blackX, blackY)))
					numFails++;

				break;
			case 5:
				if(!(placeBlackFive(luBox, blackX, blackY)))
					numFails++;

				break;
			}
		}
		else
			numFails++;
	}

	//Puts a 'D' for dark wherever there isn't a bulb or black box
	for (int x = 0; x < luBox.columns; x++)
	{
		for (int y = 0; y < luBox.rows; y++)
		{
			if ((luBox.box[x][y] < '0' || luBox.box[x][y] > '5') && luBox.box[x][y] != 'B')
				luBox.box[x][y] = 'D';
		}
	}

	illumationReset(luBox);

	//Places a bulb where it is dark and not touching a '0' - '3' so as to not invalidate those existing black boxes
	for (int x = 0; x < luBox.columns; x++)
	{
		for (int y = 0; y < luBox.rows; y++)
		{
			if (luBox.box[x][y] == 'D' && notTouching(luBox, x, y))
			{
				luBox.box[x][y] = 'B';
				illuminateFromBulb(luBox, x, y);
			}	
		}
	}
	
	//If any dark spots are left it will generate a new random puzzle
	for (int x = 0; x < luBox.columns; x++)
	{
		for (int y = 0; y < luBox.rows; y++)
		{
			if (luBox.box[x][y] == 'D')
			{
				luBox = generateRandomPuzzle(configSettings);
				x = luBox.columns;
				y = luBox.rows;
			}	
		}
	}

	//Erases everything that isn't a black box
	for (int x = 0; x < luBox.columns; x++)
	{
		for (int y = 0; y < luBox.rows; y++)
		{
			if (luBox.box[x][y] < '0' || luBox.box[x][y] > '5')
				luBox.box[x][y] = 'D';
		}
	}

	return luBox;
}