#include "Coin.h"

#include "Texture2D.h"

#include <SDL.h>

unsigned int Coin::mCoinCount = 0;
Texture2D*   Coin::mSpriteSheet = nullptr;

// -------------------------------------------------------------------------- //

Coin::Coin(SDL_Renderer* renderer, const Vector2D startPosition, const char* filePathToSpriteSheet, const float timePerFrame) 
	: RenderObject(0, 4, 0, timePerFrame, startPosition, 8, 1) 
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

void Coin::Update(const float deltaTime)
{

}

// -------------------------------------------------------------------------- //