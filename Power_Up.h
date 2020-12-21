#ifndef _POWER_UP_H_
#define _POWER_UP_H_

#include <SDL.h>
#include "Commons_SMB3.h"

// Forward declare texture2D
class Texture2D;

class PowerUp
{
public:
	PowerUp();
	PowerUp(SDL_Renderer* renderer, const Vector2D startPosition, const POWER_UP_TYPE startPowerUpType, const Vector2D startVelocity = Vector2D());
	~PowerUp();

	void Render();
	void Update(const float deltaTime);

private:
	Texture2D*    mSprite;
	SDL_Renderer* mRenderer;

	Vector2D      mPosition;
	Vector2D      mVelocity;

	float		  mWidth;
	float		  mHeight;

	unsigned int  mCurrentSpriteIndex;
	unsigned int  mFinalAnimationSpriteIndex;
	unsigned int  mStartAnimationSpriteIndex;

	bool          mIsFacingRight;
	bool          mIsUsingGravity;

	POWER_UP_TYPE mPowerUpType;
};

#endif // !_BASE_POWER_UP_H_
