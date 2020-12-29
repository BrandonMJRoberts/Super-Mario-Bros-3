#ifndef _MARIO_H_
#define _MARIO_H_

#include "BaseCharacter.h"

class Mario : public BaseCharacter
{
public:
	Mario() = delete;
	Mario(SDL_Renderer* renderer, const char* filePathToSpriteSheet, Vector2D spawnPoint, Vector2D numberOfSpritesOnDimensions);
	~Mario() override;

	void Render() override final;
	void Update(const float deltaTime, SDL_Event e) override final;

private:

};

#endif