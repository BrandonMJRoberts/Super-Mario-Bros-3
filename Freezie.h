#ifndef _FREEZIE_H_
#define _FREEZIE_H_

#include "RenderObject.h"

class Texture2D;
struct SDL_Renderer;

class Freezie final : public RenderObject
{
public:
	Freezie(SDL_Renderer* renderer, const char* filePathToSpriteSheet, const float timePerFrame, Vector2D startPos, Vector2D collisionBox, bool spawningOnLeftSide, const float movementSpeed);
	~Freezie();

private:
	Texture2D* GetSpriteSheet() override { return mSpriteSheet; }

	static Texture2D*   mSpriteSheet;
	static unsigned int mFreezieCount;
};

#endif 