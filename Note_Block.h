#ifndef _NOTE_BLOCK_H_
#define _NOTE_BLOCK_H_

#include "BlockObject.h"

class NoteBlock : public BlockObject
{
public:
	NoteBlock() = delete;
	NoteBlock(const Vector2D           spawnPosition,
			  const bool               startSpawnedInLevel,
			  SDL_Renderer*            renderer,
			  const std::string        filePathToSpriteSheet,
			  const unsigned int       spritesOnWidth,
			  const unsigned int       spritesOnHeight,
			  const double             collisionBoxWidth,
			  const double             collisionBoxHeight,
			  const float		       timePerFrame,
			  const unsigned int	   hitsBlockCanTake,
			  const POWER_UP_TYPE	   powerUpMinimumForDamage,
			  const bool               objectReleaseScales,
			  const CollectableObject* baseObjectReleased,
			  const CollectableObject* maxObjectReleased);
	~NoteBlock() override;

	void Render(const Vector2D renderReferencePoint) override;

	BaseObject* Clone(std::string dataLine) override;

private:
	void        UpdateStaticVariables(const float deltaTime);

	RenderData  GetRenderData() override { return RenderData{ mCurrentSpriteID, mStartSpriteID, mEndSpriteID, mTimeRemainingTillNextFrame, mTimePerFrame }; }

	bool                        mCanTurnToCoin;
	static bool                 mUpdatedStaticVariables;

	static unsigned int         mCurrentSpriteID;
	static unsigned int         mEndSpriteID;
	static unsigned int         mStartSpriteID;

	static float                mTimeRemainingTillNextFrame;
	const float                 mTimePerFrame;
};

#endif