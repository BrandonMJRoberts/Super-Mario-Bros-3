#include "FighterFly.h"

#include "Texture2D.h"

Texture2D*   FighterFly::mSpriteSheet      = nullptr;
unsigned int FighterFly::mFighterFlyCount  = 0;

// --------------------------------------------------------- //

FighterFly::FighterFly(SDL_Renderer* renderer, const char* filePathToSpriteSheet, const float timePerFrame, Vector2D startPos)
{
	// Load in the sprite sheet
	if (mFighterFlyCount == 0)
	{
		mSpriteSheet = new Texture2D(renderer);
		if (!mSpriteSheet->LoadFromFile(filePathToSpriteSheet))
		{
			std::cout << "Failed to load sprite sheet for coins" << std::endl;
		}
	}

	mFighterFlyCount++;

	SetupRenderRects();
}

// --------------------------------------------------------- //

FighterFly::~FighterFly()
{
	if (mFighterFlyCount == 1)
	{
		delete mSpriteSheet;
		mSpriteSheet = nullptr;
	}

	mFighterFlyCount--;
}

// --------------------------------------------------------- //

void FighterFly::UpdatePhysics()
{

}

// --------------------------------------------------------- //