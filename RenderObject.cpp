#include "RenderObject.h"

#include "Texture2D.h"

#include <SDL.h>

#include "Constants.h"

// --------------------------------------------------------------- //

RenderObject::RenderObject()
	: mCurrentSpriteID(0)
	, mEndSpriteID(0)
	, mStartFrameID(0)

	, kTimePerFrame(0.0f)
	, mTimeRemainingPerFrame(kTimePerFrame)

	, mPosition(0.0f, 0.0f)

	, mSourceRect(nullptr)
	, mDestRect(nullptr)

	, kSpritesOnWidth(0)
	, kSpritesOnHeight(0)
{

}

// --------------------------------------------------------------- //

RenderObject::RenderObject(unsigned int start, unsigned int end, unsigned int current, const float timePerFrame, Vector2D startPosition, const unsigned int spritesOnWidth, const unsigned int spritesOnHeight)
	: mCurrentSpriteID(current)
	, mEndSpriteID(end)
	, mStartFrameID(start)

	, kTimePerFrame(timePerFrame)
	, mTimeRemainingPerFrame(timePerFrame)

	, mPosition(startPosition)

	, mSourceRect(nullptr)
	, mDestRect(nullptr)

	, kSpritesOnWidth(spritesOnWidth)
	, kSpritesOnHeight(spritesOnHeight)
{

}

// --------------------------------------------------------------- //

RenderObject::~RenderObject()
{
	delete mSourceRect;
	mSourceRect = nullptr;

	delete mDestRect;
	mDestRect = nullptr;
}

// --------------------------------------------------------------- //

void RenderObject::Render()
{
	// Basic render function
	Texture2D* texture = GetSpriteSheet();

	if (!texture || !mSourceRect || !mDestRect)
		return;

	texture->Render(*mSourceRect, *mDestRect, SDL_FLIP_NONE, 0.0f);
}

// --------------------------------------------------------------- //

void RenderObject::Update(const float deltaTime)
{
	// Take the time off 
	mTimeRemainingPerFrame -= deltaTime;

	// Check for the time boundary
	if (mTimeRemainingPerFrame < 0.0f)
	{
		// Reset the time
		mTimeRemainingPerFrame = kTimePerFrame;

		// Increment the sprite ID
		mCurrentSpriteID++;

		// Check for looping boundaries
		if (mCurrentSpriteID > mEndSpriteID)
		{
			mCurrentSpriteID = 0;
		}
	}

	// Now update the physics of this object - this should be overriden by the child class
	UpdatePhysics();
}

// --------------------------------------------------------------- //

void RenderObject::SetupRenderRects()
{
	// First get the texture
	Texture2D* texture = GetSpriteSheet();

	if (!texture)
		return;

	int spriteWidth  = texture->GetWidth()  / kSpritesOnWidth;
	int spriteHeight = texture->GetHeight() / kSpritesOnHeight;

	mSourceRect        = new SDL_Rect{int(spriteWidth  *    (mCurrentSpriteID % kSpritesOnWidth)),
		                              int(spriteHeight * int(mCurrentSpriteID / kSpritesOnWidth)),
		                              spriteWidth, 
		                              spriteHeight };

	mDestRect          = new SDL_Rect{   int(mPosition.x         * SPRITE_RES), 
									  (int)((mPosition.y - 1.0f) * SPRITE_RES),
		                               spriteWidth, 
		                               spriteHeight };
}

// --------------------------------------------------------------- //
