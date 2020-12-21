#include "CharacterLuigi.h"

#include "Texture2D.h"

// ------------------------------------------------------------------------------------------------------------------------------------------------------------- //

CharacterLuigi::CharacterLuigi(SDL_Renderer* renderer, std::string imagePath, Vector2D startingPosition) : Character(renderer, imagePath, startingPosition)
{

}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------- //

CharacterLuigi::~CharacterLuigi()
{

}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------- //

void CharacterLuigi::Render()
{
	if(mFacingDirection == FACING::LEFT)
		mTexture->Render(mPosition, SDL_FLIP_HORIZONTAL, 0.0f);
	else
		mTexture->Render(mPosition, SDL_FLIP_NONE, 0.0f);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------- //

void CharacterLuigi::Update(float deltaTime, SDL_Event e)
{
	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case SDLK_w:
			Character::Jump(deltaTime);
		break;

		case SDLK_a:
			mMovingRight = false;
			mMovingLeft = true;
		break;

		case SDLK_d:
			mMovingRight = true;
			mMovingLeft = false;
		break;
		}
		break;

	case SDL_KEYUP:
		switch (e.key.keysym.sym)
		{
		case SDLK_a:
			mMovingLeft = false;
		break;

		case SDLK_d:
			mMovingRight = false;
		break;
		}
		break;

	default:
		break;
	}

	Character::Update(deltaTime, e);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------- //