#ifndef _INVIS_BLOCK_H_
#define _INVIS_BLOCK_H_

#include "BlockObject.h"

class InvisibleBlock final : public BlockObject
{
public:
	InvisibleBlock(const Vector2D           spawnPosition,
				   const bool				startSpawnedInLevel,
				   SDL_Renderer*            renderer,
				   const std::string		filePathToSpriteSheet,
				   const unsigned int		spritesOnWidth,
				   const unsigned int	    spritesOnHeight,
				   const double		        collisionBoxWidth,
				   const double			    collisionBoxHeight,
				   const float				timePerFrame,
				   const unsigned int		hitsBlockCanTake,
				   const POWER_UP_TYPE      powerUpMinimumForDamage,
				   const bool               objectReleaseScales,
				   const CollectableObject* baseObjectReleased,
				   const CollectableObject* maxObjectReleased);
	~InvisibleBlock() override;

	BaseObject* Clone(std::string dataLine) override;

	bool        Update(const float deltaTime, const Vector2D playerPosition) override;
};

#endif