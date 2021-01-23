#include "PlayableCharacter.h"

#include "Constants_SMB3.h"

// ----------------------------------------------------- //

PlayableCharacter::PlayableCharacter(SDL_Renderer* renderer, const char* filePathToSpriteSheet, Vector2D spawnPoint, Vector2D numberOfSpritesOnDimensions, const Vector2D levelBounds)
: Subject()
, mRealGridPosition(spawnPoint)
, mScreenGridPosition(0, 0)
, mRenderRefencePoint(spawnPoint)

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

	// Now calculate where the starting screen space position the player should be
	
	// If the player is near a boundary then they need specific positioning - starting with the Y axis
	if (spawnPoint.y + (PLAYABLE_SCREEN_AREA_HEIGHT / 2) > levelBounds.y)
	{
		mScreenGridPosition.y = (BACKGROUND_SPRITE_RENDER_HEIGHT - 1) - (levelBounds.y - spawnPoint.y);
	}
	else if (spawnPoint.y - (PLAYABLE_SCREEN_AREA_HEIGHT / 2) < 0.0f)
	{
		mScreenGridPosition.y = spawnPoint.y;
	}
	else
	{
		mScreenGridPosition.y = LEVEL_BOUNDING_AREA_HEIGHT / 2;
	}

	// Now for the X axis
	if (spawnPoint.x + (PLAYABLE_SCREEN_AREA_WIDTH / 2) > levelBounds.x)
	{
		mScreenGridPosition.x = levelBounds.x - spawnPoint.x;
	}
	else if (spawnPoint.x - (PLAYABLE_SCREEN_AREA_WIDTH / 2) < 0.0f)
	{
		mScreenGridPosition.x = spawnPoint.x;
	}
	else
	{
		mScreenGridPosition.x = LEVEL_BOUNDING_AREA_WIDTH / 2;
	}

	// Now calculate the starting render reference point
	CalculateInitialRenderReferencePoint(levelBounds);
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
			               int(mScreenGridPosition.y * RESOLUTION_OF_SPRITES) - RESOLUTION_OF_SPRITES,
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

	CalculateNewPosition(levelBounds, deltaTime);

}

// ----------------------------------------------------- //

