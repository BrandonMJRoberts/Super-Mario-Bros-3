#include "Character.h"

#include "Constants.h"
#include "Texture2D.h"

#include "LevelMap.h"

// --------------------------------------------------------------------------------------------------------- //

Character::Character(SDL_Renderer* renderer, std::string imagePath, Vector2D startingPosition, const unsigned int spritesOnWidth, const unsigned int spritesOnHeight, LevelMap* levelMap, const float collisionCircleRadius, const float timePerFrame, Audio_Player* AudioPlayerRef)
	: mJumpForce(CHARACTER_INITIAL_JUMP_FORCE)
	, mPlayerMovementData(0)
	, mVelocity(0, 0)
	, mPosition(startingPosition)
	, mSpawnPosition(startingPosition)

	, mRenderer(renderer)

	, kSpritesOnWidth(spritesOnWidth)
	, kSpritesOnHeight(spritesOnHeight)

	, mLevelMap(levelMap)

	, mCollisionRadius(collisionCircleRadius)
	, mCollisionBox(0.0f, 0.0f)

	, mUsingCollisionBox(false)

	, kTimePerFrame(timePerFrame)
	, mTimeRemainingPerFrame(timePerFrame)

	, mSingleSpriteWidth(0)
	, mSingleSpriteHeight(0)

	, mIsAlive(true)
	, mHasCompletedDeathBounce(false)

	, mCoinCount(0)
	, mLifeCount(3)

	, mAudioPlayerRef(AudioPlayerRef)
{
	CalculateSpriteData(renderer, imagePath);
}

// --------------------------------------------------------------------------------------------------------- //

