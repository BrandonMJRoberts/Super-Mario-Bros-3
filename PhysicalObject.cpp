#include "PhysicalObject.h"

#include "Constants_SMB3.h"

#include "InteractionLayer.h"

SDL_Renderer*			  PhysicalObject::mRenderer(nullptr);
std::vector<Texture2D*>   PhysicalObject::mSpriteSheets;
std::vector<unsigned int> PhysicalObject::mInstanceCounts;

// ----------------------------------------------------------------------------------------------------- //

PhysicalObject::PhysicalObject(const Vector2D spawnPosition
	, const bool          startSpawnedInLevel
	, SDL_Renderer*       renderer
	, const std::string   filePathToSpriteSheet
	, const unsigned int  spritesOnWidth
	, const unsigned int  spritesOnHeight
	, const double        collisionBoxWidth
	, const double        collsiionBoxHeight
	, const float         timePerFrame) 

: BaseObject(spawnPosition
, startSpawnedInLevel)
, mSingleSpriteWidth(0)
, mSingleSpriteHeight(0)
, mSpritesOnWidth(spritesOnWidth)
, mSpritesOnHeight(spritesOnHeight)
, mThisSpriteSheet(nullptr)
, mCollisionBox(collisionBoxWidth, collsiionBoxHeight)
, mFacingLeft(true)
, mVelocity(0.0f, 0.0f)
{
	if (!mRenderer)
		mRenderer = renderer;

	bool         alreadyExists   = false;
	unsigned int indexOfExisting = 0;

	// Check if we already have a sprite sheet of this loaded
	for (unsigned int i = 0; i < mSpriteSheets.size(); i++)
	{
		if (filePathToSpriteSheet == mSpriteSheets[i]->GetFilePath())
		{
			alreadyExists   = true;
			indexOfExisting = i;
			break;
		}
	}

	if (!alreadyExists)
	{
		Texture2D* newSpriteSheet = new Texture2D(renderer);
		if (!newSpriteSheet->LoadFromFile(filePathToSpriteSheet))
		{
			std::cout << "Invalid file path passed into the object loading code!" << std::endl;
			delete newSpriteSheet;
			return;
		}
		else
		{
			// Add it to the list
			mSpriteSheets.push_back(newSpriteSheet);
			mInstanceCounts.push_back(1);

			// Set this sheet to be the new one
			mThisSpriteSheet = newSpriteSheet;
		}
	}
	else
	{
		mThisSpriteSheet = mSpriteSheets[indexOfExisting];

		// Increase the instance count for this sprite sheet
		mInstanceCounts[indexOfExisting]++;
	}

	// Make sure to setup the sprite width and height
	if (mThisSpriteSheet && spritesOnWidth != 0 && spritesOnHeight != 0)
	{
		mSingleSpriteWidth  = mThisSpriteSheet->GetWidth() / spritesOnWidth;
		mSingleSpriteHeight = mThisSpriteSheet->GetHeight() / spritesOnHeight;
	}
}

// ----------------------------------------------------------------------------------------------------- //

PhysicalObject::~PhysicalObject()
{
	// Find what index we are currently on and remove one from the instance count
	for (unsigned int i = 0; i < mSpriteSheets.size(); i++)
	{
		// null checks
		if (mThisSpriteSheet && mSpriteSheets[i])
		{
			if (mThisSpriteSheet->GetFilePath() == mSpriteSheets[i]->GetFilePath())
			{
				mInstanceCounts[i]--;

				if (mInstanceCounts[i] == 0)
				{
					delete mSpriteSheets[i];
					mSpriteSheets[i] = nullptr;

					// Remove the sprite sheet from the vector
					mSpriteSheets.erase(mSpriteSheets.begin() + i);
					
					// Remove the instance count ID from the vector
					mInstanceCounts.erase(mInstanceCounts.begin() + i);
				}

				break;
			}
		}
	}

	if (mSpriteSheets.size() == 0)
		mRenderer = nullptr;
}

// ----------------------------------------------------------------------------------------------------- //

