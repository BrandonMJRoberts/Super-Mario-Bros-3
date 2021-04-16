#ifndef _COIN_RELEASE_ANIMATION_H_
#define _COIN_RELEASE_ANIMATION_H_

#include "Game_Maths.h"

class  Texture2D;
struct SDL_Renderer;

class CoinReleaseAnimation final
{
public:
	CoinReleaseAnimation() = delete;
	CoinReleaseAnimation(Vector2D startPos, SDL_Renderer* renderer);
	~CoinReleaseAnimation();

	bool Update(const float deltaTime);
	void Render(const Vector2D offset);

private:
	Vector2D           mPosition;

	static Texture2D* mThisSpriteSheet;
	static unsigned int mThisObjectCounter;

	unsigned int       mCurrentFrameID;
	unsigned int       mEndFrameID;
	unsigned int       mStartFrameID;

	const unsigned int kSpritesOnWidth;
	const unsigned int kSpritesOnHeight;

	unsigned int       mSingleSpriteWidth;
	unsigned int       mSingleSpriteHeight;

	float              mTimePerFrame;
	float              mTimeRemainingForFrame;

	Vector2D           mVelocity;

	bool               mCompletedAnimation;
};

#endif