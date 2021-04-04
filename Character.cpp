#include "Character.h"

#include "Constants.h"
#include "Texture2D.h"

#include "LevelMap.h"

// --------------------------------------------------------------------------------------------------------- //

Character::Character(SDL_Renderer* renderer, std::string imagePath, Vector2D startingPosition, const unsigned int spritesOnWidth, const unsigned int spritesOnHeight, LevelMap* levelMap, const float collisionCircleRadius)
	: mJumpForce(CHARACTER_INITIAL_JUMP_FORCE)
	, mPlayerMovementData(0)
	, mVelocity(0, 0)
	, mPosition(startingPosition)

	, mRenderer(renderer)

	, kSpritesOnWidth(spritesOnWidth)
	, kSpritesOnHeight(spritesOnHeight)

	, mLevelMap(levelMap)

	, mCollisionRadius(collisionCircleRadius)
	, mCollisionBox(0.0f, 0.0f)

	, mUsingCollisionBox(false)
{
	CalculateSpriteData(renderer, imagePath);
}

// --------------------------------------------------------------------------------------------------------- //

Character::Character(SDL_Renderer* renderer, std::string imagePath, Vector2D startingPosition, const unsigned int spritesOnWidth, const unsigned int spritesOnHeight, LevelMap* levelMap, const Vector2D collisionBox)
	: mJumpForce(CHARACTER_INITIAL_JUMP_FORCE)

	, mCollisionBox(collisionBox)
	, mCollisionRadius(0.0f)

	, mRenderer(renderer)

	, mVelocity(0, 0)
	, mPosition(startingPosition)
	, mPlayerMovementData(0)

	, kSpritesOnWidth(spritesOnWidth)
	, kSpritesOnHeight(spritesOnHeight)

	, mLevelMap(levelMap)

	, mUsingCollisionBox(true)

	, mCurrentSpriteID(3)
	, mEndSpriteID(3)
	, mStartSpriteID(3)
{
	CalculateSpriteData(renderer, imagePath);
}

// --------------------------------------------------------------------------------------------------------- //

Character::~Character()
{
	delete mTexture;
	mTexture = nullptr;

	mRenderer = nullptr;

	mLevelMap = nullptr;
}

// --------------------------------------------------------------------------------------------------------- //

void Character::Render()
{
	if (mVelocity.x > 0.0f)
	{
		mTexture->Render(*mSourceRect, *mDestRect, SDL_FLIP_HORIZONTAL, 0.0f);
	}
	else if (mVelocity.x < 0.0f)
	{
		mTexture->Render(*mSourceRect, *mDestRect, SDL_FLIP_NONE, 0.0f);
	}
	else
	{
		if (mPlayerMovementData & PlayerMovementData::WAS_FACING_RIGHT)
		{
			mTexture->Render(*mSourceRect, *mDestRect, SDL_FLIP_HORIZONTAL, 0.0f);
		}
		else
			mTexture->Render(*mSourceRect, *mDestRect, SDL_FLIP_NONE, 0.0f);
	}

}

// --------------------------------------------------------------------------------------------------------- //

void Character::HandleInput(SDL_Event e)
{
	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case SDLK_d:
			// Set that the player is now jumping
			if (!(mPlayerMovementData & PlayerMovementData::WALKING_LEFT_SMB1))
			{
				mPlayerMovementData |= PlayerMovementData::WALKING_RIGHT_SMB1;
				mPlayerMovementData |= PlayerMovementData::WAS_FACING_RIGHT;

				// Apply the movement force
				mVelocity.x = 3.0f;
			}
		break;

		case SDLK_a:
			if (!(mPlayerMovementData & PlayerMovementData::WALKING_RIGHT_SMB1))
			{
				mPlayerMovementData |= PlayerMovementData::WALKING_LEFT_SMB1;

				mPlayerMovementData &= ~(PlayerMovementData::WAS_FACING_RIGHT);

				// Apply the movement force
				mVelocity.x = -3.0f;
			}
		break;

		case SDLK_w:
			if(!(mPlayerMovementData & PlayerMovementData::JUMPING_SMB1))
				Jump();
		break;
		}
	break;

	case SDL_KEYUP:
		switch (e.key.keysym.sym)
		{
		case SDLK_d:

			if (!(mPlayerMovementData & PlayerMovementData::WALKING_LEFT_SMB1))
			{
				// Set that the player is now jumping
				mPlayerMovementData &= ~(PlayerMovementData::WALKING_RIGHT_SMB1);

				// Apply the jump force
				mVelocity.x = 0.0f;
			}
		break;

		case SDLK_a:
			if (!(mPlayerMovementData & PlayerMovementData::WALKING_RIGHT_SMB1))
			{
				mPlayerMovementData &= ~(PlayerMovementData::WALKING_LEFT_SMB1);

				// Apply the jump force
				mVelocity.x = 0.0f;
			}
		break;
		}
	break;
	}
}

