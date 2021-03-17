#ifndef _PLAYABLE_CHARACTER_H
#define _PLAYABLE_CHARACTER_H

#include "Commons_SMB3.h"
#include "Texture2D.h"

#include "Subject.h"

class InteractableLayer;
class ObjectLayer;

// ---------------------------------------------------------------- //

class PlayableCharacter final : public Subject
{
public:
	PlayableCharacter(SDL_Renderer* renderer, const char* filePathToSpriteSheet, Vector2D spawnPoint, const Vector2D levelBounds, const float timePerFrame);
	~PlayableCharacter() override;

	void Render();
	void Update(const float deltaTime, SDL_Event e, const Vector2D levelBounds, InteractableLayer* interactionLayer, ObjectLayer* objectLayer);

	// Getters
	const Vector2D GetRealGridPosition()      const { return mRealGridPosition; }
	const Vector2D GetScreenGridPosition()    const { return mScreenGridPosition; }

	const Vector2D GetRenderReferencePoint()  const { return mRenderRefencePoint; }

	bool           GetIsExitingPipe()         const { return mExitingPipe; }

	bool           GetIsAlive()               const { return mIsAlive; }
	bool           GetShouldDeathTransition() const { return (!mIsAlive && (mDeathAnimationTime < 0.0f)); }

	// Setters
	void           SetLevelOver()                   { mHasControl = false; }

	void           SetEnteringPipe(MOVEMENT_DIRECTION direction);

	void           SpawnIntoNewArea(const Vector2D newPos, const Vector2D newLevelBounds, bool pipeTransition);

private:
	// Movement functions
	void HandleMovementInput(SDL_Event e);

	void ForcedMovementUpdate(const float deltaTime);

	// positional functions
	void CalculateNewPosition(const float deltaTime, CollisionPositionalData xCollisionOccured, CollisionPositionalData yCollisionOccured);
	void CalculateInitialRenderReferencePoint();

	void CalculateScreenBoundsPosition(const Vector2D spawnPoint);

	void CalculateNewScreenPosAndRenderPos(Vector2D movementDistance, const CollisionPositionalData xCollisionOccured, const CollisionPositionalData yCollisionOccured);

	Vector2D ConvertRealPositionIntoScreenPos(Vector2D realPos);

	// Animation functions
	void UpdateAnimationsSmallMario  (PlayerMovementBitField newMovement, bool goingInto);
	void UpdateAnimationsLargeMario  (PlayerMovementBitField newMovement, bool goingInto);
	void UpdateAnimationsFrogMario   (PlayerMovementBitField newMovement, bool goingInto);
	void UpdateAnimationsHammerMario (PlayerMovementBitField newMovement, bool goingInto);
	void UpdateAnimationsFireMario   (PlayerMovementBitField newMovement, bool goingInto);
	void UpdateAnimationsTanookiMario(PlayerMovementBitField newMovement, bool goingInto);
	void UpdateAnimationsLeafMario   (PlayerMovementBitField newMovement, bool goingInto);
	void UpdateAnimationsStarMario   (PlayerMovementBitField newMovement, bool goingInto);

	void HandleChangeInAnimations(PlayerMovementBitField newMovement, bool goingInto);

	void UpdateAnimations(const float deltaTime);

	void LoadInCorrectSpriteSheet();

	// Collision functions
	CollisionPositionalData HandleXCollisions(const float deltaTime, InteractableLayer* interactionLayer, ObjectLayer* objectLayer);
	CollisionPositionalData HandleYCollisions(const float deltaTime, InteractableLayer* interactionLayer, ObjectLayer* objectLayer);

	bool                    HandleCollisionsWithInteractionLayer(InteractableLayer* interactionLayer, const Vector2D newPos);
	MovementPrevention      HandleCollisionsWithInteractionObjectLayer(ObjectLayer* objectLayer, const Vector2D newPos);

	CollisionPositionalData CheckXCollision(const Vector2D positionToCheck1, const Vector2D positionToCheck2, InteractableLayer* interactionLayer, ObjectLayer* objectLayer);
	CollisionPositionalData CheckYCollision(const Vector2D positionToCheck1, const Vector2D positionToCheck2, InteractableLayer* interactionLayer, ObjectLayer* objectLayer);

	void                    UpdatePhysics(const float deltaTime);

	// Member variables
	Vector2D      mRealGridPosition;   // The player's position in the collision world
	Vector2D      mScreenGridPosition; // Player's screen position
	Vector2D      mRenderRefencePoint; // For when rendering the level based on where the player is in it - but not using the player's exact position

	// Physics
	Vector2D      mVelocity;
	Vector2D      mAcceleration;

	// Collision
	Vector2D      mCollisionBox; // Using grid space for the collision box, not pixel size
	Vector2D      mCollisionBoxOffset; // Not all sprites on sheets render from the bottom left so they shouldnt collide from there

	// Level bounding area
	Vector2D      mLevelBounds;

	// Rendering data
	Texture2D*    mSpriteSheet;
	SDL_Renderer* mRenderer;

	// Pixels for sprite width and height
	unsigned int  mSingleSpriteWidth;
	unsigned int  mSingleSpriteHeight;

	// Count of sprites on width and height
	unsigned int  mNumberOfSpritesOnWidth;
	unsigned int  mNumberOfSpritesOnHeight;
	 
	// Rendering frame data
	unsigned int  mCurrentFrame;
	unsigned int  mStartFrame;
	unsigned int  mEndFrame;

	// Time rendering data
	float         mTimeTillNextFrame;
	const float   kBaseTimePerFrame;
	float         mTimePerFrame;

	// Velocity variables
	const float   kMaxHorizontalSpeedWalking;
	const float   kMaxHorizontalSpeedRunning;

	// Terminal velocity on the Y
	const float   kMaxYVelocity;

	// Friction 
	const float   kFrictionMultiplier;

	// Amount the extra jump force being given due to holding jump depreciates
	const float   kJumpHeldAccelerationDepreciationRate;


	const float   kJumpInitialBoost;
	const float   kJumpHeldInitialBoost;
	float         mJumpHeldCurrentBoost;

	// Current player movements
	unsigned int           mCurrentMovements;
	PlayerMovementBitField mAnimationCurrentState;

	// Time for the death animation to play out before going back to the main menu
	float              mDeathAnimationTime;

	// forced transition movement
	const float        kForcedMovementSpeed;
	float              mForcedMovementDistanceTravelled;
	MOVEMENT_DIRECTION mForcedMovementDirection;

	// State variables
	POWER_UP_TYPE mPowerUpState;

	bool         mIsAlive;
	bool         mWasFacingRight;
	bool         mGrounded;
	bool         mHasControl;

	bool         mForceMovementInDirectionSet;

	bool         mExitingPipe;
};


#endif