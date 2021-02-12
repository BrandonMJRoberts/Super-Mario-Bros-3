#ifndef _ONEWAYWALKWAY_H_
#define _ONEWAYWALKWAY_H_

#include "PhysicalObject.h"

class OneWayWalkway : public PhysicalObject
{
public:
	OneWayWalkway(const Vector2D      spawnPosition
				, const bool          startSpawnedInLevel
				, SDL_Renderer*       renderer
				, const std::string   filePathToSpriteSheet
				, const unsigned int  spritesOnWidth
				, const unsigned int  spritesOnHeight
				, const double        collisionBoxWidth
				, const double        collsiionBoxHeight
				, const float         timePerFrame);
	~OneWayWalkway() override;

	BaseObject* Clone(std::string dataLine) override;
	RenderData  GetRenderData()             override;

	Vector2D     GetCurrentPosition()       const { return Vector2D(mCurrentPosition.x, mCurrentPosition.y - mCollisionOffsetY); }

	ObjectCollisionHandleData         SetIsCollidedWith(TwoDimensionalCollision collisionData) override;

private:
	float        mTimePerframe;
	unsigned int mCurrentFrameID;

	float        mCollisionOffsetY;

};

#endif