// --------------------------------------------------------------------------------------------------------- //

void Character::ApplyMovement(const float deltaTime)
{
	// Move the character in the direction they are going
	mPosition += (mVelocity * deltaTime);

	// Screen capping
	if (mPosition.y + mCollisionBox.y > SCREEN_HEIGHT_GRID)
	{
		mPosition.y = SCREEN_HEIGHT_GRID - mCollisionBox.y;
	}
	else if (mPosition.y < 0.0f)
	{
		mPosition.y = 0.0f;
	}
}

// --------------------------------------------------------------------------------------------------------- //

void Character::Update(float deltaTime, SDL_Event e)
{
	// Handle if the player has pressed any input
	HandleInput(e);

	// handle if the player has made any collisions
	HandleCollisions(deltaTime);

	// Apply the players movement based off of their current velocity
	ApplyMovement(deltaTime);

	// Now apply the change in movement to the dest rect
	mDestRect->x =  mPosition.x * SPRITE_RES;
	mDestRect->y =  mPosition.y * SPRITE_RES;

	std::cout << mPosition.y << std::endl;
}

// --------------------------------------------------------------------------------------------------------- //

void Character::HandleCollisions(const float deltaTime)
{
	// Calculations have the top left being the collision point and y increase going 

	// First apply gravity
	mVelocity.y += CHARACTER_GRAVITY * deltaTime;

	Vector2D movementDistance = mVelocity * deltaTime;

	if (mUsingCollisionBox)
	{
		// Going to the right
		if (mVelocity.x > 0.0f)
		{
			if (mLevelMap->GetCollisionTileAt((unsigned int)(mPosition.x + movementDistance.x + mCollisionBox.x), (unsigned int)mPosition.y) != 0)
			{

			}
		}
		else // Going to the left
		{

		}

		// Going down
		if (mVelocity.y >= 0.0f)
		{
			// Check for a collision
			if (   mLevelMap->GetCollisionTileAt((unsigned int)(mPosition.y + mCollisionBox.y + movementDistance.y), (unsigned int)(mPosition.x + mCollisionBox.x)) != 0  // Right foot check
				|| mLevelMap->GetCollisionTileAt((unsigned int)(mPosition.y + mCollisionBox.y + movementDistance.y), (unsigned int)(mPosition.x)) != 0) // Left foot check
			{
				mVelocity.y = 0.0f;
			}
		}
		else // Going up
		{
			// Check for a collision
			if (   mLevelMap->GetCollisionTileAt((unsigned int)(mPosition.x + mCollisionBox.x), (unsigned int)(mPosition.y + movementDistance.y)) != 0  // Right head check
				|| mLevelMap->GetCollisionTileAt((unsigned int)(mPosition.x),                   (unsigned int)(mPosition.y + movementDistance.y)) != 0) // Left head check
			{
				// Give a bounce back down effect
				//mVelocity.y = 0.0f;
			}
		}
	}
	else // Do calculations based off of the collision circle instead of the collision box
	{
		
		// Going to the right
		if (mVelocity.x > 0.0f)
		{

		}
		else // Going to the left
		{

		}

		// Going up
		if (mVelocity.y > 0.0f)
		{

		}
		else // Going down
		{

		}
	}
}

// --------------------------------------------------------------------------------------------------------- //

void Character::SetPosition(Vector2D position)
{
	mPosition = position;
}

// --------------------------------------------------------------------------------------------------------- //

void Character::Jump()
{
	// Set that the player is now jumping
	mPlayerMovementData |= PlayerMovementData::JUMPING_SMB1;

	// Apply the jump force
	mVelocity.y          = mJumpForce;
}

// --------------------------------------------------------------------------------------------------------- //

void Character::CalculateSpriteData(SDL_Renderer* renderer, std::string filePath)
{
	mTexture = new Texture2D(renderer);
	if (!mTexture->LoadFromFile(filePath.c_str()))
	{
		std::cout << "Failed to load the character sprite." << std::endl;
		return;
	}

	int spriteWidth = mTexture->GetWidth() / kSpritesOnWidth;
	int spriteHeight = mTexture->GetHeight() / kSpritesOnHeight;

	mSourceRect = new SDL_Rect{ int(spriteWidth * (mCurrentSpriteID % kSpritesOnWidth)),
									  int(spriteHeight * int(mCurrentSpriteID / kSpritesOnWidth)),
									  spriteWidth,
									  spriteHeight };

	mDestRect = new SDL_Rect{ int(mPosition.x * SPRITE_RES),
									  (int)(mPosition.y * SPRITE_RES),
									   spriteWidth,
									   spriteHeight };
}

// --------------------------------------------------------------------------------------------------------- //