#ifndef _PARA_GOOMBA_SMB3_H_
#define _PARA_GOOMBA_SMB3_H_

#include "Goomba_SMB3.h"

#include "Wings.h"

class ParaGoomba final : public Goomba
{
public:
	ParaGoomba(const Vector2D      spawnPosition,
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
	~ParaGoomba() override;

	BaseObject* Clone(std::string data) override;

	bool Update(const float deltaTime, const Vector2D playerPosition, InteractableLayer* interactionLayer, ObjectLayer* objectLayer) override;
	void Render(const Vector2D renderReferencePoint) override;

	void Move()   override;
	void Jump()   override;
	void Attack() override;


	void        ResetUpdatedStaticVariables() override { mUpdatedStaticVariables = false; }

	ObjectCollisionHandleData SetIsCollidedWith(TwoDimensionalCollision collisionData, const unsigned int playerMovements, const bool isPlayer) override;

private:
	void        UpdateStaticVariables(const float deltaTime);

	RenderData  GetRenderData() override { return RenderData{ mCurrentSpriteID, mStartSpriteID, mEndSpriteID, mTimeRemainingTillNextFrame, mTimePerFrame }; }

	static bool				    mUpdatedStaticVariables;

	unsigned int				mCurrentSpriteID;
	unsigned int				mEndSpriteID;
	unsigned int				mStartSpriteID;

	float						mTimeRemainingTillNextFrame;
	const float				    mTimePerFrame;

	float                       mJumpForce;

	float                       mJumpTimer;
	unsigned int                mJumpsCompleted;

	const float                 kTimePerJump;
	const float                 kTimePerJumpLoop;

	Wings*                      mWings;
	Vector2D                    mWingOffset;

	float                       mHitDelay;
};

#endif