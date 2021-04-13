#include "LevelEnd.h"

#include <sstream>

#include "Constants_SMB3.h"

Texture2D* LevelEndObject::mSurroundingSprite      = nullptr;
bool       LevelEndObject::mUpdatedStaticVariables = false;

// ---------------------------------------------------------------------------------------------------------------------------------------- //

LevelEndObject::LevelEndObject(const Vector2D      spawnPosition,
							   const bool          startSpawnedInLevel,
							   SDL_Renderer*       renderer,
							   const std::string   filePathToSpriteSheet,
							   const unsigned int  spritesOnWidth,
							   const unsigned int  spritesOnHeight,
							   const double        collisionBoxWidth,
							   const double        collisionBoxHeight,
							   const float		   timePerFrame,
	                           const std::string   filePathForSurroundingLines)
	: PhysicalObject(spawnPosition, startSpawnedInLevel, renderer, filePathToSpriteSheet, spritesOnWidth, spritesOnHeight, collisionBoxWidth, collisionBoxHeight, timePerFrame)
	, mCurrentFrameID(0)
	, mStartFrameID(0)
	, mEndFrameID(2)
	, mTimeRemainingTillFrameChange(timePerFrame)
	, mTimePerFrame(timePerFrame)
	, mCollisionBoxOffset(0.25f, 0.25f)
	, mFlipFrameDirection(false)
	, mSurroundingBoxPosition(spawnPosition)
	, mCollected(false)
{
	if (mSurroundingSprite == nullptr)
	{
		mSurroundingSprite = new Texture2D(renderer);
		if (!mSurroundingSprite->LoadFromFile(filePathForSurroundingLines))
		{
			std::cout << "Failed to load in the surroundings for the end object" << std::endl;
			return;
		}
	}

	mSurroundingBoxPosition.x -= 0.3f;
	mSurroundingBoxPosition.y += 0.3f;
}

// ---------------------------------------------------------------------------------------------------------------------------------------- //

LevelEndObject::~LevelEndObject()
{
	delete mSurroundingSprite;
	mSurroundingSprite = nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------------------------- //

BaseObject* LevelEndObject::Clone(std::string dataLine)
{
	// Load in the positional data and then create a new one of these objects
	std::stringstream ssLine(dataLine);

	Vector2D newPos;
	ssLine >> newPos.x >> newPos.y;

	if (mThisSpriteSheet)
		return new LevelEndObject(newPos, true, mRenderer, mThisSpriteSheet->GetFilePath(), mSpritesOnWidth, mSpritesOnHeight, mCollisionBox.x, mCollisionBox.y, mTimePerFrame, "");
	else
		return nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------------------------- //

void LevelEndObject::Render(Vector2D renderReferencePoint)
{
	RenderSprite(renderReferencePoint, mCurrentFrameID);

	if (mSurroundingSprite)
	{
		Vector2D renderPos = Vector2D(mSurroundingBoxPosition.x - renderReferencePoint.x, mSurroundingBoxPosition.y - renderReferencePoint.y);

		SDL_Rect portionOfSpriteSheet{ 0,
									   0,
									   mSurroundingSprite->GetWidth(),
									   mSurroundingSprite->GetHeight() };

		SDL_Rect destRect{            int(renderPos.x * RESOLUTION_OF_SPRITES),
									  int(renderPos.y * RESOLUTION_OF_SPRITES) - mSurroundingSprite->GetHeight() + 1,
									  mSurroundingSprite->GetWidth(),
									  mSurroundingSprite->GetHeight() };

		mSurroundingSprite->Render(portionOfSpriteSheet, destRect);
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------------- //

bool LevelEndObject::Update(const float deltaTime, const Vector2D playerPosition, InteractableLayer* interactionLayer, ObjectLayer* objectLayer)
{
	// Move the object upwards offscreen
	if (mCollected)
	{
		mCurrentPosition.y -= 10.0 * deltaTime;
	}

	if (!mUpdatedStaticVariables)
		UpdateStaticVariables(deltaTime);

	return false;
}

// ---------------------------------------------------------------------------------------------------------------------------------------- //

void LevelEndObject::UpdateStaticVariables(const float deltaTime)
{
	mTimeRemainingTillFrameChange -= deltaTime;

	if (mTimeRemainingTillFrameChange <= 0.0f)
	{
		mTimeRemainingTillFrameChange = mTimePerFrame;

		if (mFlipFrameDirection)
		{
			mCurrentFrameID--;

			if (mCurrentFrameID == mStartFrameID)
			{
				mFlipFrameDirection = false;
			}
		}
		else
		{
			mCurrentFrameID++;
			
			if (mCollected)
			{
				if (mCurrentFrameID == mEndFrameID)
					mFlipFrameDirection = true;
			}
			else
			{
				if (mCurrentFrameID > mEndFrameID)
					mCurrentFrameID = mStartFrameID;
			}
		}

	}

	mUpdatedStaticVariables = true;
}

// ---------------------------------------------------------------------------------- //

ObjectCollisionHandleData LevelEndObject::SetIsCollidedWith(TwoDimensionalCollision collisionData, const unsigned int playerMovements, const bool isPlayer)
{
	if (!isPlayer)
		return ObjectCollisionHandleData();

	mCollected = true;

	Notify(SUBJECT_NOTIFICATION_TYPES::ADD_END_CARD, std::to_string(mCurrentFrameID));

	switch (mCurrentFrameID)
	{
	// Star
	case 0:
		mCurrentFrameID = 9;
		mEndFrameID     = 11;
		mStartFrameID   = 9;
	break;

	// Fire flower
	case 1:
		mCurrentFrameID = 6;
		mEndFrameID     = 8;
		mStartFrameID   = 6;
	break;

	// Mushroom
	case 2:
		mCurrentFrameID = 3;
		mEndFrameID     = 5;
		mStartFrameID   = 3;
	break;
	}

	Notify(SUBJECT_NOTIFICATION_TYPES::LEVEL_CLEAR, "");

	return ObjectCollisionHandleData(false, true, false, true, false, false);
}

// ---------------------------------------------------------------------------------- //