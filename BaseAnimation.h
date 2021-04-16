#ifndef _BASE_ANIMATION_H_
#define _BASE_ANIMATION_H_

#include "Game_Maths.h"

class Texture2D;

class BaseAnimation abstract
{
public:
	BaseAnimation() = delete;
	BaseAnimation(Vector2D startPos, unsigned int startFrame, unsigned int endFrame, const float timePerFrame, Vector2D startVelocity, const unsigned int spritesOnWidth, const unsigned int spritesOnHeight);
	~BaseAnimation();

	virtual void Render(Vector2D offset);
	virtual bool Update(const float deltaTime) { return false; }

	virtual void UpdateAnimations(const float deltaTime);

	virtual Texture2D* GetSpriteSheet() = 0;

protected:
	Vector2D           mPosition;

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