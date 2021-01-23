#include "Pipe_SMB3.h"

#include "Constants_SMB3.h"

#include <sstream>

unsigned int Pipe::mCurrentSpriteID = 0;
unsigned int Pipe::mStartSpriteID = 0;
unsigned int Pipe::mEndSpriteID = 3;

float        Pipe::mTimeRemainingTillNextFrame = 0.0f;

bool         Pipe::mUpdatedStaticVariables = false;

// ----------------------------------------------------------------------------------------- //

Pipe::Pipe(const Vector2D      spawnPosition,
	const bool          startSpawnedInLevel,
	SDL_Renderer*       renderer,
	const std::string   filePathToSpriteSheet,
	const unsigned int  spritesOnWidth,
	const unsigned int  spritesOnHeight,
	const double        collisionBoxWidth,
	const double        collisionBoxHeight,
	const float		    timePerFrame,

	const unsigned int  width,
	const unsigned int  height,
	const unsigned int  amountOfPipeEnds,
	const bool			containsAnEnemy,
	const PIPE_TYPE     pipeType,
	const FACING        facingDirection,
	const std::string   filePathToLoadTo,
	const unsigned int  thisStageEntranceID,
	const unsigned int  stageIDToGoTo,
	const bool          pipeIsVertical)

: PhysicalObject(spawnPosition
, startSpawnedInLevel
, renderer
, filePathToSpriteSheet
, spritesOnWidth
, spritesOnHeight
, collisionBoxWidth
, collisionBoxHeight
, timePerFrame)
, mTimePerFrame(timePerFrame)

, mWidth(width)
, mHeight(height)
, mAmountOfEnds(amountOfPipeEnds)
, mContainsAnEnemy(containsAnEnemy)
, mPipeType(pipeType)
, mPipeFacingDirection(facingDirection)
, mFilePathToLoadTo(filePathToLoadTo)
, mStageEntranceID(thisStageEntranceID)
, mStageEntranceIDToGoTo(stageIDToGoTo)
, mPipeIsVertical(pipeIsVertical)
{
	unsigned int spriteOffset = 0;

	switch (pipeType)
	{
	case PIPE_TYPE::BLACK_BLUE:
		spriteOffset = 0;
	break;

	case PIPE_TYPE::BLACK_WHITE:
		spriteOffset = 16;
	break;

	case PIPE_TYPE::DEFAULT_GREEN:
		spriteOffset = 4;
	break;

	case PIPE_TYPE::LIGHT_GREEN:
		spriteOffset = 20;
	break;

	case PIPE_TYPE::LIGHT_BROWN:
		spriteOffset = 32;
	break;

	case PIPE_TYPE::DARK_BROWN:
		spriteOffset = 34;
	break;

	case PIPE_TYPE::LIGHT_GREY:
		spriteOffset = 46;
	break;

	case PIPE_TYPE::DARK_GREEN:
		spriteOffset = 48;
	break;

	case PIPE_TYPE::DARK_GREY:
		spriteOffset = 60;
	break;

	case PIPE_TYPE::BRIGHT_ORANGE:
		spriteOffset = 62;
	break;

	case PIPE_TYPE::VERY_DARK_GREEN:
		spriteOffset = 74;
	break;

	case PIPE_TYPE::WHITE_GREEN:
		spriteOffset = 76;
	break;
	}

	// Make the current sprite ID point to the top left of the Pipe on the sprite sheet
	mCurrentSpriteID = spriteOffset;

	mDestRect   = { 0, 0, (int)mSingleSpriteWidth, (int)mSingleSpriteHeight};
	mSourceRect = { 0, 0, (int)mSingleSpriteWidth, (int)mSingleSpriteHeight};
}

// ----------------------------------------------------------------------------------------- //

Pipe::~Pipe()
{

}

// ----------------------------------------------------------------------------------------- //

