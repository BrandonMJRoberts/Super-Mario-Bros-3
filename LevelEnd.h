#ifndef _LEVEL_END_OBJECT_H_
#define _LEVEL_END_OBJECT_H_

#include "PhysicalObject.h"

class LevelEndObject final : public PhysicalObject
{
public:
	LevelEndObject() = delete;
	LevelEndObject(const Vector2D      spawnPosition,
				   const bool          startSpawnedInLevel,
				   SDL_Renderer*       renderer,
			       const std::string   filePathToSpriteSheet,
				   const unsigned int  spritesOnWidth,
				   const unsigned int  spritesOnHeight,
				   const double        collisionBoxWidth,
				   const double        collisionBoxHeight,
				   const float		   timePerFrame,
		           const std::string   filePathForSurroundingLines);
	~LevelEndObject() override;

	BaseObject* Clone(std::string dataLine) override;

	void        ResetUpdatedStaticVariables() override { mUpdatedStaticVariables = false; }

	RenderData GetRenderData() { return RenderData{ mCurrentFrameID, mStartFrameID, mEndFrameID, mTimeRemainingTillFrameChange, mTimePerFrame }; }

	void Render(const Vector2D renderReferencePoint) override;
	bool Update(const float deltaTime, const Vector2D playerPosition, InteractableLayer* interactionLayer, ObjectLayer* objectLayer) override;

	ObjectCollisionHandleData SetIsCollidedWith(TwoDimensionalCollision collisionData, const unsigned int playerMovements) override;

private:
	void          UpdateStaticVariables(const float deltaTime);

	static bool   mUpdatedStaticVariables;

	unsigned int  mCurrentFrameID;
	unsigned int  mStartFrameID;
	unsigned int  mEndFrameID;

	float         mTimeRemainingTillFrameChange;
	float         mTimePerFrame;

	Vector2D      mCollisionBoxOffset;

	static Texture2D*    mSurroundingSprite;
	Vector2D             mSurroundingBoxPosition;

	bool		  mCollected;
	bool          mFlipFrameDirection;
};

#endif 


