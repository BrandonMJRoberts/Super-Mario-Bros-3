#include "BackgroundLayer_WorldMap.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "Texture2D.h"
#include "Constants_SMB3.h"
#include "GameManager_SMB3.h"

// ----------------------------------------------------------------- //

BackgroundLayer_WorldMap::BackgroundLayer_WorldMap(const std::string filePath
, SDL_Renderer* renderer
, std::map<char, unsigned int> conversionTable)
: mWidth(0)
, mHeight(0)
, mSpritesOnSpriteSheetHeight(0)
, mSpritesOnSpriteSheetWidth(0)
, mBackgroundIndexStore(nullptr)
, mMapPortionOffset(0, 0)           // The offset into the index store, for scrolling of the background without moving the actual render positions
, mSpriteSheet(nullptr)
{
	// First load in the sprite sheet
	mSpriteSheet = new Texture2D(renderer);
	if (!mSpriteSheet->LoadFromFile(filePath + "/BackgroundSprites.png"))
	{
		std::cout << "Failed to load in the sprite sheet of background sprites: world map background." << std::endl;
		return;
	}

	// Now we need to load in the data from the file associated with this world
	LoadInDataFromFile(filePath + "/Background Layer.txt", conversionTable);
}

// ----------------------------------------------------------------- //

BackgroundLayer_WorldMap::~BackgroundLayer_WorldMap()
{
	// Clean up the memory we have used
	for (unsigned int row = 0; row < mHeight; row++)
	{
		delete[] mBackgroundIndexStore[row];
	}

	delete[] mBackgroundIndexStore;
	mBackgroundIndexStore = nullptr;

	delete mSpriteSheet;
	mSpriteSheet = nullptr;
}

// ----------------------------------------------------------------- //

void BackgroundLayer_WorldMap::Render()
{
	if (mSpriteSheet)
	{
		// First get the physical render offset from the top left
		Vector2D renderOffset = Commons_SMB3::ConvertFromGridPositionToRealPositionReturn(GameManager_SMB3::GetInstance()->GetWorldMapRenderOffset(), RESOLUTION_OF_SPRITES);

		// Setup default values like this as SDL_Rect doesnt have a useful constructor
		SDL_Rect portionOfSpriteSheet { 0, 0, RESOLUTION_OF_SPRITES, RESOLUTION_OF_SPRITES };

		SDL_Rect destRect             { int(renderOffset.x),
			                            int(renderOffset.y),
			                            RESOLUTION_OF_SPRITES, 
			                            RESOLUTION_OF_SPRITES };

		unsigned int indexToRender = 0;

		// Loop through all of the indexes that we want to render
		for (int row = (int)mMapPortionOffset.y; row < mHeight + mMapPortionOffset.y; row++)
		{
			// Now loop through the columns
			for (int col = (int)mMapPortionOffset.x; col < mWidth + mMapPortionOffset.x; col++)
			{
				// Get what index we want to render
				if (row < (int)mHeight && row > -1 && col > -1 && col < (int)mWidth)
					indexToRender = mBackgroundIndexStore[row][col];
				else
					continue;

				// We need to check if the current sprite to be rendered is one of the sprites that animate, if it is then we need to swap it out for the correct one
				for (unsigned int i = 0; i < mAnimatingSpriteData.size(); i++)
				{
					if (mBackgroundIndexStore[row][col] == mAnimatingSpriteData[i].GetDataValueIndex())
					{
						indexToRender += mAnimatingSpriteData[i].GetCurrentFrameOffset();
						break; // Leave the for loop
					}
				}

				// Calculate where the correct sprite is on the sprite sheet
				portionOfSpriteSheet.x =       (indexToRender % mSpritesOnSpriteSheetWidth) * RESOLUTION_OF_SPRITES;
				portionOfSpriteSheet.y = ((int)(indexToRender / mSpritesOnSpriteSheetWidth)) * RESOLUTION_OF_SPRITES;

				// Now render the sprite
				mSpriteSheet->Render(portionOfSpriteSheet, destRect);

				// Move the render position along
				destRect.x += RESOLUTION_OF_SPRITES;
			}

			destRect.y += RESOLUTION_OF_SPRITES;
			destRect.x = int(renderOffset.x);
		}
	}
	else
	{
		std::cout << "Sprite sheet is a null value!" << std::endl;
	}
}

// ----------------------------------------------------------------- //

void BackgroundLayer_WorldMap::Update(const float deltaTime, SDL_Event e)
{
	// Loop through all animating sprite data stores and update them
	for (unsigned int i = 0; i < mAnimatingSpriteData.size(); i++)
	{
		mAnimatingSpriteData[i].Update(deltaTime);
	}
}

// ----------------------------------------------------------------- //

