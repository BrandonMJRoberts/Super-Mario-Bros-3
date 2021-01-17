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
	Vector2D newRealGridPos   = mRealGridPosition   + (mVelocity * deltaTime);
	Vector2D newScreenGridPos = mScreenGridPosition + (mVelocity * deltaTime);

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

	std::cout << "Grid Pos Y: " << mRealGridPosition.y << std::endl;

	// Starting with the Y axis
	if (mRealGridPosition.y - (PLAYABLE_SCREEN_AREA_HEIGHT / 2) <= 0.0f)
	{
		// Set the correct reference point
		mRenderRefencePoint.y = 0.0f;

		// We also need to update mario's screen position here
		mScreenGridPosition.y = newScreenGridPos.y;
	}
	else if (mRealGridPosition.y + ((PLAYABLE_SCREEN_AREA_HEIGHT / 2) + 3) >= levelBounds.y)
	{
		// Set the correct reference point
		mRenderRefencePoint.y = levelBounds.y - (BACKGROUND_SPRITE_RENDER_HEIGHT - 1);

		// We also need to update mario's screen position here
		mScreenGridPosition.y = newScreenGridPos.y;
	}
	else
	{
		// If not near a y-axis boundary then keep mario in the centre of the y axis
		mRenderRefencePoint.y = mRealGridPosition.y - (PLAYABLE_SCREEN_AREA_HEIGHT / 2);
	}


	// Now for the X-axis
	if (mRealGridPosition.x - (PLAYABLE_SCREEN_AREA_WIDTH / 2) <= 0.0f)
	{
		mRenderRefencePoint.x = 0.0f;

		mScreenGridPosition.x = newScreenGridPos.x;
	}
	else if(mRealGridPosition.x + (PLAYABLE_SCREEN_AREA_WIDTH / 2) >= levelBounds.x)
	{
		mRenderRefencePoint.x = levelBounds.x - PLAYABLE_SCREEN_AREA_WIDTH;

		mScreenGridPosition.x = newScreenGridPos.x;
	}
	else
	{
		mRenderRefencePoint.x = newRealGridPos.x - (PLAYABLE_SCREEN_AREA_WIDTH / 2);
	}
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