#include "Pipe_SMB1.h"

#include "Texture2D.h"

#include <SDL.h>

Texture2D*   PIPE_SMB1::mSpriteSheet = nullptr;
unsigned int PIPE_SMB1::mPipeCount   = 0;

// --------------------------------------------------------------------------------------- //

PIPE_SMB1::PIPE_SMB1(SDL_Renderer* renderer, const char* filePathToSpriteSheet, PIPE_FACING_DIRECTION_SMB1 facingDirection, Vector2D bottomLeftPosition, const float timePerFrame, Vector2D collisionBox)
	: RenderObject(0, 0, 0, timePerFrame, bottomLeftPosition, 1, 1, collisionBox, 0.0f)

	, mPipeFacingDirection(facingDirection)

	, mAnimationDirectionIsRight(false)
	, mDoingAnimation(false)
{
	if (mSpriteSheet == nullptr)
	{
		mSpriteSheet = new Texture2D(renderer);
		if (!mSpriteSheet->LoadFromFile(filePathToSpriteSheet))
		{
			std::cout << "Failed to load sprite sheet for pipes" << std::endl;
		}
	}

	if (mPipeFacingDirection == PIPE_FACING_DIRECTION_SMB1::LEFT)
		mFacingLeft = true;
	else
		mFacingLeft = false;

	mPipeCount++;

	SetupRenderRects();
}

// --------------------------------------------------------------------------------------- //

PIPE_SMB1::~PIPE_SMB1()
{
	if (mPipeCount == 1)
	{
		delete mSpriteSheet;
		mSpriteSheet = nullptr;
	}

	mPipeCount--;
}

// --------------------------------------------------------------------------------------- //

bool PIPE_SMB1::Update(const float deltaTime, LevelMap* levelMap)
{
	if (mDoingAnimation)
	{
		mTimeRemainingPerFrame -= deltaTime;

		if (mTimeRemainingPerFrame < 0.0f)
		{
			mTimeRemainingPerFrame = kTimePerFrame;

			if (mAnimationDirectionIsRight)
			{
				mCurrentSpriteID++;

				if (mCurrentSpriteID > 3)
				{
					mCurrentSpriteID = 0;
					mDoingAnimation  = false;
				}
			}
			else
			{
				mCurrentSpriteID--;

				if (mCurrentSpriteID < 0)
				{
					mCurrentSpriteID = 0;
					mDoingAnimation  = false;
				}
			}
		}
	}

	return false;
}

// --------------------------------------------------------------------------------------- //

void PIPE_SMB1::SetIsDoingAnimation(bool animationGoingLeft)
{
	mDoingAnimation            = true;
	mAnimationDirectionIsRight = !animationGoingLeft;

	if (mAnimationDirectionIsRight)
	{
		mCurrentSpriteID = 0;
	}
	else
	{
		mCurrentSpriteID = 3;
	}
}

// --------------------------------------------------------------------------------------- //