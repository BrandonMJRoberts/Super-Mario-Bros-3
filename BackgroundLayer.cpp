#include "BackgroundLayer.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <SDL.h>

#include "Texture2D.h"

#include "Constants_SMB3.h"
#include "GameManager_SMB3.h"

// --------------------------------------------------------------------------------------------------------------------------- //

BackgroundLayer::BackgroundLayer(std::string filePathToDataFile 
	, std::string                  filePathToSpriteSheet 
	, std::map<char, unsigned int> lookupConversion 
	, SDL_Renderer*                renderer
	, Vector2D                     offsetFromTopLeft) 
	: mLevelHeight(0)
	, mLevelWidth(0)
	, mLevelEndType(-1)
	, mAmountOfSpritesOnSpriteSheetWidth(0)
	, mAmountOfSpritesOnSpriteSheetHeight(0)
	, mFilePathToSpriteSheet(filePathToSpriteSheet.c_str())
	, mBackgroundSpriteIndexStore(nullptr)
{
	// Load in the sprite sheet
	mSpriteSheet = new Texture2D(renderer);
	if (!mSpriteSheet->LoadFromFile(mFilePathToSpriteSheet))
	{
		std::cout << "Failed to load in sprite sheet for background sprites: " << mFilePathToSpriteSheet << std::endl;
		return;
	}

	// Load in the data from the file
	if (!LoadInDataFromFile(filePathToDataFile, lookupConversion))
	{
		std::cout << "Failed to load in the background data from the file, check the formatting and that the file exists." << std::endl;
	}

	// This offset is grid distance across the infinite virtual screen - eg. The ending of a level will have a large offset (about 150) whereas the main level has an offset of 0
	mOffsetFromTopLeft                = offsetFromTopLeft;
}

// --------------------------------------------------------------------------------------------------------------------------- //

BackgroundLayer::~BackgroundLayer()
{
	if (mSpriteSheet)
	{
		delete mSpriteSheet;
		mSpriteSheet = nullptr;
	}

	mFilePathToSpriteSheet = nullptr;

	if (mBackgroundSpriteIndexStore)
	{
		// Clear up the memory allocated for the index store
		for (unsigned int col = 0; col < mLevelHeight; col++)
		{
			delete[] mBackgroundSpriteIndexStore[col];
		}

		delete[] mBackgroundSpriteIndexStore;
	}

	mBackgroundSpriteIndexStore = nullptr;

}

// --------------------------------------------------------------------------------------------------------------------------- //

bool BackgroundLayer::CheckAllDataLoaded()
{
	// Check if all of the needed variables are initialised
	if (!mBackgroundSpriteIndexStore)
		return false;

	if (mLevelWidth == 0)
		return false;

	if (mLevelHeight == 0)
		return false;

	if (mLevelEndType == -1)
		return false;

	return true;
}

// --------------------------------------------------------------------------------------------------------------------------- //

