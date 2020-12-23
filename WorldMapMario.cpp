#include "WorldMapMario.h"

#include "GameManager_SMB3.h"

#include <iostream>
#include <SDL.h>

// ------------------------------------------------------------------------------------------- //

WorldMapMario::WorldMapMario(std::string filePathToSpriteSheet, SDL_Renderer* renderer, Vector2D startPos, unsigned int spritesOnWidth, unsigned int spritesOnHeight) : BaseWorldMapCharacter(renderer, filePathToSpriteSheet, startPos, spritesOnWidth, spritesOnHeight, 0.5f)
{
	mCurrentPowerUpState = GameManager_SMB3::GetInstance()->GetMarioCurrentPowerUpState();
}

// ------------------------------------------------------------------------------------------- //

WorldMapMario::~WorldMapMario()
{

}

// ------------------------------------------------------------------------------------------- //

void WorldMapMario::Render()
{
	BaseWorldMapCharacter::Render();
}

// ------------------------------------------------------------------------------------------- //

void WorldMapMario::Update(const float deltaTime)
{
	// First just do the generic world map character stuff
	BaseWorldMapCharacter::Update(deltaTime);
}

// ------------------------------------------------------------------------------------------- //