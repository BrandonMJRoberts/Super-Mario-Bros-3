#include "ObjectLayer.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <string.h>

#include "AllObjectsInclude.h"

#include "Constants_SMB3.h"
#include "Animation_Speeds.h"

// -------------------------------------------------------------------------------------------------------------------------- //

ObjectLayer::ObjectLayer(std::string   filePathToDataFile, 
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

	// Clear up all memory used
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

	// Clear the spawnpoint store
	mSpawnPoints.clear();
}

// -------------------------------------------------------------------------------------------------------------------------- //

void ObjectLayer::Render(const Vector2D gridReferencePoint)
{
	// Loop through all of the spawned objects are render them
	for (unsigned int i = 0; i < mSpawnedObjectsInLevel.size(); i++)
	{
		if(mSpawnedObjectsInLevel[i])
			mSpawnedObjectsInLevel[i]->Render(gridReferencePoint);
	}
}

// -------------------------------------------------------------------------------------------------------------------------- //

void ObjectLayer::Update(const float deltaTime, SDL_Event e, Vector2D gridReferencePoint)
{
	// Update the currently spawned objects
	UpdateSpawnedObjects(deltaTime, gridReferencePoint);

	// See if any of the object not currently spawned should spawn again
	CheckIfObjectsShouldSpawn(gridReferencePoint);
}

// -------------------------------------------------------------------------------------------------------------------------- //

void ObjectLayer::CheckIfObjectsShouldSpawn(const Vector2D gridReferencePoint)
{
	std::vector<unsigned int> removeIDs;

	// Loop through all unspawned objects and check if they need to be spawned
	for (unsigned int i = 0; i < mUnspawnedObjectsInLevel.size(); i++)
	{
		if (mUnspawnedObjectsInLevel[i])
		{
			// If an object is instance locked and is outside of the play area then un-instance lock it
			if (mUnspawnedObjectsInLevel[i]->GetIsInstanceLocked() && !InPlayArea(mUnspawnedObjectsInLevel[i]->GetSpawnPosition(), gridReferencePoint))
			{
				// Unlock it
				mUnspawnedObjectsInLevel[i]->SetInstanceLocked(false);

				// Continue as the check below required the object to be in the play area - to be in here it cannot be
				continue;
			}

			// If the object exists, and is in the play area, and is not instance locked
			if (InPlayArea(mUnspawnedObjectsInLevel[i]->GetSpawnPosition(), gridReferencePoint) && !mUnspawnedObjectsInLevel[i]->GetIsInstanceLocked())
			{
				// Then set this object should spawn into the world
				mSpawnedObjectsInLevel.push_back(mUnspawnedObjectsInLevel[i]);

				mUnspawnedObjectsInLevel[i]->SetPositionToSpawn();

				removeIDs.push_back(i);

				continue;
			}
		}
	}

	unsigned int offset = 0;

	// Now remove these from the other list
	while (removeIDs.size() > 0)
	{
		mUnspawnedObjectsInLevel.erase(mUnspawnedObjectsInLevel.begin() + (removeIDs[0] - offset));
		offset++;

		removeIDs.erase(removeIDs.begin());
	}
}

// -------------------------------------------------------------------------------------------------------------------------- //

bool ObjectLayer::InPlayArea(const Vector2D testPosition, const Vector2D gridReferencePoint)
{
	const double XDistance = abs(gridReferencePoint.x - testPosition.x);
	const double YDistance = abs(gridReferencePoint.y - testPosition.y);

	// First check if the X is valid
	if (XDistance < SCREEN_WIDTH_GRID_SMB3)
	{
		// Now check if the Y is valid
		if (YDistance < SCREEN_HEIGHT_GRID_SMB3)
		{
			return true;
		}
	}

	// Not in the bounds so return false.
	return false;
}

// -------------------------------------------------------------------------------------------------------------------------- //

void ObjectLayer::UpdateSpawnedObjects(const float deltaTime, Vector2D gridReferencePoint)
{
	// Loop through all spawned objects and check if they need to be unspawned ('destroyed')
	for (unsigned int i = 0; i < mSpawnedObjectsInLevel.size(); i++)
	{
		if (mSpawnedObjectsInLevel[i])
		{
			// If they return true then they need to be unspawned / destroyed
			if (mSpawnedObjectsInLevel[i]->Update(deltaTime, gridReferencePoint) || !InPlayArea(mSpawnedObjectsInLevel[i]->GetCurrentPosition(), gridReferencePoint))
			{
				// First store internally that this object has been removed from active play
				mSpawnedObjectsInLevel[i]->SetInstanceLocked(true);

				// Add it to the other list
				mUnspawnedObjectsInLevel.push_back(mSpawnedObjectsInLevel[i]);

				// Remove this object from the spawned list
				mSpawnedObjectsInLevel.erase(mSpawnedObjectsInLevel.begin() + i);
			}
		}
	}

	// Now reset the bool making sure that the animation update only gets called once per frame
	for (unsigned int i = 0; i < mSpawnedObjectsInLevel.size(); i++)
	{
		if (mSpawnedObjectsInLevel[i])
		{
			mSpawnedObjectsInLevel[i]->ResetUpdatedStaticVariables();
		}
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
			// First check if it is a spawnpoint as they need to be handelled seperately from the other objects
			if (objectNameLine == "SPAWN_POINT")
			{
				mSpawnPoints.push_back(SpawnPoint(line));
				continue;
			}

			// Create a new instance of the object
			mUnspawnedObjectsInLevel.push_back(mNameToObjectConversion[objectNameLine]->Clone(line));

			// Check if the object should start spawned in the level
			if (mUnspawnedObjectsInLevel[mUnspawnedObjectsInLevel.size() - 1] && mUnspawnedObjectsInLevel[mUnspawnedObjectsInLevel.size() - 1]->GetIsSpawnedInLevel())
			{
				mSpawnedObjectsInLevel.push_back(mUnspawnedObjectsInLevel[mUnspawnedObjectsInLevel.size() - 1]);
				mUnspawnedObjectsInLevel.pop_back();
			}
			continue;
		}
	}

	return false;
}

