#ifndef _PLAYABLE_CHARACTER_H
#define _PLAYABLE_CHARACTER_H

#include "Commons_SMB3.h"
#include "Texture2D.h"

#include "Subject.h"

class PlayableCharacter final : public Subject
{
public:
	PlayableCharacter(SDL_Renderer* renderer, const char* filePathToSpriteSheet, Vector2D spawnPoint, Vector2D numberOfSpritesOnDimensions);
	~PlayableCharacter() override;

	void Render();
	void Update(const float deltaTime, SDL_Event e, const Vector2D levelBounds);

	const Vector2D GetRealGridPosition()     const { return mRealGridPosition; }
	const Vector2D GetScreenGridPosition()   const { return mScreenGridPosition; }

	const Vector2D GetRenderReferencePoint() const { return mRenderRefencePoint; }

private:
	void HandleMovementInput(SDL_Event e);

	void CapRealPositionToPlayableArea(const float deltaTime, const Vector2D levelBounds);
	void CapScreenPositionToInteractableArea(const float deltaTime);
	void CalculateRenderReferencePoint(const Vector2D levelBounds);

	Vector2D   mRealGridPosition;   // The player's position in the collision world
	Vector2D   mScreenGridPosition; // Player's screen position

	Vector2D   mRenderRefencePoint; // For when rendering the level based on where the player is in it - but not using the player's exact position

	Vector2D   mVelocity;
	Vector2D   mAcceleration;

	unsigned int mNumberOfSpritesOnWidth;;

	Texture2D* mSpriteSheet;

	unsigned int mSingleSpriteWidth;
	unsigned int mSingleSpriteHeight;

	unsigned int mCurrentFrame;
	unsigned int mStartFrame;
	unsigned int mEndFrame;

	bool       mIsAlive;
};


#endif