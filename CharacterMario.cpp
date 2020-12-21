#include "CharacterMario.h"

#include "Constants.h"
#include "Texture2D.h"

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------ //

CharacterMario::CharacterMario(SDL_Renderer* renderer, std::string imagePath, Vector2D startingPosition) : Character(renderer, imagePath, startingPosition)
{

}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------ //

CharacterMario::~CharacterMario()
{

}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------ //

void CharacterMario::Update(float deltaTime, SDL_Event e)
{
	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case SDLK_UP:
			Character::Jump(deltaTime);
		break;

		case SDLK_LEFT:
			mMovingRight = false;
			mMovingLeft  = true;
		break;

		case SDLK_RIGHT:
			mMovingRight = true;
			mMovingLeft  = false;
		break;
		}
	break;

	case SDL_KEYUP:
		switch (e.key.keysym.sym)
		{
		case SDLK_LEFT:
			mMovingLeft = false;
		break;

		case SDLK_RIGHT:
			mMovingRight = false;
		break;
		}
	break;

	default:
	break;
	}

	Character::Update(deltaTime, e);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------ //

void CharacterMario::Render()
{
	if(mFacingDirection == FACING::LEFT)
		mTexture->Render(mPosition, SDL_FLIP_HORIZONTAL, 0.0f);
	else
		mTexture->Render(mPosition, SDL_FLIP_NONE, 0.0f);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------ //
