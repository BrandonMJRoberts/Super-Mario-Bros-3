#ifndef _BRICK_BLOCK_H_
#define _BRICK_BLOCK_H_

#include "BlockObject.h"

class BrickBlock final : public BlockObject
{
public:
	BrickBlock( const Vector2D      spawnPosition,
				const bool          startSpawnedInLevel,
				SDL_Renderer*       renderer,
				const std::string   filePathToSpriteSheet,
				const unsigned int  spritesOnWidth,
				const unsigned int  spritesOnHeight,
				const double        collisionBoxWidth,
				const double        collisionBoxHeight,
				const float		    timePerFrame,
				const unsigned int  hitsBlockCanTake,
				const POWER_UP_TYPE powerUpMinimumForDamage,
				const bool          objectReleaseScales,
				const CollectableObject* baseObjectReleased,
				const CollectableObject* maxObjectReleased,
		        const bool               canTurnToCoin);
	~BrickBlock() override;

	BaseObject* Clone(std::string dataForNewObject);

	bool GetCanTurnToCoin() const { return mCanTurnToCoin; }

private:
	bool mCanTurnToCoin;

};

#endif
