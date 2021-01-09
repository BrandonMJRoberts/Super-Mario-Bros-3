#ifndef _PARA_KOOPA_TROOPER_SMB3_H_
#define _PARA_KOOPA_TROOPER_SMB3_H_

#include "Koopa_Trooper_SMB3.h"

class ParaKoopaTrooper final : public KoopaTrooper
{
public:
	ParaKoopaTrooper(const Vector2D      spawnPosition,
		             const bool          startSpawnedInLevel,
		             SDL_Renderer*       renderer,
		             const std::string   filePathToSpriteSheet,
					 const unsigned int  spritesOnWidth,
		             const unsigned int  spritesOnHeight,
		             const double        collisionBoxWidth,
		             const double        collisionBoxHeight,
		             const float		 timePerFrame,
		             const bool          canMove,
		             const bool          canJump,
		             const bool          startFacingLeft,
		             const char  colourIndexOfKoopa);
	~ParaKoopaTrooper() override;

	BaseObject* Clone(std::string dataLine) override;

	bool Update(const float deltaTime, const Vector2D playerPosition) override;

	void Move() override;
	void Jump() override;
	void Attack() override;

private:
};

#endif