BaseObject* Pipe::Clone(std::string dataLineForClone)
{
	// Extract the data and create a new object using it
	std::stringstream streamData(dataLineForClone);

	Vector2D newPosition, dimensions;

	unsigned int pipeType, pipeFacingDirection, amountOfEnds;
	int          thisStageEntranceID, stageEntranceIDToGoTo;
	std::string  filePathToLoadInto;
	char         newPipeIsVertical;
	bool         pipeIsDoubleEnded = false, containsEnemy = false, pipeIsVertical = true;

	std::string enemyContainedWithinPipe;

	streamData >> newPosition.x >> newPosition.y >> dimensions.x >> dimensions.y >> pipeType >> pipeFacingDirection >> filePathToLoadInto >> thisStageEntranceID >> enemyContainedWithinPipe >> amountOfEnds >> stageEntranceIDToGoTo >> newPipeIsVertical;

	if (filePathToLoadInto == "NONE")
		filePathToLoadInto = "";

	if (enemyContainedWithinPipe != "")
		containsEnemy = true;

	if (newPipeIsVertical == 'F')
		pipeIsVertical = false;

	if (mThisSpriteSheet)
		return new Pipe(newPosition, false, mRenderer, mThisSpriteSheet->GetFilePath(), mSpritesOnWidth, mSpritesOnHeight, mCollisionBox.x, mCollisionBox.y, mTimePerFrame, int(dimensions.x), int(dimensions.y), amountOfEnds, containsEnemy, PIPE_TYPE(pipeType), FACING(pipeFacingDirection), filePathToLoadInto, thisStageEntranceID, stageEntranceIDToGoTo, newPipeIsVertical);
	else
		return nullptr;
}

// ----------------------------------------------------------------------------------------- //

bool Pipe::Update(const float deltaTime, const Vector2D playerPosition)
{
	return false;
}

// ----------------------------------------------------------------------------------------- //

void Pipe::Render(const Vector2D renderReferencePoint)
{
	// First check to see what type of pipe we are rendering
	if (mPipeIsVertical)
	{
		if (mAmountOfEnds == 2)
		{
			RenderTopCovering(renderReferencePoint);

			RenderBottomCovering(renderReferencePoint);
		}
		else if(mAmountOfEnds == 1)
		{
			// First check to see if the top of the pipe should have a covering
			if (mPipeFacingDirection == FACING::UP)
			{
				RenderTopCovering(renderReferencePoint);
			}
			else if (mPipeFacingDirection == FACING::DOWN)
			{
				RenderBottomCovering(renderReferencePoint);
			}
		}
	}
	else
	{
		if (mAmountOfEnds == 2)
		{
			RenderLeftCovering(renderReferencePoint);

			RenderRightCovering(renderReferencePoint);
		}
		else if(mAmountOfEnds == 1)
		{
			if(mPipeFacingDirection == FACING::RIGHT)
			{
				RenderRightCovering(renderReferencePoint);
			}
			else if (mPipeFacingDirection == FACING::RIGHT)
			{
				RenderRightCovering(renderReferencePoint);
			}
		}
	}

	RenderCentreOfPipe(renderReferencePoint);
}

// ----------------------------------------------------------------------------------------- //

void Pipe::ResetRenderPositionToTopLeft(const Vector2D renderReferencePoint)
{
	mDestRect.x = int((mCurrentPosition.x - renderReferencePoint.x) * RESOLUTION_OF_SPRITES);
	mDestRect.y = int((mCurrentPosition.y - 1 - renderReferencePoint.y) * RESOLUTION_OF_SPRITES);
}

// ----------------------------------------------------------------------------------------- //

void Pipe::ResetSourcePositionToTopLeftOfPipe()
{
	mSourceRect = {  int(mCurrentSpriteID % mSpritesOnWidth) * int(mSingleSpriteWidth),
					 int(mCurrentSpriteID / mSpritesOnWidth) * int(mSingleSpriteHeight),
					 int(mSingleSpriteWidth),
					 int(mSingleSpriteHeight) };
}

// ----------------------------------------------------------------------------------------- //

void Pipe::RenderTopCovering(const Vector2D renderReferencePoint)
{
	// First set the sprite data to being the correct place
	ResetSourcePositionToTopLeftOfPipe();

	ResetRenderPositionToTopLeft(renderReferencePoint);

	// First render the top portion of the pipe
	RenderPortionSelected();

	// Now shift it along and render the other part of the end
	mSourceRect.x = ((mCurrentSpriteID + 1) % mSpritesOnWidth) * mSingleSpriteWidth;
	mDestRect.x  += mSingleSpriteWidth;

	RenderPortionSelected();
}

// ----------------------------------------------------------------------------------------- //

