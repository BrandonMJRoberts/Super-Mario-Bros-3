#ifndef _GAME_MANAGER_H_
#define _GAME_MANAGER_H_

#include <vector>
#include <string>

#include "Commons_SMB3.h"

class GameManager_SMB3
{
public:
	static GameManager_SMB3* GetInstance();
	~GameManager_SMB3();

	bool LoadInSaveDataFromFile();
	void SaveDataOutToFile();

	Vector2D GetRenderReferencePoint()                { return mRenderReferencePoint;                             }
	void SetRenderReferencePoint(Vector2D newPos)     { mRenderReferencePoint   = newPos;                         }
	void SetRenderReferencePoint(double x, double y)  { mRenderReferencePoint.x = x; mRenderReferencePoint.y = y; }
	void SetRenderReferencePointX(double x)           { mRenderReferencePoint.x = x; }
	void SetRenderReferencePointY(double y)           { mRenderReferencePoint.y = y; }
	void AddToReferencePoint(Vector2D change)         { mRenderReferencePoint.x += change.x; mRenderReferencePoint.y += change.y; }

	std::string GetCurrentWorldMapFilePath();

	void SetScrollingLeft(bool newVal)                { mScrollingLeft = newVal; }
	void SetScrollingRight(bool newVal)               { mScrollingRight = newVal; }
	void SetScrollingSpeedMultiplier(float newVal)    { mScrollingSpeedMultiplier = newVal; }

	void Update(const float deltaTime);

	void SetHasHitRightBoundary(bool newVal)          { mHitRightBoundary = newVal; }
	void SetHasHitLeftBoundary(bool newVal)			  { mHitLeftBoundary = newVal; }
	void SetHasHitTopBoundary(bool newVal)			  { mHitTopBoundary = newVal; }
	void SetHasHitBottomBoundary(bool newVal)		  { mHitBottomBoundary = newVal; }

	bool GetHasHitRightBoundary()					  { return mHitRightBoundary; }
	bool GetHasHitLeftBoundary()					  { return mHitLeftBoundary; }
	bool GetHasHitTopBoundary()						  { return mHitTopBoundary; }
	bool GetHasHitBottomBoundary()					  { return mHitBottomBoundary; }

	unsigned int GetCurrentWorldIndex()				  { return mCurrentWorldIndex; }
	void IncrementCurrentWorldIndex()			      { mCurrentWorldIndex++; }

	CHARACTER_MAP_POWER_UP_STATE GetMarioCurrentPowerUpState() { return mMarioPowerUpState; }
	CHARACTER_MAP_POWER_UP_STATE GetLuigiCurrentPowerUpState() { return mLuigiPowerUpState; }

	void SetMarioPowerUpState(CHARACTER_MAP_POWER_UP_STATE newState) { mMarioPowerUpState = newState; }
	void SetLuigiPowerUpState(CHARACTER_MAP_POWER_UP_STATE newState) { mLuigiPowerUpState = newState; }

	bool GetIsCurrentlyPlayingAsMario()          { return mCurrentlyPlayingAsMario; }
	void SetIsPlayingAsMarioState(bool newState) { mCurrentlyPlayingAsMario = newState; }

	Vector2D GetWorldMapRenderOffset()           { return mWorldMapOffsetPoint; }

private:
	GameManager_SMB3();

	static GameManager_SMB3* mInstance;

	Vector2D     mRenderReferencePoint;
	Vector2D     mWorldMapOffsetPoint;

	unsigned int mCurrentScoreMario;
	unsigned int mCurrentScoreLuigi;

	unsigned int mCurrentLivesMario;
	unsigned int mCurrentLivesLuigi;

	CHARACTER_MAP_POWER_UP_STATE mMarioPowerUpState;
	CHARACTER_MAP_POWER_UP_STATE mLuigiPowerUpState;

	std::vector<unsigned int> mCompletedLevelIndexes; // World Specific
	int                       mCurrentWorldIndex;

	float					   mScrollingSpeedMultiplier;
	bool					   mScrollingRight;
	bool					   mScrollingLeft;

	bool					   mHitLeftBoundary;
	bool					   mHitRightBoundary;
	bool					   mHitTopBoundary;
	bool					   mHitBottomBoundary;

	bool					   mCurrentlyPlayingAsMario;
};

#endif