void PhysicalObject::RenderSprite(const Vector2D renderReferencePoint, const unsigned int currentFrameID)
{
	if (mThisSpriteSheet)
	{
		Vector2D renderPos = Vector2D(mCurrentPosition.x - renderReferencePoint.x, mCurrentPosition.y - renderReferencePoint.y);

		SDL_Rect portionOfSpriteSheet{ (int)(currentFrameID % mSpritesOnWidth) * (int)mSingleSpriteWidth,
									   (int)(currentFrameID / mSpritesOnWidth) * (int)mSingleSpriteHeight,
									   (int)mSingleSpriteWidth,
									   (int)mSingleSpriteHeight };

		SDL_Rect destRect           { int(renderPos.x * RESOLUTION_OF_SPRITES),
									  int(renderPos.y * RESOLUTION_OF_SPRITES) - (int)mSingleSpriteHeight + 2,
									 (int)mSingleSpriteWidth,
									 (int)mSingleSpriteHeight };

		mThisSpriteSheet->Render(portionOfSpriteSheet, destRect);
	}
}

// ----------------------------------------------------------------------------------------------------- //

bool PhysicalObject::Update(const float deltaTime, const Vector2D playerPosition, InteractableLayer* interactionLayer)
{
	// Animation update
	if (GetHasUpdatedStaticVariables())
	{
		UpdateStaticVariables(deltaTime);
	}

	// Check movement
	HandleMovement(deltaTime, interactionLayer);

	return false;
}

// ----------------------------------------------------------------------------------------------------- //

void PhysicalObject::UpdateStaticVariables(const float deltaTime)
{
	RenderData objectSpecificData = GetRenderData();

	// Update the frame time and adjust the current frame if needed
	objectSpecificData.timeRemainingTillFrameChange -= deltaTime;

	if (objectSpecificData.timeRemainingTillFrameChange <= 0.0f)
	{
		objectSpecificData.timeRemainingTillFrameChange = objectSpecificData.timePerFrame;

		objectSpecificData.currentFrameID++;

		if (objectSpecificData.currentFrameID > objectSpecificData.endFrameID)
		{
			objectSpecificData.currentFrameID = objectSpecificData.startrameID;
		}
	}
}

// ----------------------------------------------------------------------------------------------------- //

// Overarching collision handler, call this from the child classess
void PhysicalObject::HandleMovement(const float deltaTime, InteractableLayer* interactableLayer)
{
	MovementPrevention collisionData = MovementPrevention(HandleXCollision(deltaTime, interactableLayer), HandleYCollision(deltaTime, interactableLayer), false);

	// Now handle the collision data correctly
	if (collisionData.StopYMovement)
	{
		mVelocity.y = 0.0f;
	}
	else
	{
		mCurrentPosition.y += (mVelocity.y * deltaTime);
	}

	// See if the facing direction should flip
	if (collisionData.StopXMovement)
	{
		mFacingLeft  = !mFacingLeft;
		mVelocity.x *= -1;
	}
	else
	{
		mCurrentPosition.x += (mVelocity.x * deltaTime);
	}
}

// ----------------------------------------------------------------------------------------------------- //

bool PhysicalObject::CheckCollisionsWithInteractionLayer(InteractableLayer* interactionLayer, const Vector2D positionToCheck)
{
	// Check for collisions
	return (interactionLayer->GetIsCollision((unsigned int)positionToCheck.y, (unsigned int)positionToCheck.x));
}

// ----------------------------------------------------------------------------------------------------- //

// Overarching X collision handler
bool PhysicalObject::HandleXCollision(const float deltaTime, InteractableLayer* interactionLayer)
{
	// Default to checking right - taking into consideration the position, the collision box width, collision box offset and the movement distance
	Vector2D footPos = Vector2D(mCurrentPosition.x + mCollisionBox.x + (mVelocity.x * deltaTime), mCurrentPosition.y - 0.01);
	Vector2D headPos = Vector2D(mCurrentPosition.x + mCollisionBox.x + (mVelocity.x * deltaTime), mCurrentPosition.y - mCollisionBox.y - 0.01);

	// Check if we are going left - if we are then check left
	if (mVelocity.x < 0.0f)
	{
		// Going left
		footPos = Vector2D(mCurrentPosition.x + (mVelocity.x * deltaTime), mCurrentPosition.y - 0.01);
		headPos = Vector2D(mCurrentPosition.x + (mVelocity.x * deltaTime), mCurrentPosition.y - mCollisionBox.y - 0.01);
	}

	return (CheckXCollision(footPos, headPos, interactionLayer));
}