Character::Character(SDL_Renderer* renderer, std::string imagePath, Vector2D startingPosition, const unsigned int spritesOnWidth, const unsigned int spritesOnHeight, LevelMap* levelMap, const Vector2D collisionBox, const float timePerFrame, Audio_Player* AudioPlayerRef)
	: mJumpForce(CHARACTER_INITIAL_JUMP_FORCE)

	, mCollisionBox(collisionBox)
	, mCollisionRadius(0.0f)

	, mRenderer(renderer)

	, mVelocity(0, 0)
	, mPosition(startingPosition)
	, mSpawnPosition(startingPosition)

	, mPlayerMovementData(0)

	, kSpritesOnWidth(spritesOnWidth)
	, kSpritesOnHeight(spritesOnHeight)

	, mLevelMap(levelMap)

	, mUsingCollisionBox(true)

	, mCurrentSpriteID(3)
	, mEndSpriteID(3)
	, mStartSpriteID(3)

	, kTimePerFrame(timePerFrame)
	, mTimeRemainingPerFrame(timePerFrame)

	, mSingleSpriteWidth(0)
	, mSingleSpriteHeight(0)

	, mIsAlive(true)
	, mHasCompletedDeathBounce(false)

	, mCoinCount(0)
	, mLifeCount(3)

	, mAudioPlayerRef(AudioPlayerRef)
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
	mDestRect->x = int(mPosition.x * SPRITE_RES);
	mDestRect->y = int((mPosition.y - mCollisionBox.y) * SPRITE_RES);
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
			// Set that the player is now walking right
			if (!(mPlayerMovementData & PlayerMovementData::WALKING_RIGHT_SMB1))
			{
				mPlayerMovementData |= PlayerMovementData::WALKING_RIGHT_SMB1;
				mPlayerMovementData &= ~(PlayerMovementData::WALKING_LEFT_SMB1);

				// Set that the player is no longer previously facing right
				mPlayerMovementData &= ~(PlayerMovementData::WAS_FACING_RIGHT);

				if (!(mPlayerMovementData & PlayerMovementData::JUMPING_SMB1))
				{
					mEndSpriteID     = 2;
					mStartSpriteID   = 0;
					mCurrentSpriteID = mStartSpriteID;
				}
			}
		break;

		case SDLK_a:
			if (!(mPlayerMovementData & PlayerMovementData::WALKING_LEFT_SMB1))
			{
				mPlayerMovementData |= PlayerMovementData::WALKING_LEFT_SMB1;
				mPlayerMovementData &= ~(PlayerMovementData::WALKING_RIGHT_SMB1);

				if (!(mPlayerMovementData & PlayerMovementData::JUMPING_SMB1))
				{
					mEndSpriteID     = 2;
					mStartSpriteID   = 0;
					mCurrentSpriteID = mStartSpriteID;
				}
			}
		break;

		case SDLK_w:
			if (!(mPlayerMovementData & PlayerMovementData::JUMPING_SMB1))
			{
				Jump();
			}
		break;
		}
	break;

	case SDL_KEYUP:
		switch (e.key.keysym.sym)
		{
		case SDLK_d:
			mPlayerMovementData &= ~(PlayerMovementData::WALKING_RIGHT_SMB1);
			mPlayerMovementData |=   PlayerMovementData::WAS_FACING_RIGHT;

			if (!(mPlayerMovementData & PlayerMovementData::WALKING_LEFT_SMB1))
			{
				if (!(mPlayerMovementData & PlayerMovementData::JUMPING_SMB1))
				{
					mEndSpriteID = 3;
					mStartSpriteID = 3;
				}
				else
				{
					mEndSpriteID = 4;
					mStartSpriteID = 4;
				}

				mCurrentSpriteID = mStartSpriteID;
			}
		break;

		case SDLK_a:
			mPlayerMovementData &= ~(PlayerMovementData::WALKING_LEFT_SMB1);
			mPlayerMovementData &= ~(PlayerMovementData::WAS_FACING_RIGHT);

			if (!(mPlayerMovementData & PlayerMovementData::WALKING_RIGHT_SMB1))
			{
				if (!(mPlayerMovementData & PlayerMovementData::JUMPING_SMB1))
				{
					mEndSpriteID = 3;
					mStartSpriteID = 3;
				}
				else
				{
					mEndSpriteID = 4;
					mStartSpriteID = 4;
				}

				mCurrentSpriteID = mStartSpriteID;
			}
		break;
		}
	break;
	}

	// Determine what velocity the player should have 
	if (mPlayerMovementData & PlayerMovementData::WALKING_RIGHT_SMB1 && !(mPlayerMovementData & PlayerMovementData::WALKING_LEFT_SMB1))
	{
		mVelocity.x = 3.0f;
	}
	else if (mPlayerMovementData& PlayerMovementData::WALKING_LEFT_SMB1 && !(mPlayerMovementData & PlayerMovementData::WALKING_RIGHT_SMB1))
	{
		mVelocity.x = -3.0f;
	}
	else
	{
		mVelocity.x = 0.0f;
	}
}

// --------------------------------------------------------------------------------------------------------- //

void Character::ApplyMovement(const float deltaTime)
{
	// Move the character in the direction they are going
	mPosition += (mVelocity * deltaTime);

	// Screen capping
	if (mPosition.y > SCREEN_HEIGHT_GRID)
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
	if (mIsAlive)
	{
		// Handle if the player has pressed any input
		HandleInput(e);

		// handle if the player has made any collisions
		HandleCollisions(deltaTime);

		// Apply the players movement based off of their current velocity
		ApplyMovement(deltaTime);

		HandleAnimations(deltaTime);

		CheckForLooping();
	}
	else
	{
		HandleAnimations(deltaTime);

		UpdateDeathAnimationMovement(deltaTime);
	}
}

// --------------------------------------------------------------------------------------------------------- //

void Character::AddGravity(const float deltaTime)
{
	mVelocity.y += CHARACTER_GRAVITY * deltaTime;
}

// --------------------------------------------------------------------------------------------------------- //

