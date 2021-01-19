#ifndef _BRICK_BLOCK_SMB3_H_
#define _BRICK_BLOCK_SMB3_H_

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
	bool        Update(const float deltaTime, const Vector2D playerPosition) override;

	void        Render(const Vector2D renderReferencePoint) override;

	bool        GetCanTurnToCoin()   const { return mCanTurnToCoin; }

	void        ResetUpdatedStaticVariables() override { mUpdatedStaticVariables = false; }

private:

	void        UpdateStaticVariables(const float deltaTime);

	RenderData  GetRenderData() override { return RenderData{mCurrentSpriteID, mStartSpriteID, mEndSpriteID, mTimeRemainingTillNextFrame, mTimePerFrame}; }

	bool                        mCanTurnToCoin;
	static bool                 mUpdatedStaticVariables;

	static unsigned int         mCurrentSpriteID;
	static unsigned int         mEndSpriteID;
	static unsigned int         mStartSpriteID;

	static float                mTimeRemainingTillNextFrame;
	const float                 mTimePerFrame;
};

#endif
