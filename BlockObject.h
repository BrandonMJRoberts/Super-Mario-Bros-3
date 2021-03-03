#ifndef _BLOCK_OBJECT_H_
#define _BLOCK_OBJECT_H_

#include "PhysicalObject.h"

#include <map>

class CollectableObject;

class BlockObject abstract : public PhysicalObject
{
public:
	BlockObject() = delete;
	BlockObject(const Vector2D      spawnPosition,
		        const bool          startSpawnedInLevel,
		         SDL_Renderer*      renderer,
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
		        const CollectableObject* maxObjectReleased);

	virtual ~BlockObject() override;

	virtual BaseObject* Clone(std::string dataForNewObject) = 0;

	virtual bool        Update(const float deltaTime, const Vector2D playerPosition, InteractableLayer* interactionLayer, ObjectLayer* objectLayer) override;

	void                ReleaseCollectableObject(const POWER_UP_TYPE powerUpStateWhenHit);

protected:
	void SetupConversionTypes();
	void DeleteAllConversionObjects();

	std::map<std::string, CollectableObject*> ConvertFromStringToItemType;

	unsigned int              mHitsBlockCanTake;

	const POWER_UP_TYPE       mPowerUpTypeForDamage;

	const bool                mObjectReleasedScales;

	const CollectableObject*  mMinimumObjectReleased;
	const CollectableObject*  mMaxObjectReleased;
};

#endif // !BlockObject
