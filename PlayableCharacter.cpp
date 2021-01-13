#include "PlayableCharacter.h"

#include "Constants_SMB3.h"

// ----------------------------------------------------- //

PlayableCharacter::PlayableCharacter(SDL_Renderer* renderer, const char* filePathToSpriteSheet, Vector2D spawnPoint, Vector2D numberOfSpritesOnDimensions)
: Subject()
, mRealGridPosition(spawnPoint)
, mScreenGridPosition()
, mRenderRefencePoint(0, 0)

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

	// Now calculate where the player's screen position should be

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

void PlayableCharacter::Update(const float deltaTime, SDL_Event e, const Vector2D levelBounds)
{
	// First handle input to see if the player wants to move in a direction
	HandleMovementInput(e);

	// Checks so that the scrolling doesnt ever go off the screen into nothing
	CapRealPositionToPlayableArea(deltaTime, levelBounds);

	// Check to allow for the player to move around the screen
	CapScreenPositionToInteractableArea(deltaTime);

	// Now calculate where the refenece point should be - based slightly off of the player's real postion 
	CalculateRenderReferencePoint(levelBounds);
}

// ----------------------------------------------------- //

void PlayableCharacter::CalculateRenderReferencePoint(const Vector2D levelBounds)
{
	// First handle the Y
	if (mRealGridPosition.y + (HALF_SCREEN_HEIGHT_GRID_SMB3 + 3) >= levelBounds.y)
	{
		mRenderRefencePoint.y = levelBounds.y - (HALF_SCREEN_HEIGHT_GRID_SMB3 + 3);
	}
	else
		mRenderRefencePoint.y = mRealGridPosition.y;

	// Now handle the X
	if (mRealGridPosition.x + HALF_SCREEN_WIDTH_GRID_SMB3 > levelBounds.x)
	{
		mRenderRefencePoint.x = levelBounds.x - HALF_SCREEN_WIDTH_GRID_SMB3;
	}
	else
		mRenderRefencePoint.x = mRealGridPosition.x;
}

// ----------------------------------------------------- //

void PlayableCharacter::CapScreenPositionToInteractableArea(const float deltaTime)
{
	Vector2D newScreenGridPos = mScreenGridPosition + (mVelocity * deltaTime);

	// First do the y checks
	if (newScreenGridPos.y <= 0.0f)
		mScreenGridPosition.y = 0.0f;
	else if (newScreenGridPos.y >= double(SCREEN_HEIGHT_GRID_SMB3) - 1.0f)
		mScreenGridPosition.y = double(SCREEN_HEIGHT_GRID_SMB3) - 1.0f;
	else
		mScreenGridPosition.y = newScreenGridPos.y;

	// Now do the X checks
	if (newScreenGridPos.x <= 0.0f)
		mScreenGridPosition.x = 0.0f;
	else if (newScreenGridPos.x >= double(LEVEL_BOUNDING_AREA_WIDTH / 2.0f) - 1.0f)
		mScreenGridPosition.x = double(LEVEL_BOUNDING_AREA_WIDTH / 2.0f) - 1.0f;
	else
		mScreenGridPosition.x = newScreenGridPos.x;
}

// ----------------------------------------------------- //

void PlayableCharacter::CapRealPositionToPlayableArea(const float deltaTime, const Vector2D levelBounds)
{
	Vector2D newRealGridPos = mRealGridPosition + (mVelocity * deltaTime);

	// First do the y checks
	if (newRealGridPos.y <= 0.0f)
		mRealGridPosition.y = 0.0f;
	else if (newRealGridPos.y >= levelBounds.y)
		mRealGridPosition.y = levelBounds.y;
	else
		mRealGridPosition.y = newRealGridPos.y;

	// Now do the x checks
	if (newRealGridPos.x <= 0.0f)
		mRealGridPosition.x = 0.0f;
	else if (newRealGridPos.x >= levelBounds.x)
		mRealGridPosition.x = levelBounds.x;
	else
		mRealGridPosition.x = newRealGridPos.x;
}

// ----------------------------------------------------- //

void PlayableCharacter::HandleMovementInput(SDL_Event e)
{
	float speed = 4.0f;

	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case SDLK_d:
			mVelocity.x = speed;
		break;

		case SDLK_a:
			mVelocity.x = -speed;
		break;

		case SDLK_s:
			mVelocity.y = speed;
		break;

		case SDLK_w:
			mVelocity.y = -speed;
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