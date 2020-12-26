#include "BackgroundLayer.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <SDL.h>

#include "Texture2D.h"

#include "Constants_SMB3.h"
#include "GameManager_SMB3.h"

// --------------------------------------------------------------------------------------------------------------------------- //

BackgroundLayer::BackgroundLayer(std::string filePathToDataFile, std::string filePathToSpriteSheet, std::map<char, unsigned int> lookupConversion, SDL_Renderer* renderer, Vector2D offsetFromTopLeft)
{
	mLevelHeight					   =  0;
	mLevelWidth						   =  0;
	mLevelEndType					   = -1;
	mAountOfSpritesOnSpriteSheetWidth  =  0;
	mAountOfSpritesOnSpriteSheetHeight =  0;

	mFilePathToSpriteSheet             = filePathToSpriteSheet.c_str();
	mBackgroundSpriteIndexStore        = nullptr;

	mSpriteSheet = new Texture2D(renderer);
	if (!mSpriteSheet->LoadFromFile(mFilePathToSpriteSheet))
	{
		std::cout << "Failed to load in sprite sheet for background sprites: " << mFilePathToSpriteSheet << std::endl;
		return;
	}

	if (!LoadInDataFromFile(filePathToDataFile, lookupConversion))
	{
		std::cout << "Failed to load in the background data from the file, check the formatting and that the file exists." << std::endl;
	}

	// This offset is pixels into the window, not the rendering offset of blocks
	mOffsetFromTopLeft                = offsetFromTopLeft;

	//if (mOffsetFromTopLeft.y != 0)
	//{
	//	mOffsetFromTopLeft.y -= mLevelHeight;
	//}
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
		for (unsigned int col = 0; col < mLevelWidth; col++)
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

		if (mAountOfSpritesOnSpriteSheetWidth == 0)
		{
			ssLine = std::stringstream(line);
			ssLine >> mAountOfSpritesOnSpriteSheetWidth;
			continue;
		}

		if (mAountOfSpritesOnSpriteSheetHeight == 0)
		{
			ssLine = std::stringstream(line);
			ssLine >> mAountOfSpritesOnSpriteSheetHeight;
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
				mBackgroundSpriteIndexStore[row] = new unsigned int[mLevelWidth - 1];
			}

			// The memory is allocated for access like this: store[row][column]
		}

		// Now we just need to load in and calculate the elements of the array
		for (unsigned int charID = 0; charID < line.size(); charID++)
		{
			// The charID represents which column it is on
			// Across then down
			mBackgroundSpriteIndexStore[currentRow][charID] = lookupConversion[line[charID]];
		}

		currentRow++;
	}

	// Now check if all the data has been created correctly
	if (!CheckAllDataLoaded())
	{
		return false;
	}

	// Now output the data stored
	//for (unsigned int row = 0; row < mLevelHeight; row++)
	//{
	//	for (unsigned int col = 0; col < mLevelWidth - 1; col++)
	//	{
	//		std::cout << mBackgroundSpriteIndexStore[row][col] << " ";
	//	}

	//	std::cout << std::endl;
	//}

	std::cout << std::endl;

	return true;
}

// --------------------------------------------------------------------------------------------------------------------------- //

