#include "Coin.h"

#include "Texture2D.h"

#include <SDL.h>

#include "LevelMap.h"

unsigned int Coin::mCoinCount = 0;
Texture2D*   Coin::mSpriteSheet = nullptr;

// -------------------------------------------------------------------------- //

Coin::Coin(SDL_Renderer* renderer, const Vector2D startPosition, const char* filePathToSpriteSheet, const float timePerFrame, Vector2D collisionBox) 
	: RenderObject(0, 4, 0, timePerFrame, startPosition, 8, 1, collisionBox) 
{
	// Load in the sprite sheet
	if (mCoinCount == 0)
	{
		mSpriteSheet = new Texture2D(renderer);
		if (!mSpriteSheet->LoadFromFile(filePathToSpriteSheet))
		{
			std::cout << "Failed to load sprite sheet for coins" << std::endl;
		}
	}

	mCoinCount++;

	SetupRenderRects();
}

// -------------------------------------------------------------------------- //

Coin::~Coin()
{
	if (mCoinCount == 1)
	{
		delete mSpriteSheet;
		mSpriteSheet = nullptr;
	}

	mCoinCount--;
}

// -------------------------------------------------------------------------- //

void Coin::UpdatePhysics(const float deltaTime, LevelMap* levelMap)
{
	Vector2D movementDistance = mVelocity * deltaTime;

	// Check for collisions
	if (   levelMap->GetCollisionTileAt(int(mPosition.y + movementDistance.y), int(mPosition.x)) == '1'
		|| levelMap->GetCollisionTileAt(int(mPosition.y + movementDistance.y), int(mPosition.x + mCollisionBox.x)) == '1') // Down collision
	{
		mVelocity.y = 0.0f;
	}
	else if (levelMap->GetCollisionTileAt(int(mPosition.y - mCollisionBox.y - movementDistance.y), int(mPosition.x)) == '1' ||
		levelMap->GetCollisionTileAt(int(mPosition.y - mCollisionBox.y - movementDistance.y), int(mPosition.x + mCollisionBox.x)) == '1') // Up collision
	{
		// Up collision bounce back down
		mVelocity.y = 1.0f;
	}
	else
	{
		mVelocity.y += CHARACTER_GRAVITY * deltaTime;

		// No y collisions
		mPosition.y += movementDistance.y;
	}
}

// -------------------------------------------------------------------------- //