void PlayableCharacter::CalculateNewPosition(const Vector2D levelBounds, const float deltaTime)
{
	// Calculate the new potential positions
	Vector2D movementDistance = (mVelocity * deltaTime);

	Vector2D newRealGridPos   = mRealGridPosition   + movementDistance;
	Vector2D newScreenGridPos = mScreenGridPosition + movementDistance;

	// First cap the new positions to their relative bounds - starting with the y axis
	if (newScreenGridPos.y <= 0.0f)
		newScreenGridPos.y = 0.0f;
	else if (newScreenGridPos.y >= PLAYABLE_SCREEN_AREA_HEIGHT + 2)
		newScreenGridPos.y = PLAYABLE_SCREEN_AREA_HEIGHT + 2;

	if (newRealGridPos.y <= 0.0f)
		newRealGridPos.y = 0.0f;
	else if (newRealGridPos.y >= levelBounds.y)
		newRealGridPos.y = levelBounds.y;

	// Now for the x axis
	if (newScreenGridPos.x <= 0.0f)
		newScreenGridPos.x = 0.0f;
	else if (newScreenGridPos.x >= (PLAYABLE_SCREEN_AREA_WIDTH - 1))
		newScreenGridPos.x = (PLAYABLE_SCREEN_AREA_HEIGHT - 1);

	if (newRealGridPos.x <= 0.0f)
		newRealGridPos.x = 0.0f;
	else if (newRealGridPos.x >= levelBounds.x)
		newRealGridPos.x = levelBounds.x;

	// First update the real grid position of the player so that when the player moves their actual point in the world moves as well
	mRealGridPosition = newRealGridPos;

	// If we want to scroll the screen then scroll - X axis
	if (newScreenGridPos.x > (PLAYABLE_SCREEN_AREA_WIDTH  - (PLAYABLE_SCREEN_AREA_WIDTH / 2.0f)) + 1 && movementDistance.x > 0.0f)
	{
		// If moving in the correct direction then scroll the screen
		mRenderRefencePoint.x += movementDistance.x;
	}
	else if (newScreenGridPos.x < ((PLAYABLE_SCREEN_AREA_WIDTH / 2.0f) - 1) && movementDistance.x < 0.0f)
	{
		mRenderRefencePoint.x += movementDistance.x;
	}
	else // If we are within this leway area then don't scroll the screen
	{
		// Allow the player to move freely
		mScreenGridPosition.x = newScreenGridPos.x;
	}

	if (mRealGridPosition.x + (PLAYABLE_SCREEN_AREA_WIDTH / 2) - 1 > levelBounds.x || 
		mRealGridPosition.x - (PLAYABLE_SCREEN_AREA_WIDTH / 2) + 1 < 0.0f)
	{
		mScreenGridPosition.x = newScreenGridPos.x;
	}

	// If we want to scroll the screen then scroll - Y axis
	if (newScreenGridPos.y > PLAYABLE_SCREEN_AREA_HEIGHT - (PLAYABLE_SCREEN_AREA_HEIGHT / 2.0f) + 2 && movementDistance.y > 0.0f)
	{
		mRenderRefencePoint.y += movementDistance.y;
	}
	else if (newScreenGridPos.y < (PLAYABLE_SCREEN_AREA_HEIGHT / 2.0f) && movementDistance.y < 0.0f)
	{
		mRenderRefencePoint.y += movementDistance.y;
	}
	else
	{
		// Allow the player to move freely
		mScreenGridPosition.y = newScreenGridPos.y;
	}

	if (mRealGridPosition.y + (PLAYABLE_SCREEN_AREA_HEIGHT / 2.0f) > levelBounds.y || 
		mRealGridPosition.y - (PLAYABLE_SCREEN_AREA_HEIGHT / 2.0f) < 0.0f)
	{
		mScreenGridPosition.y = newScreenGridPos.y;
	}

	// Cap the render reference point
	if (mRenderRefencePoint.x < 0.0f)
	{
		mRenderRefencePoint.x = 0.0f;
	}
	else if (mRenderRefencePoint.x > levelBounds.x - (PLAYABLE_SCREEN_AREA_WIDTH / 2))
	{
		mRenderRefencePoint.x = levelBounds.x - (PLAYABLE_SCREEN_AREA_WIDTH / 2);
	}

	if (mRenderRefencePoint.y < 0.0f)
	{
		mRenderRefencePoint.y = 0.0f;
	}
	else if (mRenderRefencePoint.y > levelBounds.y - BACKGROUND_SPRITE_RENDER_HEIGHT + 1)
	{
		mRenderRefencePoint.y = levelBounds.y - BACKGROUND_SPRITE_RENDER_HEIGHT + 1;
	}

	//std::cout << "Y Pos: " << mRealGridPosition.y << "\tScreen Y: " << mScreenGridPosition.y  << std::endl;
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

void PlayableCharacter::CalculateInitialRenderReferencePoint(const Vector2D levelBounds)
{
	// Starting with the Y axis
	if (mRealGridPosition.y - (PLAYABLE_SCREEN_AREA_HEIGHT / 2) <= 0.0f)
	{
		// Set the correct reference point
		mRenderRefencePoint.y = 0.0f;
	}
	else if (mRealGridPosition.y + ((PLAYABLE_SCREEN_AREA_HEIGHT / 2) + 3) >= levelBounds.y)
	{
		// Set the correct reference point
		mRenderRefencePoint.y = levelBounds.y - (BACKGROUND_SPRITE_RENDER_HEIGHT - 1);
	}

	// Now for the X-axis
	if (mRealGridPosition.x - (PLAYABLE_SCREEN_AREA_WIDTH / 2) < 0.0f)
	{
		mRenderRefencePoint.x = 0.0f;
	}
	else if (mRealGridPosition.x + (PLAYABLE_SCREEN_AREA_WIDTH / 2) >= levelBounds.x)
	{
		mRenderRefencePoint.x = levelBounds.x - PLAYABLE_SCREEN_AREA_WIDTH;
	}
}

// ----------------------------------------------------- //