#ifndef _RENDER_OBJECT_H_
#define _RENDER_OBJECT_H_

#include "Game_Maths.h"

#include "Constants.h"

struct SDL_Rect;
class  Texture2D;
class  LevelMap;

class RenderObject abstract
{
public:
	RenderObject();
	RenderObject(unsigned int start, unsigned int end, unsigned int current, const float timePerFrame, Vector2D startPosition, const unsigned int spritesOnWidth, const unsigned int spritesOnHeight, Vector2D collisionBox);
	~RenderObject();

	virtual void Update(const float deltaTime, LevelMap* levelMap);

	virtual void Render();

	Vector2D GetPosition()     const { return mPosition; }
	Vector2D GetCollisionBox() const { return mCollisionBox; }

protected:
	virtual void       UpdatePhysics(const float deltaTime, LevelMap* levelMap) = 0;
	virtual Texture2D* GetSpriteSheet() = 0;

	void               SetupRenderRects();

	Vector2D     mPosition;
	Vector2D     mVelocity;
	Vector2D     mCollisionBox;
	
	unsigned int mCurrentSpriteID;
	unsigned int mEndSpriteID;
	unsigned int mStartFrameID;

	const unsigned int kSpritesOnWidth;
	const unsigned int kSpritesOnHeight;

	const float  kTimePerFrame;
	float		 mTimeRemainingPerFrame;

	float mSingleSpriteHeight;
	float mSingleSpriteWidth;

	SDL_Rect*    mSourceRect;
	SDL_Rect*    mDestRect;

	bool         mFacingLeft;
};

#endif