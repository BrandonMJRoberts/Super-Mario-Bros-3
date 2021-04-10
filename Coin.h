#ifndef _COIN_SMB1_H_
#define _COIN_SMB1_H_

#include "RenderObject.h"

struct SDL_Renderer;
class  LevelMap;

class Coin final : public RenderObject
{
public:
	Coin(SDL_Renderer* renderer, const Vector2D startPosition, const char* filePathToSpriteSheet, const float timePerFrame, Vector2D collisionBox);
	~Coin();

	bool Update(const float deltaTime, LevelMap* levelMap) override;

	void AddVelocity(Vector2D change) { mVelocity += change; }

private:
	Texture2D* GetSpriteSheet() override { return mSpriteSheet; }

	static Texture2D*   mSpriteSheet;
	static unsigned int mCoinCount;
};

#endif