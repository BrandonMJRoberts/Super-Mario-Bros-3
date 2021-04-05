#include "Crab.h"

#include "Texture2D.h"
#include "LevelMap.h"

Texture2D*   Crab::mSpriteSheet = nullptr;
unsigned int Crab::mCrabCount = 0;

// ------------------------------------------------------ //

Crab::Crab(SDL_Renderer* renderer, const char* filePathToSpriteSheet, const float timePerFrame, Vector2D startPos, Vector2D collisionBox, bool spawningOnLeftSide, const float movementSpeed, unsigned int colourID)
	: RenderObject(0, 2, 0, timePerFrame, startPos, 7, 6, collisionBox, movementSpeed)
{
	mFacingLeft = !spawningOnLeftSide;

	// Load in the sprite sheet
	if (mCrabCount == 0)
	{
		mSpriteSheet = new Texture2D(renderer);
		if (!mSpriteSheet->LoadFromFile(filePathToSpriteSheet))
		{
			std::cout << "Failed to load sprite sheet for coins" << std::endl;
		}
	}

	mCrabCount++;

	switch (colourID)
	{
	default:
	case 0:
	break;

	case 1:
		mCurrentSpriteID = 14;
		mEndSpriteID     = 16;
		mStartFrameID    = 14;
	break;

	case 2:
		mCurrentSpriteID = 28;
		mEndSpriteID     = 30;
		mStartFrameID    = 28;
	break;
	}

	SetupRenderRects();
}

// ------------------------------------------------------ //

Crab::~Crab() 
{
	if (mCrabCount == 1)
	{
		delete mSpriteSheet;
		mSpriteSheet = nullptr;
	}

	mCrabCount--;
}

// ------------------------------------------------------ //