void BackgroundLayer::Render()
{
	// First check if the sprite sheet has been loaded correctly
	if (mSpriteSheet)
	{
		// Setup the render rects
		SDL_Rect portionOfSpriteSheet{ 0, 0, RESOLUTION_OF_SPRITES, RESOLUTION_OF_SPRITES };
		SDL_Rect destRect{ 0, 0, RESOLUTION_OF_SPRITES, RESOLUTION_OF_SPRITES };

		// Now we need to loop through the level - rendering what is needed to be seen
		for (unsigned int row = 0; row < mLevelHeight; row++)
		{
			// Now loop through all of the columns that need to be rendered
			for (unsigned int col = 0; col < mLevelWidth; col++)
			{
				// Now find the correct portion of the sprite sheet that needs rendering
				portionOfSpriteSheet.x =     (mBackgroundSpriteIndexStore[row][col] % mAountOfSpritesOnSpriteSheetWidth) * RESOLUTION_OF_SPRITES;
				portionOfSpriteSheet.y = (int(mBackgroundSpriteIndexStore[row][col] / mAountOfSpritesOnSpriteSheetWidth)) * RESOLUTION_OF_SPRITES;

				// Now render the correct sprite in the correct position
				mSpriteSheet->Render(portionOfSpriteSheet, destRect);

				destRect.x += RESOLUTION_OF_SPRITES;
			}

			destRect.x = 0;
			destRect.y += RESOLUTION_OF_SPRITES;
		}
	}
	else
		std::cout << "No sprite sheet loaded for the level background!" << std::endl;


/*
	// If there is a sprite sheet then we can render
	if (mSpriteSheet)
	{
		// First convert the actual position into a grid position
		Vector2D gridReferencePoint = Commons_SMB3::ConvertFromRealPositionToGridPositionReturn(GameManager_SMB3::GetInstance()->GetRenderReferencePoint(), RESOLUTION_OF_SPRITES);

		// Quick outs so we dont process anything we dont need to
		if (gridReferencePoint.x > mOffsetFromTopLeft.x + mLevelWidth  + BACKGROUND_SPRITE_RENDER_WIDTH     || // To the far right of the section
			gridReferencePoint.x < mOffsetFromTopLeft.x                - BACKGROUND_SPRITE_RENDER_WIDTH	    || // To the far left of the section
			gridReferencePoint.y > mOffsetFromTopLeft.y                + BACKGROUND_SPRITE_RENDER_HEIGHT	|| // Below the section
			gridReferencePoint.y < mOffsetFromTopLeft.y - mLevelHeight - BACKGROUND_SPRITE_RENDER_HEIGHT) // Above the section
		{
			return;
		}
		else
		{

			// Now get the distance from this position to the next one, so that we can scroll smoothly
			Vector2D interGridPositionOffset = Vector2D(((int(gridReferencePoint.x) - gridReferencePoint.x) * RESOLUTION_OF_SPRITES), ((int(gridReferencePoint.y) - gridReferencePoint.y) * RESOLUTION_OF_SPRITES));

			SDL_Rect portionOfSpriteSheet, destRect;

			// Setup default values like this as SDL_Rect doesnt have a useful constructor
			portionOfSpriteSheet.x = 0;
			portionOfSpriteSheet.y = 0;
			portionOfSpriteSheet.w = RESOLUTION_OF_SPRITES;
			portionOfSpriteSheet.h = RESOLUTION_OF_SPRITES;

			destRect.x = (int)interGridPositionOffset.x;
			destRect.y = (int)interGridPositionOffset.y + (mOffsetFromTopLeft.y * RESOLUTION_OF_SPRITES);
			destRect.w = RESOLUTION_OF_SPRITES;
			destRect.h = RESOLUTION_OF_SPRITES;

			// Loop through the internal store of sprite indexes and render the correct ones in the correct positions
			// Must be a signed int to allow for negative values, and negative scrolling
			for (int row = (int)gridReferencePoint.y; row < gridReferencePoint.y + BACKGROUND_SPRITE_RENDER_HEIGHT; row++)
			{
				// Checks to remove scope errors and to make sure we are only rendering what is on the screen
				if (row >= (int)mLevelHeight || row < 0)
					continue;

				for (int col = (int)gridReferencePoint.x - mOffsetFromTopLeft.x; col < (gridReferencePoint.x + BACKGROUND_SPRITE_RENDER_WIDTH) - mOffsetFromTopLeft.x; col++)
				{
					if (col >= (int)mLevelWidth || col < 0)
					{
						// Make sure to add the offset on even if we are not rendering this column
						destRect.x += RESOLUTION_OF_SPRITES;
						continue;
					}

					// Now we are correctly looping through the correct area of the grid to be rendered
					// Now we just need to render the correct sprite in the correct position
					portionOfSpriteSheet.x =       (mBackgroundSpriteIndexStore[row][col] % mAountOfSpritesOnSpriteSheetWidth) * RESOLUTION_OF_SPRITES;
					portionOfSpriteSheet.y = ((int)(mBackgroundSpriteIndexStore[row][col] / mAountOfSpritesOnSpriteSheetWidth)) * RESOLUTION_OF_SPRITES;

					mSpriteSheet->Render(portionOfSpriteSheet, destRect);

					destRect.x += RESOLUTION_OF_SPRITES;
				}

				destRect.y += RESOLUTION_OF_SPRITES;
				destRect.x = (int)interGridPositionOffset.x;
			}
		}
	}

	*/
}

// --------------------------------------------------------------------------------------------------------------------------- //
