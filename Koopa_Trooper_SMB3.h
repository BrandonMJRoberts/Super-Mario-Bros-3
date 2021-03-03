#ifndef _KOOPA_TROOPER_SMB3_H_
#define _KOOPA_TROOPER_SMB3_H_

#include "EnemyObject.h"

class KoopaTrooper : public EnemyObject
{
public:
	KoopaTrooper(const Vector2D      spawnPosition,
				 const bool          startSpawnedInLevel,
				 SDL_Renderer*		 renderer,
				 const std::string   filePathToSpriteSheet,
				 const unsigned int  spritesOnWidth,
				 const unsigned int  spritesOnHeight,
				 const double        collisionBoxWidth,
				 const double        collisionBoxHeight,
				 const float		 timePerFrame,
				 const bool          canMove,
				 const bool          canJump,
				 const bool          startFacingLeft,
		         const char          colourIndexOfKoopa);
	virtual ~KoopaTrooper() override;

	virtual BaseObject* Clone(std::string dataLine) override;

	virtual bool        Update(const float deltaTime, const Vector2D playerPosition, InteractableLayer* interactionLayer, ObjectLayer* objectLayer) override;
	void                Render(const Vector2D renderReferencePoint) override;

	void        ResetUpdatedStaticVariables() override { mUpdatedStaticVariables = false; }

	ObjectCollisionHandleData SetIsCollidedWith(TwoDimensionalCollision collisionData, const unsigned int playerMovements, const bool isPlayer) override;

private:
	void        UpdateStaticVariables(const float deltaTime);

	RenderData  GetRenderData() override { return RenderData{ mCurrentSpriteID, mStartSpriteID, mEndSpriteID, mTimeRemainingTillNextFrame, mTimePerFrame }; }

	const unsigned int		    mColourIndexOfKoopa;

	static bool                 mUpdatedStaticVariables;

	static unsigned int         mCurrentSpriteID;
	static unsigned int         mEndSpriteID;
	static unsigned int         mStartSpriteID;

	static float                mTimeRemainingTillNextFrame;
	float					    mTimePerFrame;

	unsigned int                mHitsRemaining;
	float                       mTimeTillConversion;
	const float                 kTimeOnFloor;
};

#endif