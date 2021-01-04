#include "ObjectLayer.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <string.h>

#include "AllObjectsInclude.h"

#include "Constants_SMB3.h"

// -------------------------------------------------------------------------------------------------------------------------- //

ObjectLayer::ObjectLayer(std::string filePathToDataFile, 
	                     SDL_Renderer* renderer)
: mRenderer(renderer)
{
	InstantiateNameConversions();

	// Load in the data from the file
	if (!LoadInDataFromFile(filePathToDataFile))
	{
		std::cout << "Failed to load in the objects for this level area: " << filePathToDataFile << std::endl;
		return;
	}
}

// -------------------------------------------------------------------------------------------------------------------------- //

ObjectLayer::~ObjectLayer()
{
	DestroyAllNameConversions();
	mNameToObjectConversion.clear();

	for (unsigned int i = 0; i < mUnspawnedObjectsInLevel.size(); i++)
	{
		delete mUnspawnedObjectsInLevel[i];
		mUnspawnedObjectsInLevel[i] = nullptr;
	}
	mUnspawnedObjectsInLevel.clear();

	for (unsigned int i = 0; i < mSpawnedObjectsInLevel.size(); i++)
	{
		delete mSpawnedObjectsInLevel[i];
		mSpawnedObjectsInLevel[i] = nullptr;
	}
	mSpawnedObjectsInLevel.clear();
}

// -------------------------------------------------------------------------------------------------------------------------- //

void ObjectLayer::Render()
{

}

// -------------------------------------------------------------------------------------------------------------------------- //

void ObjectLayer::Update(const float deltaTime, SDL_Event e, Vector2D playerPosition)
{
	// Loop through all objects in the level and update them
	for (unsigned int i = 0; i < mSpawnedObjectsInLevel.size(); i++)
	{
		// Check if the object needs to be moved to the other list
		if (mSpawnedObjectsInLevel[i]->Update(deltaTime, playerPosition))
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

	std::string       line, objectNameLine;

	unsigned int      failsafeCounter = 0;

	// Loop through the entire file untill it is empty
	while (std::getline(dataFile, line))
	{
		if (failsafeCounter++ > FAILSAFE_MAX_COUNT_FILE_LOADING_LOOPS)
		{
			dataFile.close();
			break;
		}

		// First check to see if we have hit the end of the file
		if (line == "END")
		{
			dataFile.close();
			return true;
		}
		
		// Check if the line is empty or is a comment
		if (line == "" || line[0] == '#')
			continue;

		// If we have hit 
		if (line == "NEXT")
		{
			objectNameLine = "";
			continue;
		}

		// Now check if we are waiting for a new name of definition
		if (objectNameLine == "")
		{
			objectNameLine = line;
			continue;
		}
		else
		{
			// Create a new instance of the object
			mUnspawnedObjectsInLevel.push_back(mNameToObjectConversion[objectNameLine]->Clone(line));
			continue;
		}
	}

	return false;
}

// -------------------------------------------------------------------------------------------------------------------------- //

// NEED TO ACTUALLY HOOK THESE UP TO THE SPAWN POINTS WHEN THIS FUNCTIONALITY HAS BEEN ADDED

Vector2D ObjectLayer::GetInitialSpawnPoint() const
{
	return Vector2D();
}

// -------------------------------------------------------------------------------------------------------------------------- //

Vector2D ObjectLayer::GetSpawnPoint(unsigned int spawnPointIndex) const
{
	return Vector2D();
}

// -------------------------------------------------------------------------------------------------------------------------- //

void ObjectLayer::InstantiateNameConversions()
{
	// Setup the conversions from a string to a base default data type

	// Collectables
	mNameToObjectConversion["COIN"]               = new Coin_SMB3(Vector2D(), false, mRenderer, "", 0, 0, 0, 0, false, 0.3f);

	// Block objects
	mNameToObjectConversion["BRICK_BLOCK"]         = new BrickBlock(Vector2D(), false, mRenderer, "", 0, 0, RESOLUTION_OF_SPRITES, RESOLUTION_OF_SPRITES, 0.5f, 1, POWER_UP_TYPE::MUSHROOM, false, nullptr, nullptr, true);
	mNameToObjectConversion["INVISIBLE_BLOCK"]     = new InvisibleBlock(Vector2D(), false, mRenderer, "", 0, 0, RESOLUTION_OF_SPRITES, RESOLUTION_OF_SPRITES, 0.3f, 1, POWER_UP_TYPE::NONE, false, (CollectableObject*)mNameToObjectConversion["COIN"], nullptr);
	mNameToObjectConversion["QUESTION_MARK_BLOCK"] = new QuestionMarkBlock(Vector2D(), false, mRenderer, "", 0, 0, RESOLUTION_OF_SPRITES, RESOLUTION_OF_SPRITES, 0.3f, 1, POWER_UP_TYPE::NONE, false, (CollectableObject*)mNameToObjectConversion["COIN"], nullptr);

	mNameToObjectConversion["PIPE"]                = new Pipe(Vector2D(), false, mRenderer, "", 0, 0, 0, 0, 0.0f);

	// Enemy Objects
	mNameToObjectConversion["GOOMBA"]             = new Goomba(Vector2D(), false, mRenderer, "", 0, 0, RESOLUTION_OF_SPRITES, RESOLUTION_OF_SPRITES, 0.3f, true, false, true);
	mNameToObjectConversion["PARA_GOOMBA"]        = new ParaGoomba(Vector2D(), false, mRenderer, "", 0, 0, RESOLUTION_OF_SPRITES, RESOLUTION_OF_SPRITES, 0.3f, true, true, true);

	mNameToObjectConversion["KOOPA_TROOPER"]      = new KoopaTrooper(Vector2D(), false, mRenderer, "", 0, 0, RESOLUTION_OF_SPRITES, RESOLUTION_OF_SPRITES, 0.3f, true, false, true);
	mNameToObjectConversion["PARA_KOOPA_TROOPER"] = new KoopaTrooper(Vector2D(), false, mRenderer, "", 0, 0, RESOLUTION_OF_SPRITES, RESOLUTION_OF_SPRITES, 0.3f, true, true, true);

	mNameToObjectConversion["SPAWN_POINT"]        = new SpawnPoint(Vector2D(), false, -1);
}

// -------------------------------------------------------------------------------------------------------------------------- //

void ObjectLayer::DestroyAllNameConversions()
{
	// Collectables
	delete mNameToObjectConversion["COIN"];
	mNameToObjectConversion["COIN"] = nullptr;

	// Block objects
	delete mNameToObjectConversion["BRICK BLOCK"];
	mNameToObjectConversion["BRICK BLOCK"] = nullptr;

	delete mNameToObjectConversion["INVISIBLE BLOCK"];
	mNameToObjectConversion["INVISIBLE BLOCK"] = nullptr;

	delete mNameToObjectConversion["QUESTION_MARK_BLOCK"];
	mNameToObjectConversion["QUESTION_MARK_BLOCK"] = nullptr;

	delete mNameToObjectConversion["PIPE"];
	mNameToObjectConversion["PIPE"] = nullptr;

	// Enemies
	delete mNameToObjectConversion["GOOMBA"];
	mNameToObjectConversion["GOOMBA"] = nullptr;

	delete mNameToObjectConversion["KOOPA TROOPER"];
	mNameToObjectConversion["KOOPA TROOPER"] = nullptr;
}

// -------------------------------------------------------------------------------------------------------------------------- //