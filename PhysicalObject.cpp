#include "PhysicalObject.h"

#include "Constants_SMB3.h"

SDL_Renderer*			  PhysicalObject::mRenderer(nullptr);
std::vector<Texture2D*>   PhysicalObject::mSpriteSheets;
std::vector<unsigned int> PhysicalObject::mInstanceCounts;

// ----------------------------------------------------------------------------------------------------- //

PhysicalObject::PhysicalObject(const Vector2D spawnPosition
	, const bool          startSpawnedInLevel
	, SDL_Renderer*       renderer
	, const std::string   filePathToSpriteSheet
	, const unsigned int  spritesOnWidth
	, const unsigned int  spritesOnHeight
	, const double        collisionBoxWidth
	, const double        collsiionBoxHeight
	, const float         timePerFrame) 

: BaseObject(spawnPosition
, startSpawnedInLevel)
, mCurrentSpriteID(0)
, mStartSpriteID(0)
, mEndSpriteID(0)
, mSingleSpriteWidth(0)
, mSingleSpriteHeight(0)
, mSpritesOnWidth(spritesOnWidth)
, mSpritesOnHeight(spritesOnHeight)
, mTimePerFrame(timePerFrame)
, mTimeRemainingTillNextFrame(timePerFrame)
, mThisSpriteSheet(nullptr)
, mCollisionBox(collisionBoxWidth, collsiionBoxHeight)
{
	if (!mRenderer)
		mRenderer = renderer;

	bool         alreadyExists   = false;
	unsigned int indexOfExisting = 0;

	// Check if we already have a sprite sheet of this loaded
	for (unsigned int i = 0; i < mSpriteSheets.size(); i++)
	{
		if (filePathToSpriteSheet == mSpriteSheets[i]->GetFilePath())
		{
			alreadyExists   = true;
			indexOfExisting = i;
			break;
		}
	}

	if (!alreadyExists)
	{
		Texture2D* newSpriteSheet = new Texture2D(renderer);
		if (!newSpriteSheet->LoadFromFile(filePathToSpriteSheet))
		{
			std::cout << "Invalid file path passed into the object loading code!" << std::endl;
			delete newSpriteSheet;
			return;
		}
		else
		{
			// Add it to the list
			mSpriteSheets.push_back(newSpriteSheet);
			mInstanceCounts.push_back(1);

			// Set this sheet to be the new one
			mThisSpriteSheet = newSpriteSheet;
		}
	}
	else
	{
		mThisSpriteSheet = mSpriteSheets[indexOfExisting];
	}

	// Make sure to setup the sprite width and height
	if (mThisSpriteSheet && spritesOnWidth != 0 && spritesOnHeight != 0)
	{
		mSingleSpriteWidth  = mThisSpriteSheet->GetWidth() / spritesOnWidth;
		mSingleSpriteHeight = mThisSpriteSheet->GetHeight() / spritesOnHeight;
	}
}

// ----------------------------------------------------------------------------------------------------- //

PhysicalObject::~PhysicalObject()
{
	// Find what index we are currently on and remove one from the instance count
	for (unsigned int i = 0; i < mSpriteSheets.size(); i++)
	{
		if (mThisSpriteSheet->GetFilePath() == mSpriteSheets[i]->GetFilePath())
		{
			mInstanceCounts[i]--;

			if (mInstanceCounts[i] == 0)
			{
				delete mSpriteSheets[i];
				mSpriteSheets[i] = nullptr;

				mSpriteSheets.erase(mSpriteSheets.begin() + i);
			}

			mInstanceCounts.erase(mInstanceCounts.begin() + i);

			break;
		}
	}

	if (mSpriteSheets.size() == 0)
		mRenderer = nullptr;
}

// ----------------------------------------------------------------------------------------------------- //

void PhysicalObject::Render(const Vector2D renderReferencePoint)
{
	if (mThisSpriteSheet)
	{
		Vector2D renderPos = Vector2D(mCurrentPosition.x - renderReferencePoint.x, mCurrentPosition.y - renderReferencePoint.y);

		SDL_Rect portionOfSpriteSheet{ (int)(mCurrentSpriteID % mSpritesOnWidth) * (int)mSingleSpriteWidth,
									  (int)(mCurrentSpriteID / mSpritesOnWidth) * (int)mSingleSpriteHeight,
									  (int)mSingleSpriteWidth,
									  (int)mSingleSpriteHeight };

		SDL_Rect destRect           {  int(renderPos.x * RESOLUTION_OF_SPRITES),
									  int(renderPos.y * RESOLUTION_OF_SPRITES) - (int)mSingleSpriteHeight,
									 (int)mSingleSpriteWidth,
									 (int)mSingleSpriteHeight };

		mThisSpriteSheet->Render(portionOfSpriteSheet, destRect);
	}
}

// ----------------------------------------------------------------------------------------------------- //

bool PhysicalObject::Update(const float deltaTime, const Vector2D playerPosition)
{
	// Update the frame time and adjust the current frame if needed
	mTimeRemainingTillNextFrame -= deltaTime;

	if (mTimeRemainingTillNextFrame <= 0.0f)
	{
		mTimeRemainingTillNextFrame = mTimePerFrame;

		mCurrentSpriteID++;

		if (mCurrentSpriteID > mEndSpriteID)
		{
			mCurrentSpriteID = mStartSpriteID;
		}
	}

	return false;
}

// ----------------------------------------------------------------------------------------------------- //