#ifndef _COLLECTABLE_OBJECT_BASE_H_
#define _COLLECTABLE_OBJECT_BASE_H_

#include "PhysicalObject.h"

class CollectableObject abstract : public PhysicalObject
{
public:
	CollectableObject() = delete;
	CollectableObject(const Vector2D       spawnPosition,
		              const bool           startSpawnedInLevel,
		              SDL_Renderer*        renderer,
					  const std::string    filePathToSpriteSheet,
					  const unsigned int   spritesOnWidth,
					  const unsigned int   spritesOnHeight,
				      const double         collisionBoxWidth,
					  const double         collisionBoxHeight,
					  const float		   timePerFrame,
		              const bool           collectableCanMove);
	virtual ~CollectableObject() override;

	virtual BaseObject* Clone(std::string dataForNewObject) = 0;

	virtual bool Update(const float deltaTime, const Vector2D playerPosition, InteractableLayer* interactionLayer) override;

	virtual void Move();
	virtual void Jump();

	ObjectCollisionHandleData SetIsCollidedWith(TwoDimensionalCollision collisionData, const unsigned int playerMovements) override { return ObjectCollisionHandleData(true, false, false, false, false); }

protected:
	Vector2D    mVelocity; // velocity for the movement of the collectable

	bool        mCanMove;
	bool        mCanJump;
	bool        mFacingLeft;
};

#endif