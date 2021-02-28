#ifndef _GOOMBA_SMB3_H_
#define _GOOMBA_SMB3_H_

#include "EnemyObject.h"

class Goomba : public EnemyObject
{
public:
	Goomba(const Vector2D      spawnPosition,
		   const bool          startSpawnedInLevel,
		   SDL_Renderer*       renderer,
		   const std::string   filePathToSpriteSheet,
		   const unsigned int  spritesOnWidth,
		   const unsigned int  spritesOnHeight,
		   const double        collisionBoxWidth,
		   const double        collisionBoxHeight,
		   const float		   timePerFrame,
		   const bool          canMove,
	       const bool          canJump,
		   const bool          startFacingLeft);
	virtual ~Goomba() override;

	virtual BaseObject* Clone(std::string data) override;

	virtual bool Update(const float deltaTime, const Vector2D playerPosition, InteractableLayer* interactionLayer) override;

	void         Render(const Vector2D renderReferencePoint) override;

	virtual void Move()   override;
	virtual void Jump()   override;
	virtual void Attack() override;

	void         ResetUpdatedStaticVariables() override { mUpdatedStaticVariables = false; }

	ObjectCollisionHandleData SetIsCollidedWith(TwoDimensionalCollision collisionData, const unsigned int playerMovements) override;
private:
	void         UpdateStaticVariables(const float deltaTime);

	RenderData   GetRenderData() override { return RenderData{ mCurrentSpriteID, mStartSpriteID, mEndSpriteID, mTimeRemainingTillNextFrame, mTimePerFrame }; }

	static bool                 mUpdatedStaticVariables;

	static unsigned int         mCurrentSpriteID;
	static unsigned int         mEndSpriteID;
	static unsigned int         mStartSpriteID;

	static float                mTimeRemainingTillNextFrame;
	const float					mTimePerFrame;

	unsigned int                mHitsRemaining;

	float                       mTimerTillDespawn;

};

#endif