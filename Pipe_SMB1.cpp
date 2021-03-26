#include "Pipe_SMB1.h"

#include "Texture2D.h"

#include <SDL.h>

// --------------------------------------------------------------------------------------- //

PIPE_SMB1::PIPE_SMB1(SDL_Renderer* renderer, const char* filePathToSpriteSheet, PIPE_FACING_DIRECTION_SMB1 facingDirection, Vector2D bottomLeftPosition)
	: mSpriteSheet(nullptr)

	, mSourceRect(nullptr)
	, mDestRect(nullptr)

	, mPipeFacingDirection(facingDirection)
	, mPosition(bottomLeftPosition)
{
	mSpriteSheet = new Texture2D(renderer);
	if (!mSpriteSheet->LoadFromFile(filePathToSpriteSheet))
	{
		std::cout << "Failed to load sprite sheet for pipes" << std::endl;
	}
}

// --------------------------------------------------------------------------------------- //

PIPE_SMB1::~PIPE_SMB1()
{
	delete mSpriteSheet;
	mSpriteSheet = nullptr;

	delete mSourceRect;
	mSourceRect = nullptr;

	delete mDestRect;
	mDestRect = nullptr;
}

// --------------------------------------------------------------------------------------- //

void PIPE_SMB1::Render()
{
	if (!mSpriteSheet)
		return;

	mSpriteSheet->Render(*mSourceRect, *mDestRect, SDL_FLIP_NONE, 0.0f);
}

// --------------------------------------------------------------------------------------- //

void PIPE_SMB1::Update(const float deltaTime)
{

}

// --------------------------------------------------------------------------------------- //

void PIPE_SMB1::SetReleasingEnemy()
{

}

// --------------------------------------------------------------------------------------- //