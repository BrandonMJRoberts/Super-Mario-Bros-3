#ifndef _ENEMY_OBJECT_H_
#define _ENEMY_OBJECT_H_

#include "PhysicalObject.h"

class EnemyObject abstract : public PhysicalObject
{
public:
	EnemyObject() = delete;
	EnemyObject(const Vector2D      spawnPosition,
				const bool          startSpawnedInLevel,
				SDL_Renderer*       renderer,
				const std::string   filePathToSpriteSheet,
				const unsigned int  spritesOnWidth,
				const unsigned int  spritesOnHeight,
				const double        collisionBoxWidth,
				const double        collisionBoxHeight,
				const float		    timePerFrame,
				const bool          canMove,
				const bool          canJump,
				const bool          startFacingLeft);
	virtual ~EnemyObject() override;

	virtual BaseObject* Clone(std::string dataForNewObject) = 0;

	virtual bool Update(const float deltaTime, const Vector2D playerPosition) override;

	virtual void Move();
	virtual void Jump();
	virtual void Attack();

protected:
	Vector2D mVelocity;

	bool mCanJump;
	bool mCanMove;
	bool mFacingLeft;
};

#endif // !_ENEMY_OBJECT_H_
