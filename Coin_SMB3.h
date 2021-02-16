#ifndef _COIN_SMB3_H_
#define _COIN_SMB3_H_

#include "CollectableObject.h"

struct SDL_Renderer;

class Coin_SMB3 final : public CollectableObject
{
public:
	Coin_SMB3() = delete;
	Coin_SMB3(const Vector2D     spawnPosition, 
		      const bool         startSpawnedInLevel, 
		      SDL_Renderer*      renderer, 
		      const char* const  filePathToSpriteSheet, 
		      const unsigned int amountOfSpritesOnWidth, 
		      const unsigned int amountOfSpritesOnHeight,
			  const double       collisionBoxWidth,
		      const double       collisionBoxHeight,
		      const bool         collectableCanMove,
		      const float        timePerAnimationFrame);
	~Coin_SMB3() override;

	void Render(const Vector2D renderReferencePoint) override;
	bool Update(const float deltaTime, const Vector2D playerPosition, InteractableLayer* interactionLayer) override;

	virtual BaseObject* Clone(std::string dataLine) override;

	void        ResetUpdatedStaticVariables() override { mUpdatedStaticVariables = false; }

	ObjectCollisionHandleData        SetIsCollidedWith(TwoDimensionalCollision collisionData) override { Notify(SUBJECT_NOTIFICATION_TYPES::COIN_COLLECTED, ""); return ObjectCollisionHandleData(true, false, false, false, false); }

private:
	void        UpdateStaticVariables(const float deltaTime);

	RenderData  GetRenderData() override { return RenderData{ mCurrentSpriteID, mStartSpriteID, mEndSpriteID, mTimeRemainingTillNextFrame, mTimePerFrame }; }

	static bool                 mUpdatedStaticVariables;

	static unsigned int         mCurrentSpriteID;
	static unsigned int         mEndSpriteID;
	static unsigned int         mStartSpriteID;

	static float                mTimeRemainingTillNextFrame;
	const float                 mTimePerFrame;
};

#endif