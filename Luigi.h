#ifndef _LUIGI_H_
#define _LUIGI_H_

#include "BaseCharacter.h"

class Luigi : public BaseCharacter
{
public:
	Luigi() = delete;
	Luigi(SDL_Renderer* renderer, const char* filePathToSpriteSheet, Vector2D spawnPoint, Vector2D numberOfSpritesOnDimensions);
	~Luigi() override;

	void Render() override final;
	void Update(const float deltaTime, SDL_Event e) override final;

private:

};

#endif