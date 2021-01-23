#ifndef _PIPE_SMB3_H_
#define _PIPE_SMB3_H_

#include "PhysicalObject.h"

class Pipe final : public PhysicalObject
{
public:
	Pipe(const Vector2D     spawnPosition,
		const bool          startSpawnedInLevel,
		SDL_Renderer* renderer,
		const std::string   filePathToSpriteSheet,
		const unsigned int  spritesOnWidth,
		const unsigned int  spritesOnHeight,
		const double        collisionBoxWidth,
		const double        collisionBoxHeight,
		const float		    timePerFrame,

		const unsigned int  width,
		const unsigned int  height,
		const bool          doubleEnded,
		const bool			containsAnEnemy,
		const PIPE_TYPE     pipeType,
		const FACING        facingDirection,
		const std::string   filePathToLoadTo,
		const unsigned int  thisStageEntranceID,
		const unsigned int  stageIDToGoTo,
		const bool          pipeIsVertical = true);
	~Pipe() override;

	BaseObject* Clone(std::string dataLineForClone) override;

	bool        Update(const float deltaTime, const Vector2D playerPosition) override;
	void        Render(const Vector2D renderReferencePoint) override;

	void        ResetUpdatedStaticVariables() override { mUpdatedStaticVariables = false; }

private:
	void        RenderPortionSelected(bool flippedInY = false, bool flippedInX = false);

	void		RenderTopCovering(const Vector2D renderReferencePoint);
	void		RenderBottomCovering(const Vector2D renderReferencePoint);
	void		RenderLeftCovering(const Vector2D renderReferencePoint);
	void		RenderRightCovering(const Vector2D renderReferencePoint);

	void        RenderCentreOfPipe(const Vector2D renderReferencePoint);

	void        ResetRenderPositionToTopLeft(const Vector2D renderReferencePoint);

	static bool mUpdatedStaticVariables;

	RenderData  GetRenderData() override { return RenderData{ mCurrentSpriteID, mStartSpriteID, mEndSpriteID, mTimeRemainingTillNextFrame, mTimePerFrame }; }

	static unsigned int         mCurrentSpriteID;
	static unsigned int         mEndSpriteID;
	static unsigned int         mStartSpriteID;

	static float                mTimeRemainingTillNextFrame;
	const float                 mTimePerFrame;


	const unsigned int			mWidth;
	const unsigned int			mHeight;

	const unsigned int          mStageEntranceID;
	const unsigned int          mStageEntranceIDToGoTo;

	SDL_Rect                    mSourceRect;
	SDL_Rect                    mDestRect;

	const std::string           mFilePathToLoadTo;

	const PIPE_TYPE				mPipeType;
	const FACING                mPipeFacingDirection;

	const bool					mIsDoubleEnded;
	const bool					mContainsAnEnemy;
	const bool                  mPipeIsVertical;
};

#endif