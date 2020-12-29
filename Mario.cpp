#include "Mario.h"

// ------------------------------------------------------------------------------------------------------------------------- //

Mario::Mario(SDL_Renderer* renderer, const char* filePathToSpriteSheet, Vector2D spawnPoint, Vector2D numberOfSpritesOnDimensions) : BaseCharacter(renderer, filePathToSpriteSheet, spawnPoint, numberOfSpritesOnDimensions)
{

}

// ------------------------------------------------------------------------------------------------------------------------- //

Mario::~Mario()
{

}

// ------------------------------------------------------------------------------------------------------------------------- //

void Mario::Render()
{
	BaseCharacter::Render();
}

// ------------------------------------------------------------------------------------------------------------------------- //

void Mario::Update(const float deltaTime, SDL_Event e)
{
	BaseCharacter::Update(deltaTime, e);
}

// ------------------------------------------------------------------------------------------------------------------------- //