bool BackgroundLayer::LoadInDataFromFile(std::string filePath, std::map<char, unsigned int> lookupConversion)
{
	// First we need to open the file and then read in the data in in the correct format
	std::ifstream dataFile(filePath);
	if (!dataFile.is_open())
	{
		std::cout << "Failed to open background data file: " << filePath << std::endl;
		return false;
	}

	std::string       line; 
	char              lineChar[MAX_LEVEL_WIDTH];
	std::stringstream ssLine;

	unsigned int currentRow      = 0;
	unsigned int failsafeCounter = 0;

	// Loop through the entire file untill it is empty
	while (line != "END")
	{
		failsafeCounter++;
		if (failsafeCounter >= FAILSAFE_MAX_COUNT_FILE_LOADING_LOOPS)
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

		// First we need to allocate the level width, height and ending type
		if (mLevelWidth == 0)
		{
			ssLine = std::stringstream(line);
			ssLine >> mLevelWidth;
			continue;
		}

		if (mLevelHeight == 0)
		{
			ssLine = std::stringstream(line);
			ssLine >> mLevelHeight;
			continue;
		}

		if (mLevelEndType == -1)
		{
			ssLine = std::stringstream(line);
			ssLine >> mLevelEndType;
			continue;
		}

		if (mAmountOfSpritesOnSpriteSheetWidth == 0)
		{
			ssLine = std::stringstream(line);
			ssLine >> mAmountOfSpritesOnSpriteSheetWidth;
			continue;
		}

		if (mAmountOfSpritesOnSpriteSheetHeight == 0)
		{
			ssLine = std::stringstream(line);
			ssLine >> mAmountOfSpritesOnSpriteSheetHeight;
			continue;
		}

		// From here on we are just allocating the background sprite indexes - so if the store doesnt exist then create it
		if (!mBackgroundSpriteIndexStore)
		{
			// First allocate the memory indexes for the height/rows
			mBackgroundSpriteIndexStore = new unsigned int*[mLevelHeight];

			// Now loop through the rows and allocte the memory for the width/columns
			for (unsigned int row = 0; row < mLevelHeight; row++)
			{
				mBackgroundSpriteIndexStore[row] = new unsigned int[mLevelWidth];
			}

			// The memory is allocated for access like this: store[row][column]
		}

		// Now we just need to load in and calculate the elements of the array
		for (unsigned int charID = 0; charID < mLevelWidth; charID++)
		{
			// The charID represents which column it is on
			// Across then down
			if(charID < line.size())
				mBackgroundSpriteIndexStore[currentRow][charID] = lookupConversion[line[charID]];
		}

		if(currentRow + 1 < mLevelHeight)
			currentRow++;
		else
		{
			dataFile.close();

			// Now check if all the data has been created correctly
			if (!CheckAllDataLoaded())
			{
				return false;
			}

			return true;
		}
	}

	// Now check if all the data has been created correctly
	if (!CheckAllDataLoaded())
	{
		return false;
	}

	std::cout << std::endl;

	return true;
}

// --------------------------------------------------------------------------------------------------------------------------- //

void BackgroundLayer::Render(Vector2D gridReferencePoint)
{
	// If there is a sprite sheet then we can render
	if (mSpriteSheet)
	{
		int      xLerp                = int(((int)gridReferencePoint.x - gridReferencePoint.x) * RESOLUTION_OF_SPRITES);
		int      yLerp                = int(((int)gridReferencePoint.y - gridReferencePoint.y) * RESOLUTION_OF_SPRITES);

		SDL_Rect portionOfSpriteSheet {0, 0, 
			                           RESOLUTION_OF_SPRITES, RESOLUTION_OF_SPRITES};

		SDL_Rect destRect             {xLerp, yLerp, 
			                           RESOLUTION_OF_SPRITES, RESOLUTION_OF_SPRITES };

		// Loop through the internal store of sprite indexes and render the correct ones in the correct positions
		// Must be a signed int to allow for negative values, and negative scrolling
		for (int row = int(gridReferencePoint.y + mOffsetFromTopLeft.y); row < (gridReferencePoint.y + BACKGROUND_SPRITE_RENDER_HEIGHT) + mOffsetFromTopLeft.y; row++)
		{
			// Checks to remove scope errors and to make sure we are only rendering what is on the screen
			if (row >= int(mLevelHeight) || row < 0)
			{
				destRect.y += RESOLUTION_OF_SPRITES;
				continue;
			}

			for (int col = int(gridReferencePoint.x - mOffsetFromTopLeft.x); col < (gridReferencePoint.x + BACKGROUND_SPRITE_RENDER_WIDTH) - mOffsetFromTopLeft.x; col++)
			{
				// Error checking
				if (col >= (int)mLevelWidth || col < 0)
				{
					// Make sure to add the offset on even if we are not rendering this column
					destRect.x += RESOLUTION_OF_SPRITES;
					continue;
				}

				// Now we are correctly looping through the correct area of the grid to be rendered
				// Now we just need to render the correct sprite in the correct position
				portionOfSpriteSheet.x =     (mBackgroundSpriteIndexStore[row][col] % mAmountOfSpritesOnSpriteSheetWidth)  * RESOLUTION_OF_SPRITES;
				portionOfSpriteSheet.y = (int(mBackgroundSpriteIndexStore[row][col] / mAmountOfSpritesOnSpriteSheetWidth)) * RESOLUTION_OF_SPRITES;

				// Render the sprite in the correct position
				mSpriteSheet->Render(portionOfSpriteSheet, destRect);

				// Move along the x-axis
				destRect.x += RESOLUTION_OF_SPRITES;
			}

			// Move down the y-axis and reset the x-axis
			destRect.y += RESOLUTION_OF_SPRITES;
			destRect.x = xLerp;
		}
	}
}

// --------------------------------------------------------------------------------------------------------------------------- //
