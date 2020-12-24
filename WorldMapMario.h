#ifndef _WORLD_MAP_MARIO_H_
#define _WORLD_MAP_MARIO_H_

#include "Texture2D.h"
#include "Commons_SMB3.h"

#include "BaseWorldMapCharacter.h"

#include <string>

struct SDL_Renderer;

class WorldMapMario final : public BaseWorldMapCharacter
{
public:
	WorldMapMario() = delete;
	WorldMapMario(const std::string filePathToSpriteSheet, SDL_Renderer* renderer, Vector2D startPos, const unsigned int spritesOnWidth, const unsigned int spritesOnHeight, const float timePerAnimationFrame);
	~WorldMapMario() override;

	void Render()                      override;
	void Update(const float deltaTime) override;
};


#endif // !_WORLD_MAP_MARIO_H_
