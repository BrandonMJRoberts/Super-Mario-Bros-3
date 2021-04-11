#ifndef _SPINY_H_
#define _SPINY_H_

#include "RenderObject.h"

struct SDL_Renderer;

class Spiny final : public RenderObject
{
public:
	Spiny(SDL_Renderer* renderer, const char* filePathToSpriteSheet, const float timePerFrame, Vector2D startPos, Vector2D collisionBox, bool spawningOnLeftSide, const float movementSpeed, unsigned int colourID);
	~Spiny();

	void SetPOWHit() override;

private:
	bool ClassSpecificUpdate(const float deltaTime) override;

	Texture2D* GetSpriteSheet() override { return mSpriteSheet; }

	static Texture2D*   mSpriteSheet;
	static unsigned int mSpinyCount;

	unsigned int        mSpriteSheetStartPoint;

	float               mFreezeTimer;
};

#endif