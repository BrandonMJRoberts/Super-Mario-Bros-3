#include "Crab.h"

#include "Texture2D.h"

Texture2D*   Crab::mSpriteSheet = nullptr;
unsigned int Crab::mCrabCount = 0;

// ------------------------------------------------------ //

Crab::Crab(SDL_Renderer* renderer, const char* filePathToSpriteSheet, const float timePerFrame, Vector2D startPos) 
	: RenderObject(0, 0, 0, timePerFrame, startPos, 7, 6)
{
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

void Crab::UpdatePhysics()
{

}

// ------------------------------------------------------ //