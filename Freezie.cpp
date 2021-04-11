#include "Freezie.h"

#include "Texture2D.h"

#include "LevelMap.h"

Texture2D*   Freezie::mSpriteSheet  = nullptr;
unsigned int Freezie::mFreezieCount = 0;

// --------------------------------------------------------- //

Freezie::Freezie(SDL_Renderer* renderer, const char* filePathToSpriteSheet, const float timePerFrame, Vector2D startPos, Vector2D collisionBox, bool spawningOnLeftSide, const float movementSpeed)
	: RenderObject(0, 2, 0, timePerFrame, startPos, 8, 1, collisionBox, movementSpeed)

	, mIsCracking(false)
{
	mFacingLeft = !spawningOnLeftSide;

	// Load in the sprite sheet
	if (mFreezieCount == 0)
	{
		mSpriteSheet = new Texture2D(renderer);
		if (!mSpriteSheet->LoadFromFile(filePathToSpriteSheet))
		{
			std::cout << "Failed to load sprite sheet for coins" << std::endl;
		}
	}

	mFreezieCount++;

	SetupRenderRects();
}

// --------------------------------------------------------- //

Freezie::~Freezie()
{
	if (mFreezieCount == 1)
	{
		delete mSpriteSheet;
		mSpriteSheet = nullptr;
	}

	mFreezieCount--;
}

// --------------------------------------------------------- //

void Freezie::SetPOWHit()
{
	mIsCracking      = true;
	mIsFlipped       = true;
	mCrackCountdown  = kTimePerFrame * 4; // give time for the animation

	mCurrentSpriteID = 3;
	mEndSpriteID     = 7;
	mStartFrameID    = 3;

	mVelocity.x      = 0.0f;
}

// ------------------------------------------------------ //

bool Freezie::ClassSpecificUpdate(const float deltaTime)
{
	if (mIsCracking && mGrounded)
	{
		mCrackCountdown -= deltaTime;

		if (mCrackCountdown < 0.0f)
		{
			return true;
		}
	}

	return false;
}

// ------------------------------------------------------ //