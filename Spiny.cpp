#include "Spiny.h"

#include "Texture2D.h"

Texture2D*   Spiny::mSpriteSheet = nullptr;
unsigned int Spiny::mSpinyCount = 0;

// --------------------------------------------------------- //

Spiny::Spiny(SDL_Renderer* renderer, const char* filePathToSpriteSheet, const float timePerFrame, Vector2D startPos)
{
	// Load in the sprite sheet
	if (mSpinyCount == 0)
	{
		mSpriteSheet = new Texture2D(renderer);
		if (!mSpriteSheet->LoadFromFile(filePathToSpriteSheet))
		{
			std::cout << "Failed to load sprite sheet for coins" << std::endl;
		}
	}

	mSpinyCount++;

	SetupRenderRects();
}

// --------------------------------------------------------- //

Spiny::~Spiny()
{
	if (mSpinyCount == 1)
	{
		delete mSpriteSheet;
		mSpriteSheet = nullptr;
	}

	mSpinyCount--;
}

// --------------------------------------------------------- //

void Spiny::UpdatePhysics()
{

}

// --------------------------------------------------------- //