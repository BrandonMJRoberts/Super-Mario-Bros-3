#ifndef _PIPE_SMB3_H_
#define _PIPE_SMB3_H_

#include "PhysicalObject.h"

class Pipe final : public PhysicalObject
{
public:
	Pipe(const Vector2D     spawnPosition,
		const bool          startSpawnedInLevel,
		SDL_Renderer*       renderer,
		const std::string   filePathToSpriteSheet,
		const unsigned int  spritesOnWidth,
		const unsigned int  spritesOnHeight,
		const double        collisionBoxWidth,
		const double        collisionBoxHeight,
		const float		    timePerFrame);
	~Pipe() override;

	BaseObject* Clone(std::string dataLineForClone) override;

	bool        Update(const float deltaTime, const Vector2D playerPosition) override;
	void        Render(const Vector2D renderReferencePoint) override;

	void        ResetUpdatedStaticVariables() override { mUpdatedStaticVariables = false; }

private:
	static bool mUpdatedStaticVariables;

	RenderData  GetRenderData() override { return RenderData{ mCurrentSpriteID, mStartSpriteID, mEndSpriteID, mTimeRemainingTillNextFrame, mTimePerFrame }; }

	static unsigned int         mCurrentSpriteID;
	static unsigned int         mEndSpriteID;
	static unsigned int         mStartSpriteID;

	static float                mTimeRemainingTillNextFrame;
	const float                 mTimePerFrame;

	unsigned int				mWidth;
	unsigned int				mHeight;
};

#endif