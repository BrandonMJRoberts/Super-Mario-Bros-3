#include "LevelMap.h"

#include <fstream>
#include <iostream>

#include <SDL.h>

#include <string>

#define SPRITE_RESOLUTION 32

// -------------------------------------------------------------------------------- //

LevelMap::LevelMap(const char* filePathToCollisionMap, const char* filePathToSpriteSheet, SDL_Renderer* renderer) 
	: mLevelHeight(0), 
	mLevelWidth(0), 
	mCollisionMap(nullptr), 
	mRenderer(renderer)
{
	// Load the map into the level map from the file path
	LoadCollisionMapFromFile(filePathToCollisionMap);

	// Create the background collision sprite sheet
	mBackgroundSpriteSheet = new Texture2D(mRenderer);
	if (!mBackgroundSpriteSheet->LoadFromFile(filePathToSpriteSheet))
	{
		std::cout << "Failed to load in the background sprite sheet." << std::endl;
		return;
	}
}

// -------------------------------------------------------------------------------- //

LevelMap::~LevelMap()
{
	// Clean up the memory allocated for the memory 
	for (unsigned int i = 0; i < mLevelHeight; i++)
	{
		delete[] mCollisionMap[i];
	}
	delete[] mCollisionMap;

	mCollisionMap = nullptr;
}

// -------------------------------------------------------------------------------- //

int LevelMap::GetCollisionTileAt(unsigned int h, unsigned int w)
{
	// Bounds checks
	if (h < mLevelHeight && w < mLevelWidth && h >= 0 && w >= 0)
	{
		// Pass back the value
		return mCollisionMap[h][w];
	}

	return 0;
}

// -------------------------------------------------------------------------------- //

void LevelMap::LoadCollisionMapFromFile(const char* filePath)
{
	std::ifstream inFile;

	inFile.open(filePath);

	if (inFile.is_open())
	{
		// Delete the old map
		if (mCollisionMap)
		{
			for (unsigned int i = 0; i < mLevelHeight; i++)
			{
				delete[] mCollisionMap[i];
			}
			delete[] mCollisionMap;
			mCollisionMap = nullptr;
		}

		// Get the width and height
		inFile >> mLevelWidth >> mLevelHeight;

		// Now allocate memory for the map
		mCollisionMap = new int* [mLevelHeight];
		for (unsigned int i = 0; i < mLevelHeight; i++)
		{
			mCollisionMap[i] = new int[mLevelWidth];
		}

		// Now we have the memory allocated we need to populate the array with values from the text file
		std::string line = "";

		// Load in the data
		for (unsigned int h = 0; h < mLevelHeight; h++)
		{
			// Get the next line in the file
			inFile >> line;

			for (unsigned int w = 0; w < mLevelWidth; w++)
			{
				if (line.size() > w)
				{
					if (line[w] == '0')
						mCollisionMap[h][w] = 0;
					else
						mCollisionMap[h][w] = 1;
				}
			}
		}

		inFile.close();
		return;
	}
	else
	{
		inFile.close();
	}
}

// -------------------------------------------------------------------------------- //

void LevelMap::Render()
{
	SDL_Rect sourceRect = SDL_Rect{ 0, 0, SPRITE_RESOLUTION, SPRITE_RESOLUTION };
	SDL_Rect destRect   = SDL_Rect{ 0, 0, SPRITE_RESOLUTION, SPRITE_RESOLUTION };

	// Loop through the collision map and render the correct sprite in each position
	for (unsigned int h = 0; h < mLevelHeight; h++)
	{
		for (unsigned int w = 0; w < mLevelWidth; w++)
		{
			sourceRect.x = mCollisionMap[h][w] * SPRITE_RESOLUTION;

			destRect.x = w * SPRITE_RESOLUTION;
			destRect.y = h * SPRITE_RESOLUTION;

			if (mBackgroundSpriteSheet)
				mBackgroundSpriteSheet->Render(sourceRect, destRect, SDL_FLIP_NONE, 0.0f);
		}
	}
}

// -------------------------------------------------------------------------------- //