void Pipe::RenderBottomCovering(const Vector2D renderReferencePoint)
{
	// Now render the bottom covering of the pipe
	mDestRect.y = int((mCurrentPosition.y - renderReferencePoint.y + (mHeight - 1)) * RESOLUTION_OF_SPRITES);
	mDestRect.x = int((mCurrentPosition.x - renderReferencePoint.x) * RESOLUTION_OF_SPRITES);

	ResetSourcePositionToTopLeftOfPipe();

	RenderPortionSelected(true, false);

	// Now move accross and render the other portion
	mDestRect.x += mSingleSpriteWidth;

	mSourceRect.x = ((mCurrentSpriteID + 1) % mSpritesOnWidth) * mSingleSpriteWidth;

	RenderPortionSelected(true, false);
}

// ----------------------------------------------------------------------------------------- //

void Pipe::RenderLeftCovering(const Vector2D renderReferencePoint)
{
	mSourceRect = { int(((mCurrentSpriteID + 2) % mSpritesOnWidth) * mSingleSpriteWidth),
					int(((mCurrentSpriteID + 2) / mSpritesOnWidth) * mSingleSpriteHeight),
					int(mSingleSpriteWidth),
					int(mSingleSpriteHeight) };

	ResetRenderPositionToTopLeft(renderReferencePoint);

	RenderPortionSelected();

	mDestRect.y   += mSingleSpriteHeight;
	mSourceRect.y += mSingleSpriteHeight;

	RenderPortionSelected();
}

// ----------------------------------------------------------------------------------------- //

void Pipe::RenderRightCovering(const Vector2D renderReferencePoint)
{
	mSourceRect = { int(((mCurrentSpriteID + 2) % mSpritesOnWidth) * mSingleSpriteWidth),
				    int(((mCurrentSpriteID + 2) / mSpritesOnWidth) * mSingleSpriteHeight),
					int(mSingleSpriteWidth),
					int(mSingleSpriteHeight) };

	mDestRect.x = int((mCurrentPosition.x - renderReferencePoint.x + (mWidth - 1)) * RESOLUTION_OF_SPRITES);
	mDestRect.y = int((mCurrentPosition.y - renderReferencePoint.y) * RESOLUTION_OF_SPRITES);

	RenderPortionSelected(false, true);

	// Now shift accross and render the other portion
	mSourceRect.y += mSingleSpriteHeight;
	mDestRect.y   += mSingleSpriteHeight;

	RenderPortionSelected(false, true);
}

// ----------------------------------------------------------------------------------------- //

