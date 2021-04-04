#include "Crab.h"

#include "Texture2D.h"
#include "LevelMap.h"

Texture2D*   Crab::mSpriteSheet = nullptr;
unsigned int Crab::mCrabCount = 0;

// ------------------------------------------------------ //

Crab::Crab(SDL_Renderer* renderer, const char* filePathToSpriteSheet, const float timePerFrame, Vector2D startPos, Vector2D collisionBox, bool spawningOnLeftSide, const float movementSpeed) 
	: RenderObject(0, 2, 0, timePerFrame, startPos, 7, 6, collisionBox, movementSpeed)
{
	mFacingLeft = !spawningOnLeftSide;

	// Load in the sprite sheet
	if (mCrabCount == 0)
	{
		mSpriteSheet = new Texture2D(renderer);
		if (!mSpriteSheet->LoadFromFile(filePathToSpriteSheet))
		{
			std::cout << "Failed to load sprite sheet for coins" << std::endl;
		}
	}

	mCrabCount++;

	SetupRenderRects();
}

// ------------------------------------------------------ //

Crab::~Crab() 
{
	if (mCrabCount == 1)
	{
		delete mSpriteSheet;
		mSpriteSheet = nullptr;
	}

	mCrabCount--;
}

// ------------------------------------------------------ //