#include "WorldMapMario.h"

#include "GameManager_SMB3.h"

#include <iostream>
#include <SDL.h>

// ------------------------------------------------------------------------------------------- //

WorldMapMario::WorldMapMario(const std::string filePathToSpriteSheet, SDL_Renderer* renderer, Vector2D startPos, const unsigned int spritesOnWidth, const unsigned int spritesOnHeight, const float timePerAnimationFrame) : BaseWorldMapCharacter(renderer, filePathToSpriteSheet, startPos, spritesOnWidth, spritesOnHeight, timePerAnimationFrame)
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