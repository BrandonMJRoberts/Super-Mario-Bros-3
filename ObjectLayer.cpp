#include "ObjectLayer.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "Constants_SMB3.h"

// -------------------------------------------------------------------------------------------------------------------------- //

ObjectLayer::ObjectLayer(std::string filePathToDataFile, SDL_Renderer* renderer)
{
	mRenderer = renderer;

	if (!LoadInDataFromFile(filePathToDataFile))
	{
		std::cout << "Failed to load in the objects for this level area: " << filePathToDataFile << std::endl;
	}
}

// -------------------------------------------------------------------------------------------------------------------------- //

ObjectLayer::~ObjectLayer()
{

}

// -------------------------------------------------------------------------------------------------------------------------- //

void ObjectLayer::Render()
{

}

// -------------------------------------------------------------------------------------------------------------------------- //

void ObjectLayer::Update(const float deltaTime, SDL_Event e)
{
	// Loop through all objects in the level and update them
	for (unsigned int i = 0; i < mSpawnedObjectsInLevel.size(); i++)
	{
		// Check if the object needs to be moved to the other list
		if (mSpawnedObjectsInLevel[i].Update(deltaTime))
		{
			// Then add this object to the other list - its position will be reset in its update so we dont need to do it here
			mUnspawnedObjectsInLevel.push_back(mSpawnedObjectsInLevel[i]);
		}
	}
}

// -------------------------------------------------------------------------------------------------------------------------- //

void ObjectLayer::CheckIfShouldSpawnObject()
{
	// Loop through all unspawned objects and check if they need to be spawned
	for (unsigned int i = 0; i < mUnspawnedObjectsInLevel.size(); i++)
	{
		// if(mUnspawnedObjectsInLevel.GetSpawnPosition() // is within the play area bounding)
			// Move to the other list	
	}
}

// -------------------------------------------------------------------------------------------------------------------------- //

void ObjectLayer::CheckForDestroyingObjects()
{
	// Loop through all spawned objects and check if they need to be unspawned ('destroyed')
	for (unsigned int i = 0; i < mSpawnedObjectsInLevel.size(); i++)
	{

	}
}

// -------------------------------------------------------------------------------------------------------------------------- //

bool ObjectLayer::LoadInDataFromFile(std::string filePath)
{
	// First we need to open the file and then read in the data in in the correct format
	std::ifstream dataFile(filePath);
	if (!dataFile)
	{
		std::cout << "Failed to open interaction data file: " << filePath << std::endl;
		return false;
	}

	std::string       line, objectDefinitionType;
	char              lineChar[MAX_LEVEL_WIDTH];
	std::stringstream ssLine;

	unsigned int failsafeCounter = 0;

	// Loop through the entire file untill it is empty
	while (line != "END")
	{
		failsafeCounter++;
		if (failsafeCounter > FAILSAFE_MAX_COUNT_FILE_LOADING_LOOPS)
		{
			std::cout << "The file " << filePath << " does not contain the END." << std::endl;
			return false;
		}

		dataFile.getline(lineChar, MAX_LEVEL_WIDTH);
		line = std::string(lineChar);

		// If the line is a comment then just skip over it, or if the entire line is empty
		if (line[0] == '#' || line == "")
			continue;

		// If we hit the end declaration then stop reading in from the file
		if (line == "END")
			break;

		// First check if we are on a new object type definition
		// This check works through checking if the first word is a string or an int, as all object definitions start with a position
		// So first check if the line starts with an int
		if (int(line[0] > 47 && int(line[0]) < 58))
		{
			// Then it is a definition of another object of the currently selected type

			// Need to think of a way to efficiently do this !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			if (line == "GOOMBA")
			{
				// Temperary placeholder as all objects have not been created yet
				mUnspawnedObjectsInLevel.push_back(BaseObject_SMB3(mRenderer));
			}
			else
			{
				std::cout << "Tried to create an object that is of a type that doesnt exist!" << std::endl;
				return false;
			}
		}
		else
		{
			// We are re-stating the object definition
			objectDefinitionType = line;
		}
	
	}

	return true;
}

// -------------------------------------------------------------------------------------------------------------------------- //