#ifndef _CRAB_SMB1_H_
#define _CRAB_SMB1_H_

#include "RenderObject.h"

struct SDL_Renderer;
class LevelMap;

class Crab final : public RenderObject
{
public:
	Crab(SDL_Renderer* renderer, const char* filePathToSpriteSheet, const float timePerFrame, Vector2D startPos, Vector2D collisionBox = Vector2D(1.0f, 1.0f));
	~Crab();

	void UpdatePhysics(const float deltaTime, LevelMap* levelMap) override;

private:
	Texture2D* GetSpriteSheet() override { return mSpriteSheet; }

	static Texture2D*   mSpriteSheet;
	static unsigned int mCrabCount;
};

#endif