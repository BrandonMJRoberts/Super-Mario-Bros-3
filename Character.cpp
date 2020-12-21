#include "Character.h"

#include "Constants.h"
#include "Texture2D.h"

// --------------------------------------------------------------------------------------------------------- //

Character::Character(SDL_Renderer* renderer, std::string imagePath, Vector2D startingPosition)
{
	mRenderer = renderer;

	mTexture = new Texture2D(renderer);
	if (!mTexture->LoadFromFile(imagePath))
	{
		std::cout << "Failed to load the character sprite." << std::endl;
	}

	// Default to facing to the right
	mFacingDirection = FACING::RIGHT;

	mMovingLeft  = false;
	mMovingRight = false;

	mJumping   = false;
	mJumpForce = CHARACTER_INITIAL_JUMP_FORCE;
	mCanJump   = false;

	mCollisionRadius = 15.0f;
}

// --------------------------------------------------------------------------------------------------------- //

Character::~Character()
{
	mRenderer = NULL;
}

// --------------------------------------------------------------------------------------------------------- //

void Character::Render()
{
	if(mFacingDirection == FACING::LEFT)
		mTexture->Render(mPosition, SDL_FLIP_HORIZONTAL, 0.0f);
	else
		mTexture->Render(mPosition, SDL_FLIP_NONE, 0.0f);

}

// --------------------------------------------------------------------------------------------------------- //

void Character::Update(float deltaTime, SDL_Event e)
{
	if (mJumping)
	{
		Jump(deltaTime);
	}
	else
	{
		if (mPosition.y >= SCREEN_HEIGHT - 64)
		{
			mPosition.y = SCREEN_HEIGHT - 64;
			mCanJump = true;
		}
		else if (mPosition.y < SCREEN_HEIGHT - 64)
			AddGravity(deltaTime);
	}

	if (mMovingLeft)
		MoveLeft(deltaTime);
	else if (mMovingRight)
		MoveRight(deltaTime);
}

// --------------------------------------------------------------------------------------------------------- //

void Character::SetPosition(Vector2D position)
{
	mPosition = position;
}

// --------------------------------------------------------------------------------------------------------- //

void Character::MoveLeft(float deltaTime)
{
	mFacingDirection = FACING::LEFT;

	mPosition.x -= (CHARACTER_MOVEMENT_SPEED * deltaTime);
}

// --------------------------------------------------------------------------------------------------------- //

void Character::MoveRight(float deltaTime)
{
	mFacingDirection = FACING::RIGHT;

	mPosition.x += (CHARACTER_MOVEMENT_SPEED * deltaTime);
}

// --------------------------------------------------------------------------------------------------------- //

void Character::AddGravity(float deltaTime)
{
	mPosition.y -= CHARACTER_GRAVITY * deltaTime;
}

// --------------------------------------------------------------------------------------------------------- //

void Character::Jump(float deltaTime)
{
	if (!mJumping)
	{
		mJumpForce = CHARACTER_INITIAL_JUMP_FORCE;
		mJumping   = true;
		mCanJump   = false;
	}
	else
	{
		mPosition.y -= mJumpForce * deltaTime;

		mJumpForce -= CHARACTER_JUMP_FORCE_DECREMENT * deltaTime;

		if (mJumpForce <= 0.0f)
		{
			mJumping   = false;
			mJumpForce = 0.0f;
			mCanJump   = true;
		}
	}
}

// --------------------------------------------------------------------------------------------------------- //