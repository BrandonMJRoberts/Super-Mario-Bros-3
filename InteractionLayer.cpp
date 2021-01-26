#include "InteractionLayer.h"

#include "Commons_SMB3.h"
#include "Constants_SMB3.h"
#include "GameManager_SMB3.h"
#include "Texture2D.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <SDL.h>

// --------------------------------------------------------------------------------------------------------------------------- //

InteractableLayer::InteractableLayer(std::string filePathToDataFile
, std::string                  filePathToSpriteSheet
, std::map<char, unsigned int> lookupConversion
, SDL_Renderer*                renderer
, Vector2D                     offsetFromTopLeft) 
: mAmountOfSpritesInDataFile(0)
, mLevelHeight(0)
, mLevelWidth(0)
, mAmountOfSpritesOnSpriteSheetHeight(0)
, mAmountOfSpritesOnSpriteSheetWidth(0)
, mOffsetFromTopLeft(offsetFromTopLeft)
{
	// Load in the data from the file
	if(!LoadInDataFromFile(filePathToDataFile, lookupConversion))
	{
		std::cout << "Failed to load the Interaction layer data: " << filePathToDataFile << std::endl;
		return;
	}

	// Create the sprite sheet 
	mSpriteSheet = new Texture2D(renderer);
	if (!mSpriteSheet->LoadFromFile(filePathToSpriteSheet))
	{
		std::cout << "Failed to create the sprite sheet for the interaction layer: " << filePathToSpriteSheet << std::endl;
		return;
	}
}

// --------------------------------------------------------------------------------------------------------------------------- //

InteractableLayer::~InteractableLayer()
{
	mListOfObjectUsedInInteractionLayer.clear();

	if (mInteractionLayerDataStore)
	{
		// Clear up the memory allocated for the index store
		for (unsigned int col = 0; col < mLevelWidth; col++)
		{
			delete[] mInteractionLayerDataStore[col];
		}

		delete[] mInteractionLayerDataStore;
	}

	mInteractionLayerDataStore = nullptr;

	delete mSpriteSheet;
	mSpriteSheet = nullptr;
}

// --------------------------------------------------------------------------------------------------------------------------- //

void InteractableLayer::Render(const Vector2D gridReferencePoint)
{
	// Only render if there is a sprite sheet to render from
	if (mSpriteSheet)
	{
		int xLerp = int((int(gridReferencePoint.x) - gridReferencePoint.x) * RESOLUTION_OF_SPRITES);
		int yLerp = int((int(gridReferencePoint.y) - gridReferencePoint.y) * RESOLUTION_OF_SPRITES);

		SDL_Rect portionOfSpriteSheet{ 0, 0, 
			                           RESOLUTION_OF_SPRITES, 
			                           RESOLUTION_OF_SPRITES };

		SDL_Rect destRect            { xLerp, 
			                           yLerp, 
			                           RESOLUTION_OF_SPRITES, 
			                           RESOLUTION_OF_SPRITES };

		// Loop through the internal store of sprite indexes and render the correct ones in the correct positions
		for (int row = int(gridReferencePoint.y + mOffsetFromTopLeft.y); row < (gridReferencePoint.y + BACKGROUND_SPRITE_RENDER_HEIGHT) + mOffsetFromTopLeft.y; row++)
		{
			if (row >= (int)mLevelHeight || row < 0)
			{
				destRect.y += RESOLUTION_OF_SPRITES;
				continue;
			}

			for (int col = int(gridReferencePoint.x - mOffsetFromTopLeft.x); col < (gridReferencePoint.x + BACKGROUND_SPRITE_RENDER_WIDTH) - mOffsetFromTopLeft.x; col++)
			{
				if (col >= (int)mLevelWidth || col < mOffsetFromTopLeft.x || col < 0)
				{
					destRect.x += RESOLUTION_OF_SPRITES;
					continue;
				}

				// Now we are correctly looping through the correct area of the grid to be rendered
				// Now we just need to render the correct sprite in the correct position
				if (mInteractionLayerDataStore[row][col] != 150)
				{
					portionOfSpriteSheet.x =     (mInteractionLayerDataStore[row][col] % mAmountOfSpritesOnSpriteSheetWidth)  * RESOLUTION_OF_SPRITES;
					portionOfSpriteSheet.y = (int(mInteractionLayerDataStore[row][col] / mAmountOfSpritesOnSpriteSheetWidth)) * RESOLUTION_OF_SPRITES;

					mSpriteSheet->Render(portionOfSpriteSheet, destRect);
				}

				// Move the x-axis along 
				destRect.x += RESOLUTION_OF_SPRITES;
			}

			// Move the y-axis along and reset the x-position
			destRect.y += RESOLUTION_OF_SPRITES;
			destRect.x = xLerp;
		}
	}
}

// --------------------------------------------------------------------------------------------------------------------------- //

void InteractableLayer::Update(const float deltaTime)
{

}

// --------------------------------------------------------------------------------------------------------------------------- //

bool InteractableLayer::LoadInDataFromFile(std::string filePath, std::map<char, unsigned int> conversionTable)
{
	// First we need to open the file and then read in the data in in the correct format
	std::ifstream dataFile(filePath);
	if (!dataFile)
	{
		std::cout << "Failed to open interaction data file: " << filePath << std::endl;
		return false;
	}

	std::string       line;
	char              lineChar[MAX_LEVEL_WIDTH];
	std::stringstream ssLine;

	unsigned int currentRow                    = 0;
	unsigned int currentAmountOfSpritesDefined = 0;
	unsigned int failsafeCounter               = 0;

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

		// First we need to allocate the level width, height and the amount of sprite types (so we can loop the correct amount of times)
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

		if (mAmountOfSpritesInDataFile == 0)
		{
			ssLine = std::stringstream(line);
			ssLine >> mAmountOfSpritesInDataFile;
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

		// Now we need to load in the interactable sprites in order
		if (currentAmountOfSpritesDefined < mAmountOfSpritesInDataFile)
		{
			mListOfObjectUsedInInteractionLayer.push_back(line);
			currentAmountOfSpritesDefined++;
			continue;
		}

		// From here on we are just allocating the background sprite indexes - so if the store doesnt exist then create it
		if (!mInteractionLayerDataStore)
		{
			// First allocate the memory indexes for the height/rows
			mInteractionLayerDataStore = new unsigned int* [mLevelHeight];

			// Now loop through the rows and allocte the memory for the width/columns
			for (unsigned int row = 0; row < mLevelHeight; row++)
			{
				mInteractionLayerDataStore[row] = new unsigned int[mLevelWidth - 1];
			}

			// The memory is alloctaed for access like this: store[row][column]
		}

		// Now we just need to load in and calculate the elements of the array
		for (unsigned int charID = 0; charID < line.size(); charID++)
		{
			mInteractionLayerDataStore[currentRow][charID] = conversionTable[line[charID]];
		}

		currentRow++;
	}

	// Now check if all the data has been created correctly
	if (!CheckAllDataLoaded())
	{
		return false;
	}

	return true;
}

// --------------------------------------------------------------------------------------------------------------------------- //

bool InteractableLayer::CheckAllDataLoaded()
{
	if (mLevelWidth == 0)
		return false;

	if (mLevelHeight == 0)
		return false;

	if (mAmountOfSpritesInDataFile == 0)
		return false;

	if (!mInteractionLayerDataStore)
		return false;

	if (mAmountOfSpritesOnSpriteSheetHeight == 0)
		return false;

	if (mAmountOfSpritesOnSpriteSheetWidth == 0)
		return false;

	return true;
}

// --------------------------------------------------------------------------------------------------------------------------- //