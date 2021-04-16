#include "CoinRelaseAnimation.h"

#include "Constants_SMB3.h"
#include "Texture2D.h"

#include <SDL.h>

Texture2D*   CoinReleaseAnimation::mThisSpriteSheet   = nullptr;
unsigned int CoinReleaseAnimation::mThisObjectCounter = 0;

// ----------------------------------------------------------------------------------- //

CoinReleaseAnimation::CoinReleaseAnimation(Vector2D startPos, SDL_Renderer* renderer)
	: kSpritesOnWidth(8),
	kSpritesOnHeight(1),

	mPosition(startPos),

	mCurrentFrameID(0),
	mStartFrameID(0),
	mEndFrameID(4),

	mSingleSpriteWidth(0),
	mSingleSpriteHeight(0),

	mCompletedAnimation(false),

	mTimePerFrame(0.05f),
	mTimeRemainingForFrame(mTimePerFrame),
	mVelocity(0.0f, 8.0f)
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

void CoinReleaseAnimation::Render(const Vector2D offset)
{
	if (mThisSpriteSheet)
	{
		Vector2D renderPos = Vector2D(mPosition.x - offset.x, mPosition.y - offset.y);

		SDL_Rect portionOfSpriteSheet{ (int)(mCurrentFrameID % kSpritesOnWidth) * (int)mSingleSpriteWidth,
									   (int)(mCurrentFrameID / kSpritesOnWidth) * (int)mSingleSpriteHeight,
									   (int)mSingleSpriteWidth,
									   (int)mSingleSpriteHeight };

		SDL_Rect destRect{            int(renderPos.x * RESOLUTION_OF_SPRITES),
									  int(renderPos.y * RESOLUTION_OF_SPRITES) - (int)mSingleSpriteHeight + 2,
									 (int)mSingleSpriteWidth,
									 (int)mSingleSpriteHeight };

		mThisSpriteSheet->Render(portionOfSpriteSheet, destRect, SDL_FLIP_NONE);
	}
}

// ----------------------------------------------------------------------------------- //

bool CoinReleaseAnimation::Update(const float deltaTime)
{
	if (!mCompletedAnimation)
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

	mTimeRemainingForFrame -= deltaTime;
	if (mTimeRemainingForFrame < 0.0f)
	{
		mTimeRemainingForFrame = mTimePerFrame;

		mCurrentFrameID++;

		if (mCurrentFrameID > mEndFrameID && !mCompletedAnimation)
			mCurrentFrameID = mStartFrameID;
	}

	return (mCompletedAnimation && mCurrentFrameID > mEndFrameID);
}

// ----------------------------------------------------------------------------------- //