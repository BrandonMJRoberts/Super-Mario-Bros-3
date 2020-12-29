#include "Luigi.h"

// ------------------------------------------------------------------------------------------------------------------------- //

Luigi::Luigi(SDL_Renderer* renderer, const char* filePathToSpriteSheet, Vector2D spawnPoint, Vector2D numberOfSpritesOnDimensions) : BaseCharacter(renderer, filePathToSpriteSheet, spawnPoint, numberOfSpritesOnDimensions)
{

}

// ------------------------------------------------------------------------------------------------------------------------- //

Luigi::~Luigi()
{

}

// ------------------------------------------------------------------------------------------------------------------------- //

void Luigi::Render()
{
	BaseCharacter::Render();
}

// ------------------------------------------------------------------------------------------------------------------------- //

void Luigi::Update(const float deltaTime, SDL_Event e)
{
	BaseCharacter::Update(deltaTime, e);
}

// ------------------------------------------------------------------------------------------------------------------------- //