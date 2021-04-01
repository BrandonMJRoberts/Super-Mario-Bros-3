#include "Freezie.h"

#include "Texture2D.h"

Texture2D*   Freezie::mSpriteSheet  = nullptr;
unsigned int Freezie::mFreezieCount = 0;

// --------------------------------------------------------- //

Freezie::Freezie(SDL_Renderer* renderer, const char* filePathToSpriteSheet, const float timePerFrame, Vector2D startPos)
{
	// Load in the sprite sheet
	if (mFreezieCount == 0)
	{
		mSpriteSheet = new Texture2D(renderer);
		if (!mSpriteSheet->LoadFromFile(filePathToSpriteSheet))
		{
			std::cout << "Failed to load sprite sheet for coins" << std::endl;
		}
	}

	mFreezieCount++;

	SetupRenderRects();
}

// --------------------------------------------------------- //

Freezie::~Freezie()
{
	if (mFreezieCount == 1)
	{
		delete mSpriteSheet;
		mSpriteSheet = nullptr;
	}

	mFreezieCount--;
}

// --------------------------------------------------------- //

void Freezie::UpdatePhysics()
{

}

// --------------------------------------------------------- //