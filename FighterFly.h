#ifndef _FIGHTER_FLY_H_
#define _FIGHTER_FLY_H_

#include "RenderObject.h"

class Texture2D;
struct SDL_Renderer;

class FighterFly final : public RenderObject
{
public:
	FighterFly(SDL_Renderer* renderer, const char* filePathToSpriteSheet, const float timePerFrame, Vector2D startPos, Vector2D collisionBox, bool spawningOnLeftSide, const float movementSpeed, unsigned int colourID);
	~FighterFly();

private:
	Texture2D* GetSpriteSheet() override { return mSpriteSheet; }
	
	static Texture2D*	mSpriteSheet;
	static unsigned int mFighterFlyCount;
};

#endif