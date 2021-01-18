#ifndef _BASE_WORLD_MAP_CHARACTER_H_
#define _BASE_WORLD_MAP_CHARACTER_H_

#include <string>

#include "Commons_SMB3.h"
#include "Constants_SMB3.h"
#include "Texture2D.h"

struct SDL_Renderer;
class NodeMap_WorldMap;

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

class BaseWorldMapCharacter abstract
{
public:
	BaseWorldMapCharacter() = delete;
	BaseWorldMapCharacter(SDL_Renderer* renderer, const std::string filePathToSpriteSheet, const Vector2D startPosition, const unsigned int spritesOnWidth, const unsigned int spritesOnHeight, const float timePerAnimationFrame);
	virtual ~BaseWorldMapCharacter();

	void      ChangePowerUpState(CHARACTER_MAP_POWER_UP_STATE newState);

	virtual void Render();
	virtual void Update(const float deltaTime, NodeMap_WorldMap& nodeMapRef, SDL_Event e);

	Vector2D     GetGridPosition()                      { return mPosition; }
	Vector2D     GetRealdPosition()                     { return (mPosition * RESOLUTION_OF_SPRITES); }

	bool         GetIsMoving()                    const { if (mMovementDirection == MOVEMENT_DIRECTION::NONE) return false; else return true; }
	MOVEMENT_DIRECTION GetMovementDirection()     const { return mMovementDirection; }

	void SetMoveToPosition(const Vector2D newPos)       { mMoveToPosition = newPos; }
	void ResetMovementDirection()                       { mMovementDirection = MOVEMENT_DIRECTION::NONE; }

protected:
	void CheckForMovementInput(SDL_Event e);
	bool CanTurnToDirection(MOVEMENT_DIRECTION newDir, NodeMap_WorldMap& nodeMapRef);

	void SetNewMoveToPosition();

	Texture2D*                   mSpriteSheet;

	Vector2D                     mPosition;
	Vector2D                     mMoveToPosition;

	unsigned int                 mAmountOfSpritesOnWidth;
	unsigned int                 mAmountOfSpritesOnHeight;
	unsigned int                 mSingleSpriteWidth;
	unsigned int                 mSingleSpriteHeight;

	unsigned int                 mCurrentFrame;
	unsigned int                 mStartFrame;
	unsigned int                 mEndFrame;

	MOVEMENT_DIRECTION           mMovementDirection;
	MOVEMENT_DIRECTION           mRequestedMovementDirection;

	float                        mTimeRemainingTillFrameChange;
	const float                  mTimePerAnimationFrame;
	float                        mTimeTillNextMove;

	CHARACTER_MAP_POWER_UP_STATE mCurrentPowerUpState;

	bool                         mButtonIsPressed;
};

#endif // !_BASE_WORLD_MAP_CHARACTER_H_
