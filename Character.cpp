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

	, mCanJump(false)
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

	, mCanJump(false)
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

	// Now apply the change in movement to the dest rect
	mDestRect->x = mPosition.x * SPRITE_RES;
	mDestRect->y = (mPosition.y - mCollisionBox.y) * SPRITE_RES;
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

	CheckForLooping();
}

// --------------------------------------------------------------------------------------------------------- //

void Character::HandleCollisions(const float deltaTime)
{
	Vector2D movementDistance = mVelocity * deltaTime;

	// Check for collisions
	if (   mLevelMap->GetCollisionTileAt(int(mPosition.y + movementDistance.y), int(mPosition.x))                   == 1 // left check
		|| mLevelMap->GetCollisionTileAt(int(mPosition.y + movementDistance.y), int(mPosition.x + mCollisionBox.x)) == 1) // right check
	{
		mPosition.y = (int)(mPosition.y + movementDistance.y) - 0.001;

		mVelocity.y = 0.0f;
		mCanJump    = true;
	}
	else if (mLevelMap->GetCollisionTileAt(int(mPosition.y + movementDistance.y), int(mPosition.x)) == 1 ||  // left check
		     mLevelMap->GetCollisionTileAt(int(mPosition.y + movementDistance.y), int(mPosition.x + mCollisionBox.x)) == 1) // right check
	{
		// Up collision bounce back down
		mVelocity.y = 1.0f;
	}
	else
	{
		mVelocity.y += CHARACTER_GRAVITY * deltaTime;
		mPosition.y += movementDistance.y;
		mCanJump     = false;
	}

	// Now for x checks
	if (   mLevelMap->GetCollisionTileAt(int(mPosition.y - mCollisionBox.y), int(mPosition.x)) == 1  // top left check
		|| mLevelMap->GetCollisionTileAt(int(mPosition.y),                   int(mPosition.x)) == 1) // bottom left check
	{
		mVelocity.x = 0.0f;
	}
	else if (mLevelMap->GetCollisionTileAt(int(mPosition.y - mCollisionBox.y), int(mPosition.x + mCollisionBox.x)) == 1 ||  // top right check
		     mLevelMap->GetCollisionTileAt(int(mPosition.y),                   int(mPosition.x + mCollisionBox.x)) == 1) // bottom right check
	{
		// Up collision bounce back down
		mVelocity.x = 0.0f;
	}
	else
	{
		// No y collisions
		mPosition.x += movementDistance.x;
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
									  (int)((mPosition.y - mCollisionBox.y) * SPRITE_RES),
									   spriteWidth,
									   spriteHeight };
}

// --------------------------------------------------------------------------------------------------------- //

void Character::CheckForLooping()
{
	if (!mLevelMap)
		return;

	if (mPosition.x > mLevelMap->GetLevelWidth() && mVelocity.x > 0.0f)
	{
		mPosition.x = -(mCollisionBox.x) + 0.01;
	}
	else if (mPosition.x + mCollisionBox.x < 0.0f && mVelocity.x < 0.0f)
	{
		mPosition.x = mLevelMap->GetLevelWidth() - 0.01;
	}
}

// --------------------------------------------------------------------------------------------------------- //