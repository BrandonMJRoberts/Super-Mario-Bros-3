#ifndef _GOOMBA_SMB3_H_
#define _GOOMBA_SMB3_H_

#include "EnemyObject.h"

class Goomba final : public EnemyObject
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
	~Goomba() override;

	BaseObject* Clone(std::string data) override;

	bool Update(const float deltaTime, const Vector2D playerPosition) override;

	void Move()   override;
	void Jump()   override;
	void Attack() override;

private:

};

#endif