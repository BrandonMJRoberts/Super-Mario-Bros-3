#include "CoinRelaseAnimation.h"

#include "Constants_SMB3.h"
#include "Texture2D.h"

#include <SDL.h>

Texture2D*   CoinReleaseAnimation::mThisSpriteSheet   = nullptr;
unsigned int CoinReleaseAnimation::mThisObjectCounter = 0;

// ----------------------------------------------------------------------------------- //

CoinReleaseAnimation::CoinReleaseAnimation(Vector2D startPos, SDL_Renderer* renderer) 
	: BaseAnimation(startPos, 0, 4, 0.05f, Vector2D(0.0f, 8.0f), 8, 1)
	, mReleased(false)
{
	mThisObjectCounter++;

	if (mThisSpriteSheet == nullptr)
	{
		mThisSpriteSheet = new Texture2D(renderer);
		if (!mThisSpriteSheet->LoadFromFile("SDL_Mario_Project/Objects/Coin Release Anim.png"))
		{
			std::cout << "Failed to load in sprite sheet for the coin sprite animation." << std::endl;
		}
	}

	if (!mThisSpriteSheet || kSpritesOnWidth == 0 || kSpritesOnHeight == 0)
		return;

	mSingleSpriteWidth  = mThisSpriteSheet->GetWidth()  / kSpritesOnWidth;
	mSingleSpriteHeight = mThisSpriteSheet->GetHeight() / kSpritesOnHeight;
}

// ----------------------------------------------------------------------------------- //

CoinReleaseAnimation::~CoinReleaseAnimation()
{
	mThisObjectCounter--;
	if (mThisObjectCounter == 0)
	{
		delete mThisSpriteSheet;
		mThisSpriteSheet = nullptr;
	}
}

// ----------------------------------------------------------------------------------- //

bool CoinReleaseAnimation::Update(const float deltaTime)
{
	if (mReleased && !mCompletedAnimation)
	{
		mVelocity.y -= GRAVITY * deltaTime;
		mPosition   -= mVelocity * deltaTime;

		if (mVelocity.y < -5.0f)
		{
			mCompletedAnimation = true;

			mEndFrameID     = 7;
			mStartFrameID   = 5;
			mCurrentFrameID = 5;

			mTimePerFrame *= 2.0f;
		}
	}

	UpdateAnimations(deltaTime);

	return (mCompletedAnimation && mCurrentFrameID > mEndFrameID);
}

// ----------------------------------------------------------------------------------- //

void CoinReleaseAnimation::SetReleased()
{
	mReleased = true;
}

// ----------------------------------------------------------------------------------- //