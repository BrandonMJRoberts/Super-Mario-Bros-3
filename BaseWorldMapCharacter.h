#ifndef _BASE_WORLD_MAP_CHARACTER_H_
#define _BASE_WORLD_MAP_CHARACTER_H_

#include <string>

#include "Commons_SMB3.h"
#include "Texture2D.h"

struct SDL_Renderer;

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

class BaseWorldMapCharacter abstract
{
public:
	BaseWorldMapCharacter() = delete;
	BaseWorldMapCharacter(SDL_Renderer* renderer, std::string filePathToSpriteSheet, Vector2D startPosition, unsigned int spritesOnWidth, unsigned int spritesOnHeight, const float timePerAnimationFrame);
	virtual ~BaseWorldMapCharacter();

	void      ChangePowerUpState(CHARACTER_MAP_POWER_UP_STATE newState);

	virtual void Render();
	virtual void Update(const float deltaTime);

protected:
	Texture2D*                   mSpriteSheet;

	Vector2D                     mPosition;

	unsigned int                 mAmountOfSpritesOnWidth;
	unsigned int                 mAmountOfSpritesOnHeight;
	unsigned int                 mSingleSpriteWidth;
	unsigned int                 mSingleSpriteHeight;

	unsigned int                 mCurrentFrame;
	unsigned int                 mStartFrame;
	unsigned int                 mEndFrame;

	float                        mTimeRemainingTillFrameChange;
	const float                  mTimePerAnimationFrame;

	CHARACTER_MAP_POWER_UP_STATE mCurrentPowerUpState;
};

#endif // !_BASE_WORLD_MAP_CHARACTER_H_
