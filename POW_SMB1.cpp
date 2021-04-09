#include "POW_SMB1.h"

#include "Texture2D.h"
#include "Constants.h"

#include <SDL.h>

// ----------------------------------------------------------------- //

POW::POW(SDL_Renderer* renderer, const char* filePathToSpriteSheet, Vector2D bottomLeftStartPos, Vector2D collisionBox)
	: RenderObject(0, 0, 0, 0.0f, bottomLeftStartPos, 3, 1, collisionBox, 0.0f)
	, mSpriteSheet(nullptr)

	, mStartPosition(bottomLeftStartPos)

	, mDoingBounce(false)
	, mGoingUp(true)

	, mCollisionBox(1.0f, 1.0f)
{
	mSourceRect = new SDL_Rect{ 0, 0,                                                               SPRITE_RES, SPRITE_RES };
	mDestRect   = new SDL_Rect{ (int)(mPosition.x * SPRITE_RES), (int)((mPosition.y - 1.0f) * SPRITE_RES), SPRITE_RES, SPRITE_RES };

	mSpriteSheet = new Texture2D(renderer);
	if (!mSpriteSheet->LoadFromFile(filePathToSpriteSheet))
	{
		std::cout << "Failed to load sprite sheet for pow block!" << std::endl;
	}
}

// ----------------------------------------------------------------- //

POW::~POW()
{
	delete mSpriteSheet;
	mSpriteSheet = nullptr;
}

// ----------------------------------------------------------------- //

bool POW::Update(const float deltaTime, LevelMap* levelMap)
{
	if (mDoingBounce)
	{
		if (mGoingUp)
		{
			mPosition.y -= 3.0 * deltaTime;

			if (mPosition.y < mStartPosition.y - 1.0f)
			{
				// Completed this section
				mGoingUp = false;
			}
		}
		else
		{
			mPosition.y += 3.0 * deltaTime;

			if (mPosition.y > mStartPosition.y)
			{
				// Reset the position
				mPosition.y = mStartPosition.y;

				// Completed this section
				mGoingUp = true;
				mDoingBounce = false;
			}
		}

		// Now update the dest rect so the pow block updates its render position
		mDestRect->y = (int)(mPosition.y - SPRITE_RES);
	}

	return false;
}

// ----------------------------------------------------------------- //

bool POW::SetHasBeenHit()
{
	if (mDoingBounce)
		return false;

	mDoingBounce = true;

	mCurrentSpriteID++;

	switch (mCurrentSpriteID)
	{
	case 1:
		mCollisionBox.y = 0.75f;
	break;

	case 2:
		mCollisionBox.y = 0.5f;
	break;

	case 3:
		return true;
	break;
	}

	return false;
}

// ----------------------------------------------------------------- //