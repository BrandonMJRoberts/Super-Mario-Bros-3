#ifndef _COIN_SMB1_H_
#define _COIN_SMB1_H_

#include "RenderObject.h"

struct SDL_Renderer;
class  LevelMap;

class Coin final : public RenderObject
{
public:
	Coin(SDL_Renderer* renderer, const Vector2D startPosition, const char* filePathToSpriteSheet, const float timePerFrame, Vector2D collisionBox = Vector2D(1.0f, 1.0f));
	~Coin();

	void UpdatePhysics(const float deltaTime, LevelMap* levelMap) override;

private:
	Texture2D* GetSpriteSheet() override { return mSpriteSheet; }

	static Texture2D*   mSpriteSheet;
	static unsigned int mCoinCount;
};

#endif