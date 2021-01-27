#ifndef _PLAYABLE_CHARACTER_H
#define _PLAYABLE_CHARACTER_H

#include "Commons_SMB3.h"
#include "Texture2D.h"

#include "Subject.h"

class InteractableLayer;
class ObjectLayer;

class PlayableCharacter final : public Subject
{
public:
	PlayableCharacter(SDL_Renderer* renderer, const char* filePathToSpriteSheet, Vector2D spawnPoint, Vector2D numberOfSpritesOnDimensions, const Vector2D levelBounds);
	~PlayableCharacter() override;

	void Render();
	void Update(const float deltaTime, SDL_Event e, const Vector2D levelBounds, InteractableLayer* interactionLayer, ObjectLayer* objectLayer);

	const Vector2D GetRealGridPosition()     const { return mRealGridPosition; }
	const Vector2D GetScreenGridPosition()   const { return mScreenGridPosition; }

	const Vector2D GetRenderReferencePoint() const { return mRenderRefencePoint; }

	void           SpawnIntoNewArea(const Vector2D newPos, const Vector2D newLevelBounds);

private:
	void HandleMovementInput(SDL_Event e);

	void CalculateNewPosition(const float deltaTime, const Vector2D newPos);
	void CalculateInitialRenderReferencePoint();

	void CalculateScreenBoundsPosition(const Vector2D spawnPoint);

	bool HandleCollisionsWithInteractionLayer(InteractableLayer* interactionLayer, const Vector2D newPos);
	bool HandleCollisionsWithInteractionObjectLayer(ObjectLayer* objectLayer, const Vector2D newPos);

	bool CheckXCollision(const Vector2D positionToCheck1, const Vector2D positionToCheck2, InteractableLayer* interactionLayer, ObjectLayer* objectLayer, double& newXPosRef);
	bool CheckYCollision(const Vector2D positionToCheck1, const Vector2D positionToCheck2, InteractableLayer* interactionLayer, ObjectLayer* objectLayer, double& newYPosRef);

	void UpdatePhysics(const float deltaTime);

	Vector2D   mRealGridPosition;   // The player's position in the collision world
	Vector2D   mScreenGridPosition; // Player's screen position

	Vector2D   mRenderRefencePoint; // For when rendering the level based on where the player is in it - but not using the player's exact position

	Vector2D   mVelocity;
	Vector2D   mAcceleration;

	Vector2D   mCollisionBox; // Using grid space for the collision box, not pixel size

	Vector2D   mLevelBounds;

	unsigned int mNumberOfSpritesOnWidth;;

	Texture2D*   mSpriteSheet;

	unsigned int mSingleSpriteWidth;
	unsigned int mSingleSpriteHeight;

	unsigned int mCurrentFrame;
	unsigned int mStartFrame;
	unsigned int mEndFrame;

	const float  kMaxSpeedWalking;
	const float  kMaxSpeedRunning;

	const float  kFrictionMultiplier;

	bool       mIsAlive;
	bool       mApplyFriction;

	bool       mIsWalking;
};


#endif