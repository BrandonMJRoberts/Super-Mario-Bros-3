#include "Coin.h"

#include "Texture2D.h"

#include <SDL.h>

#include "LevelMap.h"

unsigned int Coin::mCoinCount = 0;
Texture2D*   Coin::mSpriteSheet = nullptr;

// -------------------------------------------------------------------------- //

Coin::Coin(SDL_Renderer* renderer, const Vector2D startPosition, const char* filePathToSpriteSheet, const float timePerFrame, Vector2D collisionBox) 
	: RenderObject(0, 4, 0, timePerFrame, startPosition, 8, 1, collisionBox, 0.0f) 
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
