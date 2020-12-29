#include "PlayableCharacter.h"

#include "Constants_SMB3.h"

// ----------------------------------------------------- //

PlayableCharacter::PlayableCharacter(SDL_Renderer* renderer, const char* filePathToSpriteSheet, Vector2D spawnPoint, Vector2D numberOfSpritesOnDimensions)
: mRealGridPosition(spawnPoint)
, mScreenGridPosition(spawnPoint)
, mVelocity(0, 0)
, mAcceleration(0, 0)
, mIsAlive(true)
, mSpriteSheet(nullptr)
, mSingleSpriteWidth(0)
, mSingleSpriteHeight(0)
, mCurrentFrame(0)
, mStartFrame(0)
, mEndFrame(0)
, mNumberOfSpritesOnWidth((unsigned int)numberOfSpritesOnDimensions.x)
{
	// Load in the sprite sheet passed in
	mSpriteSheet = new Texture2D(renderer);
	if (!mSpriteSheet->LoadFromFile(filePathToSpriteSheet))
	{
		std::cout << "Failed to load in the sprite sheet for the level character." << std::endl;
		mSpriteSheet = nullptr;
		return;
	}

	if (numberOfSpritesOnDimensions.x > 0 && numberOfSpritesOnDimensions.y > 0)
	{
		mSingleSpriteWidth  = mSpriteSheet->GetWidth()  / (unsigned int)numberOfSpritesOnDimensions.x;
		mSingleSpriteHeight = mSpriteSheet->GetHeight() / (unsigned int)numberOfSpritesOnDimensions.y;
	}
}

// ----------------------------------------------------- //

PlayableCharacter::~PlayableCharacter()
{
	delete mSpriteSheet;
	mSpriteSheet = nullptr;
}

// ----------------------------------------------------- //

void PlayableCharacter::Render()
{
	if (mSpriteSheet)
	{
		// First calculate what we should render - which sprite
		SDL_Rect portionOfSpriteSheet {int((mCurrentFrame % mNumberOfSpritesOnWidth) * mSingleSpriteWidth),
									   int((mCurrentFrame / mNumberOfSpritesOnWidth)* mSingleSpriteHeight),
			                           int(mSingleSpriteWidth), 
			                           int(mSingleSpriteHeight) };

		// Now calculate where we should render it
		SDL_Rect destRect {int(mScreenGridPosition.x * RESOLUTION_OF_SPRITES), 
			               int(mScreenGridPosition.y * RESOLUTION_OF_SPRITES),
			               int(mSingleSpriteWidth), 
			               int(mSingleSpriteHeight) };

		// Render it
		mSpriteSheet->Render(portionOfSpriteSheet, destRect);
	}
	else
		std::cout << "Invalid sprite sheet trying to be rendered from: Base Character" << std::endl;
}

// ----------------------------------------------------- //

void PlayableCharacter::Update(const float deltaTime, SDL_Event e)
{
	// First handle input to see if the player wants to move in a direction
	HandleMovementInput(e);

	// Now apply any movement that the player is currently doing to their position
	mRealGridPosition += (mVelocity * deltaTime);

	// And if the player is in a part of the screen in which they can move then move them
	if (mScreenGridPosition.x < (LEVEL_BOUNDING_AREA_WIDTH / 2.0f) && mRealGridPosition.x > 0.0f)
	{
		mScreenGridPosition.x += (mVelocity.x * deltaTime);
	}

	if (mScreenGridPosition.y < LEVEL_BOUNDING_AREA_HEIGHT && mRealGridPosition.y > 0.0f)
	{
		mScreenGridPosition.y += (mVelocity.y * deltaTime);
	}
}

// ----------------------------------------------------- //

void PlayableCharacter::HandleMovementInput(SDL_Event e)
{
	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case SDLK_d:
			mVelocity.x = 1.0f;
		break;

		case SDLK_a:
			mVelocity.x = -1.0f;
		break;

		case SDLK_s:
			mVelocity.y = 1.0f;
		break;

		case SDLK_w:
			mVelocity.y = -1.0f;
		break;
		}
	break;

	case SDL_KEYUP:
		switch (e.key.keysym.sym)
		{
		case SDLK_d:
		case SDLK_a:
			mVelocity.x = 0.0f;
		break;

		case SDLK_s:
		case SDLK_w:
			mVelocity.y = 0.0f;
		break;
		}
		break;
	}
}

// ----------------------------------------------------- //