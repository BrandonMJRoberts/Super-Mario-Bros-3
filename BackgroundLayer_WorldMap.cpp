#include "BackgroundLayer_WorldMap.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "Texture2D.h"
#include "Constants_SMB3.h"
#include "GameManager_SMB3.h"

// ----------------------------------------------------------------- //

BackgroundLayer_WorldMap::BackgroundLayer_WorldMap(const std::string filePath, SDL_Renderer* renderer, std::map<char, unsigned int> conversionTable)
{
	mWidth						= 0;
	mHeight					    = 0;
	mSpritesOnSpriteSheetWidth  = 0;
	mSpritesOnSpriteSheetHeight = 0;
	mBackgroundIndexStore       = nullptr;

	// First load in the sprite sheet
	mSpriteSheet = new Texture2D(renderer);
	if (!mSpriteSheet->LoadFromFile(filePath + "/BackgroundSprites.png"))
	{
		std::cout << "Failed to load in the sprite sheet of background sprites: world map background." << std::endl;
		return;
	}

	// Now we need to load in the data from the file associated with this world
	LoadInDataFromFile(filePath + "/Background Layer.txt", conversionTable);

	// Store the render offset from the top left of the screen
	mOffsetFromTopLeft = GameManager_SMB3::GetInstance()->GetWorldMapRenderOffset();
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
	// If there is a sprite sheet then we can render
	if (mSpriteSheet)
	{
		unsigned int indexToRender;

		// First convert the actual position into a grid position
		Vector2D gridReferencePoint = Commons_SMB3::ConvertFromRealPositionToGridPositionReturn(GameManager_SMB3::GetInstance()->GetRenderReferencePoint(), RESOLUTION_OF_SPRITES);

		// Quick outs so we dont process anything we dont need to
		if (gridReferencePoint.x > mOffsetFromTopLeft.x + mWidth + BACKGROUND_SPRITE_RENDER_WIDTH || // To the far right of the section
			gridReferencePoint.x < mOffsetFromTopLeft.x - BACKGROUND_SPRITE_RENDER_WIDTH          || // To the far left of the section
			gridReferencePoint.y > mOffsetFromTopLeft.y + BACKGROUND_SPRITE_RENDER_HEIGHT         || // Below the section
			gridReferencePoint.y < mOffsetFromTopLeft.y - mHeight - BACKGROUND_SPRITE_RENDER_HEIGHT) // Above the section
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

			destRect.x			   = (int)interGridPositionOffset.x + ((int)mOffsetFromTopLeft.x * RESOLUTION_OF_SPRITES);
			destRect.y			   = (int)interGridPositionOffset.y + ((int)mOffsetFromTopLeft.y * RESOLUTION_OF_SPRITES);
			destRect.w			   = RESOLUTION_OF_SPRITES;
			destRect.h		       = RESOLUTION_OF_SPRITES;

			// Loop through the internal store of sprite indexes and render the correct ones in the correct positions
			// Must be a signed int to allow for negative values, and negative scrolling
			for (int row = (int)gridReferencePoint.y; row < gridReferencePoint.y + BACKGROUND_SPRITE_RENDER_HEIGHT; row++)
			{
				// Checks to remove scope errors and to make sure we are only rendering what is on the screen
				if (row >= (int)mHeight || row < 0)
					continue;

				for (int col = (int)gridReferencePoint.x - (int)mOffsetFromTopLeft.x; col < ((int)gridReferencePoint.x + BACKGROUND_SPRITE_RENDER_WIDTH) - (int)mOffsetFromTopLeft.x; col++)
				{
					if (col >= (int)mWidth || col < 0)
					{
						// Make sure to add the offset on even if we are not rendering this column
						destRect.x += RESOLUTION_OF_SPRITES;
						continue;
					}

					indexToRender = mBackgroundIndexStore[row][col];

					// First we need to check if the current sprite to be rendered is one of the sprites that animate, if it is then we need to swap it out for the correct one
					// Loop through the data stores and check if any of the values match
					for (unsigned int i = 0; i < mAnimatingSpriteData.size(); i++)
					{
						if (mBackgroundIndexStore[row][col] == mAnimatingSpriteData[i].GetDataValueIndex())
						{
							indexToRender += mAnimatingSpriteData[i].GetCurrentFrameOffset();
							break; // Leave the for loop
						}
					}

					// Now we are correctly looping through the correct area of the grid to be rendered
					// Now we just need to render the correct sprite in the correct position
					portionOfSpriteSheet.x =	   (indexToRender % mSpritesOnSpriteSheetWidth) * RESOLUTION_OF_SPRITES;
					portionOfSpriteSheet.y = ((int)(indexToRender / mSpritesOnSpriteSheetWidth)) * RESOLUTION_OF_SPRITES;

					mSpriteSheet->Render(portionOfSpriteSheet, destRect);

					destRect.x += RESOLUTION_OF_SPRITES;
				}

				destRect.y += RESOLUTION_OF_SPRITES;
				destRect.x = (int)interGridPositionOffset.x + ((int)mOffsetFromTopLeft.x * RESOLUTION_OF_SPRITES);
			}
		}
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
	char*             cLine = new char[100];
	std::string       sLine;
	std::stringstream ssLine;

	unsigned int      currentRow = 0;

	unsigned int      failSafeCount = 0;

	int               spritesWithAnimation = -1;
	char              nameOfSprite;
	unsigned int      lengthOfAnimationFrames;
	float             timePerAnim;

	while (file.getline(cLine, 100))
	{
		failSafeCount++;

		if (failSafeCount > FAILSAFE_MAX_COUNT_FILE_LOADING_LOOPS)
		{
			std::cout << "Error loading in the data file for the background of the current world" << std::endl;
			return;
		}

		// First convert the char array to a string, and the string to a string stream
		sLine = cLine;
	
		if (sLine == "END" || sLine[0] == '#' || sLine == "")
			continue;
		
		ssLine = std::stringstream(sLine);

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

		// Now load in the correct amount of data 
		if (mAnimatingSpriteData.size() != spritesWithAnimation)
		{
			// Load in the data
			ssLine >> nameOfSprite >> lengthOfAnimationFrames >> timePerAnim;
			mAnimatingSpriteData.push_back(SpriteThatAnimatesData(conversionTable[nameOfSprite], lengthOfAnimationFrames, timePerAnim));
			continue;
		}

		if (!mBackgroundIndexStore)
		{
			// Memory allocation
			mBackgroundIndexStore = new unsigned int* [mHeight];
			for (unsigned int i = 0; i < mHeight; i++)
			{
				mBackgroundIndexStore[i] = new unsigned int[mWidth - 1];
			}
		}

		// Now we need to load in and allocate the data from the file
		for (unsigned int charID = 0; charID < sLine.size(); charID++)
		{
			mBackgroundIndexStore[currentRow][charID] = conversionTable[sLine[charID]];
		}

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
	if (row >= mHeight || col >= mWidth || row < 0 || col < 0)
		return 300;

	return mBackgroundIndexStore[row][col];
}

// ----------------------------------------------------------------- //