void Pipe::RenderCentreOfPipe(const Vector2D renderReferencePoint)
{
	// First calculate the distance the centre needs to go
	unsigned int distance, offset;

	if (mPipeIsVertical)
	{
		if (mAmountOfEnds == 0)
			distance = mHeight;
		else
			distance = mHeight - 1;
	}
	else
	{
		if (mAmountOfEnds == 0)
			distance = mWidth;
		else
			distance = mWidth - 1;
	}

	// Check to see if it is double ended as this changes the range rendered
	if (mAmountOfEnds == 2)
	{
		distance--;
	}

	switch (mPipeFacingDirection)
	{
	case FACING::UP:
		mSourceRect.y = int((mCurrentSpriteID + 8) / mSpritesOnWidth) * mSingleSpriteHeight;

		offset = 0;

		while (offset < distance)
		{
			mSourceRect.x = ((mCurrentSpriteID + 8) % mSpritesOnWidth) * mSingleSpriteWidth;

			// Left side
			mDestRect.x = int((mCurrentPosition.x - renderReferencePoint.x) * RESOLUTION_OF_SPRITES);
			mDestRect.y = int((mCurrentPosition.y - renderReferencePoint.y + offset) * RESOLUTION_OF_SPRITES);

			RenderPortionSelected();

			mSourceRect.x = ((mCurrentSpriteID + 9) % mSpritesOnWidth) * mSingleSpriteWidth;

			// Right side
			mDestRect.x = int((mCurrentPosition.x - renderReferencePoint.x + 1) * RESOLUTION_OF_SPRITES);
			mDestRect.y = int((mCurrentPosition.y - renderReferencePoint.y + offset) * RESOLUTION_OF_SPRITES);

			RenderPortionSelected();

			offset++;
		}

		return;
	break;

	case FACING::DOWN:
		mSourceRect.y = int((mCurrentSpriteID + 8) / mSpritesOnWidth) * mSingleSpriteHeight;

		offset = 0;

		while (offset < distance)
		{
			mSourceRect.x = ((mCurrentSpriteID + 8) % mSpritesOnWidth) * mSingleSpriteWidth;

			// Left side
			mDestRect.x = int((mCurrentPosition.x - renderReferencePoint.x) * RESOLUTION_OF_SPRITES);
			mDestRect.y = int((mCurrentPosition.y - renderReferencePoint.y + offset) * RESOLUTION_OF_SPRITES);

			RenderPortionSelected();

			mSourceRect.x = ((mCurrentSpriteID + 9) % mSpritesOnWidth) * mSingleSpriteWidth;

			// Right side
			mDestRect.x = int((mCurrentPosition.x - renderReferencePoint.x + 1) * RESOLUTION_OF_SPRITES);
			mDestRect.y = int((mCurrentPosition.y - renderReferencePoint.y + offset) * RESOLUTION_OF_SPRITES);

			RenderPortionSelected();

			offset++;
		}
		return;
	break;

	case FACING::LEFT:
		offset = 0;

		while (offset < distance)
		{
			mSourceRect.x = ((mCurrentSpriteID + 3) % mSpritesOnWidth) * mSingleSpriteWidth;
			mSourceRect.y = int((mCurrentSpriteID + 3) / mSpritesOnWidth) * mSingleSpriteHeight;

			// Left side
			mDestRect.x = int((mCurrentPosition.x - renderReferencePoint.x + offset) * RESOLUTION_OF_SPRITES);
			mDestRect.y = int((mCurrentPosition.y - renderReferencePoint.y)           * RESOLUTION_OF_SPRITES);

			RenderPortionSelected();

			mSourceRect.x = ((mCurrentSpriteID + 11) % mSpritesOnWidth) * mSingleSpriteWidth;
			mSourceRect.y = int((mCurrentSpriteID + 11) / mSpritesOnWidth) * mSingleSpriteHeight;

			// Right side
			mDestRect.x = int((mCurrentPosition.x - renderReferencePoint.x + offset)  * RESOLUTION_OF_SPRITES);
			mDestRect.y = int((mCurrentPosition.y - renderReferencePoint.y + 1)        * RESOLUTION_OF_SPRITES);

			RenderPortionSelected();

			offset++;
		}
		return;
	break;

	case FACING::RIGHT:
		offset = 0;

		while (offset < distance)
		{
			mSourceRect.x =    ((mCurrentSpriteID + 3) % mSpritesOnWidth) * mSingleSpriteWidth;
			mSourceRect.y = int((mCurrentSpriteID + 3) / mSpritesOnWidth) * mSingleSpriteHeight;

			// Left side
			mDestRect.x = int((mCurrentPosition.x - renderReferencePoint.x + offset) * RESOLUTION_OF_SPRITES);
			mDestRect.y = int((mCurrentPosition.y - renderReferencePoint.y)           * RESOLUTION_OF_SPRITES);

			RenderPortionSelected(false, true);

			mSourceRect.x =    ((mCurrentSpriteID + 11) % mSpritesOnWidth) * mSingleSpriteWidth;
			mSourceRect.y = int((mCurrentSpriteID + 11) / mSpritesOnWidth) * mSingleSpriteHeight;

			// Right side
			mDestRect.x = int((mCurrentPosition.x - renderReferencePoint.x + offset) * RESOLUTION_OF_SPRITES);
			mDestRect.y = int((mCurrentPosition.y - renderReferencePoint.y + 1) * RESOLUTION_OF_SPRITES);

			RenderPortionSelected(false, true);

			offset++;
		}

		return;
	break;
	}

}

// ----------------------------------------------------------------------------------------- //

void Pipe::RenderPortionSelected(bool flippedInY, bool flippedInX)
{
	if (mThisSpriteSheet)
	{
		if (flippedInY)
		{
			if (flippedInX)
			{
				mThisSpriteSheet->Render(mSourceRect, mDestRect, SDL_FLIP_HORIZONTAL, 0.0f);
			}
			else
			{
				mThisSpriteSheet->Render(mSourceRect, mDestRect, SDL_FLIP_VERTICAL, 0.0f);
			}
		}
		else
		{
			if (flippedInX)
			{
				mThisSpriteSheet->Render(mSourceRect, mDestRect, SDL_FLIP_HORIZONTAL, 0.0f);
			}
			else
			{
				mThisSpriteSheet->Render(mSourceRect, mDestRect, SDL_FLIP_NONE, 0.0f);
			}
		}
	}
}

// ----------------------------------------------------------------------------------------- //