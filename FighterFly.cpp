#include "FighterFly.h"

#include "Texture2D.h"

#include "LevelMap.h"

Texture2D*   FighterFly::mSpriteSheet      = nullptr;
unsigned int FighterFly::mFighterFlyCount  = 0;

// --------------------------------------------------------- //

FighterFly::FighterFly(SDL_Renderer* renderer, const char* filePathToSpriteSheet, const float timePerFrame, Vector2D startPos, Vector2D collisionBox, bool spawningOnLeftSide, const float movementSpeed, unsigned int colourID)
	: RenderObject(0, 1, 0, timePerFrame, startPos, 7, 3, collisionBox, movementSpeed)
{
	mFacingLeft = !spawningOnLeftSide;

	// Load in the sprite sheet
	if (mFighterFlyCount == 0)
	{
		mSpriteSheet = new Texture2D(renderer);
		if (!mSpriteSheet->LoadFromFile(filePathToSpriteSheet))
		{
			std::cout << "Failed to load sprite sheet for coins" << std::endl;
		}
	}

	mFighterFlyCount++;

	switch (colourID)
	{
	default:
	case 0:
	break;

	case 1:
		mCurrentSpriteID = 7;
		mEndSpriteID     = 8;
		mStartFrameID    = 7;
	break;

	case 2:
		mCurrentSpriteID = 14;
		mEndSpriteID     = 15;
		mStartFrameID    = 14;
	break;
	}

	SetupRenderRects();
}

// --------------------------------------------------------- //

FighterFly::~FighterFly()
{
	if (mFighterFlyCount == 1)
	{
		delete mSpriteSheet;
		mSpriteSheet = nullptr;
	}

	mFighterFlyCount--;
}

// --------------------------------------------------------- //