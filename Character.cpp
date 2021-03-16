#include "Character.h"

#include "Constants.h"
#include "Texture2D.h"

#include "LevelMap.h"

// --------------------------------------------------------------------------------------------------------- //

Character::Character(SDL_Renderer* renderer, std::string imagePath, Vector2D startingPosition, const unsigned int spritesOnWidth, const unsigned int spritesOnHeight, LevelMap* levelMap, const float collisionCircleRadius)
	: mJumpForce(CHARACTER_INITIAL_JUMP_FORCE)
	, mCollisionRadius(collisionCircleRadius)
	, mRenderer(renderer)
	, mVelocity(0, 0)
	, mPosition(startingPosition)
	, kSpritesOnWidth(spritesOnWidth)
	, kSpritesOnHeight(spritesOnHeight)
	, mLevelMap(levelMap)
	, mPlayerMovementData(0)
	, mCollisionBox(0.0f, 0.0f)
{
	mTexture = new Texture2D(renderer);
	if (!mTexture->LoadFromFile(imagePath))
	{
		std::cout << "Failed to load the character sprite." << std::endl;
	}
}

// --------------------------------------------------------------------------------------------------------- //

Character::Character(SDL_Renderer* renderer, std::string imagePath, Vector2D startingPosition, const unsigned int spritesOnWidth, const unsigned int spritesOnHeight, LevelMap* levelMap, const Vector2D collisionBox)
	: mJumpForce(CHARACTER_INITIAL_JUMP_FORCE)
	, mCollisionBox(collisionBox)
	, mRenderer(renderer)
	, mVelocity(0, 0)
	, mPosition(startingPosition)
	, kSpritesOnWidth(spritesOnWidth)
	, kSpritesOnHeight(spritesOnHeight)
	, mLevelMap(levelMap)
	, mCollisionRadius(0.0f)
	, mPlayerMovementData(0)
{
	mTexture = new Texture2D(renderer);
	if (!mTexture->LoadFromFile(imagePath))
	{
		std::cout << "Failed to load the character sprite." << std::endl;
	}
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
	if(mVelocity.x < 0.0f)
		mTexture->Render(mPosition, SDL_FLIP_HORIZONTAL, 0.0f);
	else
		mTexture->Render(mPosition, SDL_FLIP_NONE, 0.0f);

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
			WalkRight();
		break;

		case SDLK_a:
			WalkLeft();
		break;

		case SDLK_w:
			if(!(mPlayerMovementData & PlayerMovementData::JUMPING_SMB1))
				Jump();
		break;
		}
	break;
	}
}

// --------------------------------------------------------------------------------------------------------- //

void Character::ApplyMovement(const float deltaTime)
{
	mPosition += (mVelocity * deltaTime);
}

// --------------------------------------------------------------------------------------------------------- //

void Character::Update(float deltaTime, SDL_Event e)
{
	HandleInput(e);

	UpdatePhysics(deltaTime);

	ApplyMovement(deltaTime);
}

// --------------------------------------------------------------------------------------------------------- //

void Character::UpdatePhysics(const float deltaTime)
{
	// First check for collisions
	if(mLevelMap->GetTileAt(mPosition.x + (mVelocity.x * deltaTime), mPosition.y) != 0)
	{
		// There is a collision to the right of the player

	}

	//if()

	// Now apply the change in velocity based on those collisions
	mVelocity.y -= (CHARACTER_GRAVITY * deltaTime);
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

void Character::WalkRight()
{
	// Set that the player is now jumping
	mPlayerMovementData |= PlayerMovementData::WALKING_RIGHT_SMB1;

	// Apply the jump force
	mVelocity.x = 3.0f;
}

// --------------------------------------------------------------------------------------------------------- //

void Character::WalkLeft()
{
	// Set that the player is now jumping
	mPlayerMovementData |= PlayerMovementData::WALKING_LEFT_SMB1;

	// Apply the jump force
	mVelocity.x = -3.0f;
}

// --------------------------------------------------------------------------------------------------------- //