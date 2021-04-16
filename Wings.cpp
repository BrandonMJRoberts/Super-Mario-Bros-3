#include "Wings.h"

#include <SDL.h>

#include "Texture2D.h"
#include "Constants_SMB3.h"

Texture2D*   Wings::mThisSpriteSheet   = nullptr;
unsigned int Wings::mThisObjectCounter = 0;

// ------------------------------------------------------------- //

Wings::Wings(Vector2D startPos, SDL_Renderer* renderer) 
	: BaseAnimation(startPos, 0, 1, 0.05f, Vector2D(0.0f, 0.0f), 2, 3)
	, mLostWings(false)
{
	mThisObjectCounter++;

	if (mThisSpriteSheet == nullptr)
	{
		mThisSpriteSheet = new Texture2D(renderer);
		if (!mThisSpriteSheet->LoadFromFile("SDL_Mario_Project/Enemies/Wing/Wing.png"))
		{
			std::cout << "Failed to load in sprite sheet for the wings animation." << std::endl;
		}
	}

	if (!mThisSpriteSheet || kSpritesOnWidth == 0 || kSpritesOnHeight == 0)
		return;

	mSingleSpriteWidth  = mThisSpriteSheet->GetWidth() / kSpritesOnWidth;
	mSingleSpriteHeight = mThisSpriteSheet->GetHeight() / kSpritesOnHeight;
}

// ------------------------------------------------------------- //

Wings::~Wings()
{
	mThisObjectCounter--;
	if (mThisObjectCounter == 0)
	{
		delete mThisSpriteSheet;
		mThisSpriteSheet = nullptr;
	}
}

// ------------------------------------------------------------- //

bool Wings::Update(const float deltaTime)
{
	UpdateAnimations(deltaTime);

	if (!mCompletedAnimation)
	{
		// Handle which animation we are doing
		if (mLostWings)
		{
			mVelocity.y -= GRAVITY * deltaTime;

			if (mPosition.y > SCREEN_HEIGHT_SMB3)
			{
				mCompletedAnimation = true;
			}
		}

		mPosition -= mVelocity * deltaTime;
	}

	// Now for animation specific code
	return false;
}

// ------------------------------------------------------------- //

void Wings::Flap()
{
	
}

// ------------------------------------------------------------- //

void Wings::LoseWings()
{
	mLostWings = true;

	mCurrentFrameID = 1;
	mEndFrameID     = 1;
	mStartFrameID   = 1;

	mVelocity.y     = -3.0f;
}

// ------------------------------------------------------------- //