// -------------------------------------------------------------------------------------------------------------------------- //

Vector2D ObjectLayer::GetInitialSpawnPoint() const
{
	// Loop through all spawned objects - as spawnpoints are forced to always be spawned - and get the spawnpoint with ID 0
	for (unsigned int i = 0; i < mSpawnPoints.size(); i++)
	{
		if (mSpawnPoints[i].IsIndex(0))
			return mSpawnPoints[i].GetPosition();
	}

	// If we cant find the spawnpoint then just return nothing
	return Vector2D();
}

// -------------------------------------------------------------------------------------------------------------------------- //

Vector2D ObjectLayer::GetSpawnPoint(unsigned int spawnPointIndex) const
{
	// Loop through all spawned objects - as spawnpoints are forced to always be spawned - and get the spawnpoint with ID 0
	for (unsigned int i = 0; i < mSpawnPoints.size(); i++)
	{
		if (mSpawnPoints[i].IsIndex(spawnPointIndex))
			return mSpawnPoints[i].GetPosition();
	}

	// If we cant find the spawnpoint then just return nothing
	return Vector2D();
}

// -------------------------------------------------------------------------------------------------------------------------- //

void ObjectLayer::InstantiateNameConversions()
{
	// Setup the conversions from a string to a base default data type

	// Collectables
	mNameToObjectConversion["COIN"]                = new Coin_SMB3(Vector2D(), false, mRenderer, "SDL_Mario_Project/Objects/Coin.png", 6, 1, RESOLUTION_OF_SPRITES, RESOLUTION_OF_SPRITES, false, COIN_ANIMATION_SPEED);

	// Block objects
	mNameToObjectConversion["BRICK_BLOCK"]         = new BrickBlock(Vector2D(), false, mRenderer, "SDL_Mario_Project/Objects/BrickBlock.png", 4, 1, RESOLUTION_OF_SPRITES, RESOLUTION_OF_SPRITES, BRICK_BLOCK_ANIMATION_SPEED, 1, POWER_UP_TYPE::MUSHROOM, false, nullptr, nullptr, true);
	mNameToObjectConversion["INVISIBLE_BLOCK"]     = new InvisibleBlock(Vector2D(), false, mRenderer, "SDL_Mario_Project/Objects/InvisibleBlock.png", 2, 1, RESOLUTION_OF_SPRITES, RESOLUTION_OF_SPRITES, 0.0f, 1, POWER_UP_TYPE::NONE, false, (CollectableObject*)mNameToObjectConversion["COIN"], nullptr);
	mNameToObjectConversion["QUESTION_MARK_BLOCK"] = new QuestionMarkBlock(Vector2D(), false, mRenderer, "SDL_Mario_Project/Objects/QuestionMarkBlock.png", 5, 1, RESOLUTION_OF_SPRITES, RESOLUTION_OF_SPRITES, QUESTION_MARK_BLOCK_ANIMATION_SPEED, 1, POWER_UP_TYPE::NONE, false, (CollectableObject*)mNameToObjectConversion["COIN"], nullptr);

	mNameToObjectConversion["PIPE"]                = new Pipe(Vector2D(), false, mRenderer, "", 1, 1, 0, 0, 0.0f);

	// Enemy Objects
	mNameToObjectConversion["GOOMBA"]             = new Goomba(Vector2D(), false, mRenderer, "SDL_Mario_Project/Enemies/Goomba/Goomba.png", 6, 3, RESOLUTION_OF_SPRITES, RESOLUTION_OF_SPRITES, 0.15f, true, false, true);
	mNameToObjectConversion["PARA_GOOMBA"]        = new ParaGoomba(Vector2D(), false, mRenderer, "SDL_Mario_Project/Enemies/Goomba/Goomba.png", 1, 1, RESOLUTION_OF_SPRITES, RESOLUTION_OF_SPRITES, 0.3f, true, true, true);

	mNameToObjectConversion["KOOPA_TROOPER"]      = new KoopaTrooper(Vector2D(), false, mRenderer, "SDL_Mario_Project/Enemies/Koopa Trooper/Koopa.png", 14, 3, RESOLUTION_OF_SPRITES, RESOLUTION_OF_SPRITES, KOOPA_ANIMATION_SPEED, true, false, true, 0);
	mNameToObjectConversion["PARA_KOOPA_TROOPER"] = new KoopaTrooper(Vector2D(), false, mRenderer, "SDL_Mario_Project/Enemies/Koopa Trooper/Koopa.png", 1, 1, RESOLUTION_OF_SPRITES, RESOLUTION_OF_SPRITES, KOOPA_ANIMATION_SPEED, true, true, true, 0);
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

	delete mNameToObjectConversion["PARA_GOOMBA"];
	mNameToObjectConversion["PARA_GOOMBA"] = nullptr;

	delete mNameToObjectConversion["KOOPA TROOPER"];
	mNameToObjectConversion["KOOPA TROOPER"] = nullptr;

	delete mNameToObjectConversion["PARA_KOOPA_TROOPER"];
	mNameToObjectConversion["PARA_KOOPA_TROOPER"] = nullptr;
}

// -------------------------------------------------------------------------------------------------------------------------- //