// ----------------------------------------------------------------------------------------------------- //

// Overarching Y collision handler
bool PhysicalObject::HandleYCollision(const float deltaTime, InteractableLayer* interactionLayer)
{
	// Default to going downwards
	Vector2D leftPos  = Vector2D(mCurrentPosition.x,                   mCurrentPosition.y + (mVelocity.y * deltaTime));
	Vector2D rightPos = Vector2D(mCurrentPosition.x + mCollisionBox.x, mCurrentPosition.y + (mVelocity.y * deltaTime));

	// Going downwards
	if (mVelocity.y >= 0.0f)
	{
		if (CheckYCollision(leftPos, rightPos, interactionLayer))
		{
			return true;
		}
	}
	else // Going upwards
	{
		leftPos  = Vector2D(mCurrentPosition.x,                   mCurrentPosition.y - mCollisionBox.y + (mVelocity.y * deltaTime));
		rightPos = Vector2D(mCurrentPosition.x + mCollisionBox.x, mCurrentPosition.y - mCollisionBox.y + (mVelocity.y * deltaTime));

		return (CheckYCollision(leftPos, rightPos, interactionLayer));
	}

	return false;
}

// ----------------------------------------------------------------------------------------------------- //

bool PhysicalObject::CheckXCollision(const Vector2D headPos, const Vector2D footPos, InteractableLayer* interactionLayer)
{
	// Check for collisions
	if (CheckCollisionsWithInteractionLayer(interactionLayer, headPos) || CheckCollisionsWithInteractionLayer(interactionLayer, footPos)) //||
		//HandleCollisionsWithInteractionObjectLayer(objectLayer, positionToCheck1).StopXMovement || HandleCollisionsWithInteractionObjectLayer(objectLayer, positionToCheck2).StopXMovement ||
		//positionToCheck1.x < 0.0f ||
		//positionToCheck2.x < 0.0f //||
		//positionToCheck1.x > mLevelBounds.x ||
		//positionToCheck2.x > mLevelBounds.x
		//)
	{
		// Return that there was a collision
		return true;
	}

	// Return that there was not a collision
	return false;
}

// ----------------------------------------------------------------------------------------------------- //

bool PhysicalObject::CheckYCollision(const Vector2D leftPos, const Vector2D rightPos, InteractableLayer* interactionLayer)
{
	// Check terrain collision
	if (CheckCollisionsWithInteractionLayer(interactionLayer, leftPos) || CheckCollisionsWithInteractionLayer(interactionLayer, rightPos))// ||
		//positionToCheck1.y < 0.0f ||
		//positionToCheck2.y < 0.0f ||
		//positionToCheck1.y + mCollisionBox.y > mLevelBounds.y + 2 ||
		//positionToCheck2.y + mCollisionBox.y > mLevelBounds.y + 2)
	{
		// Return that there was a collision
		return true;
	}

	//MovementPrevention returnData1 = HandleCollisionsWithInteractionObjectLayer(objectLayer, positionToCheck1), returnData2 = HandleCollisionsWithInteractionObjectLayer(objectLayer, positionToCheck2);

	//if (returnData1.StopYMovement || returnData2.StopYMovement)
	//{
	//	if (returnData1.givesJump || returnData2.givesJump)
	//		mJumpTimerLeway = 0.1f;

		// Return that there was a collision
	//	return true;
	//}

	// Return that there was not a collision
	return false;
}

// ----------------------------------------------------------------------------------------------------- //

void PhysicalObject::ApplyGravity(const float deltaTime)
{
	// Apply gravity to the object
	mVelocity.y += double(GRAVITY * deltaTime);
}