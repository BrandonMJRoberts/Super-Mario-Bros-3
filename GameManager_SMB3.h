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

	std::string GetCurrentWorldMapFilePath();

	void Update(const float deltaTime);

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

	Vector2D     mWorldMapOffsetPoint;

	unsigned int mCurrentScoreMario;
	unsigned int mCurrentScoreLuigi;

	unsigned int mCurrentLivesMario;
	unsigned int mCurrentLivesLuigi;

	CHARACTER_MAP_POWER_UP_STATE mMarioPowerUpState;
	CHARACTER_MAP_POWER_UP_STATE mLuigiPowerUpState;

	std::vector<unsigned int> mCompletedLevelIndexes; // World Specific
	int                       mCurrentWorldIndex;

	bool					   mCurrentlyPlayingAsMario;
};

#endif