void Character::HandleCollisions(const float deltaTime)
{
	
	bool  yCollision = true;
	float checkingDistance = 0.15f;

	// first check y axis
	if (       mVelocity.y >= 0.0f
		&& (   mLevelMap->GetCollisionTileAt(int(mPosition.y + checkingDistance), int(mPosition.x)) // Bottom left 
			|| mLevelMap->GetCollisionTileAt(int(mPosition.y + checkingDistance), int(mPosition.x + mCollisionBox.x)))) // Bottom right
	{
		// Set that you are not jumping
		mPlayerMovementData &= ~(PlayerMovementData::JUMPING_SMB1);

		// Set the position
		mPosition.y = int(mPosition.y + checkingDistance) - 0.001;

		// zero the velocity
		mVelocity.y = 0.0f;

		// See if the player should revert back to normal standing frame
		if (   !(mPlayerMovementData & PlayerMovementData::WALKING_LEFT_SMB1)
			&& !(mPlayerMovementData & PlayerMovementData::WALKING_RIGHT_SMB1))
		{
			mCurrentSpriteID = 3;
			mEndSpriteID     = 3;
			mStartSpriteID   = 3;
		}
		else
		{
			mEndSpriteID   = 2;
			mStartSpriteID = 0;
		}
	}
	else if (  mVelocity.y < 0.0f  // Head collision
		&& (   mLevelMap->GetCollisionTileAt(int(mPosition.y - mCollisionBox.y - checkingDistance), int(mPosition.x)) // Top left 
			|| mLevelMap->GetCollisionTileAt(int(mPosition.y - mCollisionBox.y - checkingDistance), int(mPosition.x + mCollisionBox.x)))) // Top right
	{
		mVelocity.y = 2.0f;
	}
	else
	{
		yCollision = false;

		mVelocity.y += CHARACTER_GRAVITY * deltaTime;
		std::cout << "GRAVITY!" << std::endl;
	}


	// X axis
	if (       mVelocity.x <= 0.0f && 
		(      mLevelMap->GetCollisionTileAt(int(mPosition.y - mCollisionBox.y),          int(mPosition.x - checkingDistance)) // Top left
			|| mLevelMap->GetCollisionTileAt(int(mPosition.y),                            int(mPosition.x - checkingDistance))
			|| mLevelMap->GetCollisionTileAt(int(mPosition.y - (mCollisionBox.y / 2.0f)), int(mPosition.x - checkingDistance)))) // bottom left
	{
		// Zero the velocity
		mVelocity.x = 0.0f;

		mPosition.x = int(mPosition.x - checkingDistance) + 1.01;

		mPlayerMovementData &= ~(PlayerMovementData::WAS_FACING_RIGHT);
	}
	else if (  mVelocity.x >= 0.0f &&
		 (     mLevelMap->GetCollisionTileAt(int(mPosition.y - mCollisionBox.y),          int(mPosition.x + mCollisionBox.x + checkingDistance))   // Top right
			|| mLevelMap->GetCollisionTileAt(int(mPosition.y),                            int(mPosition.x + mCollisionBox.x + checkingDistance))
			|| mLevelMap->GetCollisionTileAt(int(mPosition.y - (mCollisionBox.y / 2.0f)), int(mPosition.x + mCollisionBox.x + checkingDistance)))) // bottom right
	{
		// Zero the velocity
		mVelocity.x = 0.0f;

		// Set the position
		mPosition.x = int(mPosition.x + checkingDistance) - 0.01;

		mPlayerMovementData |= PlayerMovementData::WAS_FACING_RIGHT;
	}
	else
	{
		mPosition.x += (mVelocity * deltaTime).x;
	}

	if (!yCollision)
	{
		mPosition.y += (mVelocity * deltaTime).y;
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

	mStartSpriteID   = 4;
	mEndSpriteID     = 4;
	mCurrentSpriteID = mStartSpriteID;

	if(mAudioPlayerRef)
		mAudioPlayerRef->OnNotify(SUBJECT_NOTIFICATION_TYPES::PLAYER_JUMPED, "");
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

	if (kSpritesOnHeight == 0 || kSpritesOnWidth == 0)
		return;

	mSingleSpriteWidth  = mTexture->GetWidth()  / kSpritesOnWidth;
	mSingleSpriteHeight = mTexture->GetHeight() / kSpritesOnHeight;

	mSourceRect = new SDL_Rect{       int(mSingleSpriteWidth * (mCurrentSpriteID % kSpritesOnWidth)),
									  int(mSingleSpriteHeight * int(mCurrentSpriteID / kSpritesOnWidth)),
									  (int)mSingleSpriteWidth,
									  (int)mSingleSpriteHeight };

	mDestRect = new SDL_Rect{           int(mPosition.x * SPRITE_RES),
									  (int)((mPosition.y - mCollisionBox.y) * SPRITE_RES),
									   (int)mSingleSpriteWidth,
									   (int)mSingleSpriteHeight };
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

void Character::HandleAnimations(const float deltaTime)
{
	mTimeRemainingPerFrame -= deltaTime;

	if (mTimeRemainingPerFrame < 0.0f)
	{
		mCurrentSpriteID++;
		mTimeRemainingPerFrame = kTimePerFrame;

		if (mCurrentSpriteID > mEndSpriteID)
		{
			mCurrentSpriteID = mStartSpriteID;
		}
	}

	// Now update the sprite sheet position
	mSourceRect->x = int(mSingleSpriteWidth  *    (mCurrentSpriteID % kSpritesOnWidth));
	mSourceRect->y = int(mSingleSpriteHeight * int(mCurrentSpriteID / kSpritesOnWidth));
}

// --------------------------------------------------------------------------------------------------------- //

void Character::SetHasBeenHit()
{
	if (!mIsAlive)
		return;

	mIsAlive         = false;

	mStartSpriteID   = 10;
	mEndSpriteID     = 10;
	mCurrentSpriteID = mStartSpriteID;

	mDeathPosition   = mPosition;

	mLifeCount--;

	mVelocity        = Vector2D(0.0f, -1.0f);

	if (mLifeCount < 0)
	{
		mLifeCount = 0;
	}

	if (mAudioPlayerRef)
		mAudioPlayerRef->OnNotify(SUBJECT_NOTIFICATION_TYPES::PLAYER_DIED, "SMB1");
}

// --------------------------------------------------------------------------------------------------------- //

void Character::UpdateDeathAnimationMovement(const float deltaTime)
{
	// Going down
	mVelocity.y += deltaTime * DEATH_ANIMATION_SPEED;

	mPosition.y += mVelocity.y * deltaTime;

	if (mPosition.y - mCollisionBox.y > mLevelMap->GetLevelHeight())
	{
		RespawnPlayer();
	}
}

// --------------------------------------------------------------------------------------------------------- //

void Character::RespawnPlayer()
{
	mIsAlive  = true;
	mPosition = mSpawnPosition;

	mCurrentSpriteID = 3;
	mEndSpriteID     = 3;
	mStartSpriteID   = 3;

	mVelocity        = Vector2D();

	mTimeRemainingPerFrame   = kTimePerFrame;

	mPlayerMovementData      = 0;

	mHasCompletedDeathBounce = false;

	mSourceRect = new SDL_Rect{        int(mSingleSpriteWidth * (mCurrentSpriteID % kSpritesOnWidth)),
									   int(mSingleSpriteHeight * int(mCurrentSpriteID / kSpritesOnWidth)),
									  (int)mSingleSpriteWidth,
									  (int)mSingleSpriteHeight };

	mDestRect = new SDL_Rect{          int(mPosition.x * SPRITE_RES),
									  (int)((mPosition.y - mCollisionBox.y) * SPRITE_RES),
									   (int)mSingleSpriteWidth,
									   (int)mSingleSpriteHeight };
}

// --------------------------------------------------------------------------------------------------------- //
