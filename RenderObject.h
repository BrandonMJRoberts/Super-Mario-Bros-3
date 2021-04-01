#ifndef _RENDER_OBJECT_H_
#define _RENDER_OBJECT_H_

#include "Game_Maths.h"

struct SDL_Rect;
class  Texture2D;

class RenderObject abstract
{
public:
	RenderObject();
	RenderObject(unsigned int start, unsigned int end, unsigned int current, const float timePerFrame, Vector2D startPosition, const unsigned int spritesOnWidth, const unsigned int spritesOnHeight);
	~RenderObject();

	virtual void Update(const float deltaTime);

	virtual void Render();

protected:
	virtual void       UpdatePhysics() = 0;
	virtual Texture2D* GetSpriteSheet() = 0;

	void               SetupRenderRects();

	Vector2D     mPosition;
	
	unsigned int mCurrentSpriteID;
	unsigned int mEndSpriteID;
	unsigned int mStartFrameID;

	const unsigned int kSpritesOnWidth;
	const unsigned int kSpritesOnHeight;

	const float  kTimePerFrame;
	float		 mTimeRemainingPerFrame;

	SDL_Rect*    mSourceRect;
	SDL_Rect*    mDestRect;
};

#endif