void BackgroundLayer_WorldMap::LoadInDataFromFile(std::string filePath, std::map<char, unsigned int> conversionTable)
{
	// First open the file needed
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		std::cout << "Failed to open the data file for the background of the world map." << std::endl;
		return;
	}

	// Now we know the file is open we can read in the data
	std::string       sLine;
	std::stringstream ssLine;

	unsigned int      currentRow    = 0;
	unsigned int      failSafeCount = 0;

	unsigned int      lengthOfAnimationFrames;
	int               spritesWithAnimation = -1;
	float             timePerAnim;
	char              nameOfSprite;

	while (getline(file, sLine))
	{
		failSafeCount++;

		if (failSafeCount > FAILSAFE_MAX_COUNT_FILE_LOADING_LOOPS)
		{
			std::cout << "Error loading in the data file for the background of the current world" << std::endl;
			file.close();
			return;
		}

		// Check if we should discount this line first, this should happen if it is empty or is a comment
		if (sLine[0] == '#' || sLine == "")
			continue;

		// If this is the final line then we want to close the final and return back to where we came from
		if (sLine == "END")
		{
			file.close();
			return;
		}
		
		// Convert the string into an easier to use data type
		ssLine = std::stringstream(sLine);

		// Get the data from the file in the correct order required from the file
		// This order is stated in each file themselves
		if (mWidth == 0)
		{
			ssLine >> mWidth;
			continue;
		}

		if (mHeight == 0)
		{
			ssLine >> mHeight;
			continue;
		}

		if (mSpritesOnSpriteSheetWidth == 0)
		{
			ssLine >> mSpritesOnSpriteSheetWidth;
			continue;
		}

		if (mSpritesOnSpriteSheetHeight == 0)
		{
			ssLine >> mSpritesOnSpriteSheetHeight;
			continue;
		}

		// Now load in the amount of sprites on this sprite sheet that have animation frames
		if (spritesWithAnimation == -1)
		{
			ssLine >> spritesWithAnimation;
			continue;
		}

		// Now load in the correct amount of data for the amount of animating sprites of the background
		if (mAnimatingSpriteData.size() != spritesWithAnimation)
		{
			// Load in the data
			ssLine >> nameOfSprite >> lengthOfAnimationFrames >> timePerAnim;
			mAnimatingSpriteData.push_back(SpriteThatAnimatesData(conversionTable[nameOfSprite], lengthOfAnimationFrames, timePerAnim));
			continue;
		}

		// If the background data store has not been created before then do it now
		if (!mBackgroundIndexStore)
		{
			// Allocate the correct amount of memory for this task
			mBackgroundIndexStore = new unsigned int* [mHeight];
			for (unsigned int i = 0; i < mHeight; i++)
			{
				mBackgroundIndexStore[i] = new unsigned int[mWidth - 1];
			}
		}

		// Now we need to load in and allocate the data from the file
		for (unsigned int charID = 0; charID < sLine.size(); charID++)
		{
			mBackgroundIndexStore[currentRow][charID] = conversionTable[(char)sLine[charID]];
		}

		// Increment the current row being written to
		currentRow++;
	}
}

// ----------------------------------------------------------------- //

SpriteThatAnimatesData::SpriteThatAnimatesData(unsigned int index, unsigned int len, float timePerChange) 
{ 
	mDataValueIndex          = index;
	mAmountOfAnimationFrames = len; 
	mAnimationTimePerFrame   = timePerChange; 
	mTimeRemainingTillChange = mAnimationTimePerFrame;
	mCurrentFrameOffset      = 0;
}

// ----------------------------------------------------------------- //

void SpriteThatAnimatesData::Update(const float deltaTime)
{
	mTimeRemainingTillChange -= deltaTime;  
	
	if (mTimeRemainingTillChange <= 0.0f)
	{
		mTimeRemainingTillChange = mAnimationTimePerFrame;

		mCurrentFrameOffset++;

		if (mCurrentFrameOffset >= mAmountOfAnimationFrames)
			mCurrentFrameOffset = 0;
	}
}

// ----------------------------------------------------------------- //

void BackgroundLayer_WorldMap::ReplaceDataPoint(unsigned int row, unsigned int col, unsigned int newVal)
{
	if (row >= mHeight || col >= mWidth || row < 0 || col < 0)
		return;

	mBackgroundIndexStore[row][col] = newVal;
}

// ----------------------------------------------------------------- //

unsigned int BackgroundLayer_WorldMap::GetSpecificDataPoint(unsigned int row, unsigned int col)
{
	// Bounds check
	if (row >= mHeight || col >= mWidth || row < 0 || col < 0)
		return 300;

	return mBackgroundIndexStore[row][col];
}

// ----------------------------------------------------------------- //
