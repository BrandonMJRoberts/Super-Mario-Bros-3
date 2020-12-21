#include "GameManager_SMB3.h"

GameManager_SMB3* GameManager_SMB3::mInstance = nullptr;

#include <iostream>
#include <fstream>
#include <sstream>

#include "Constants_SMB3.h"

// ---------------------------------------------------------------------- //

GameManager_SMB3::GameManager_SMB3()
{
	mCurrentLivesMario = 5;
	mCurrentLivesLuigi = 5;

	mCurrentScoreLuigi = 0;
	mCurrentScoreMario = 0;

	mCurrentWorldIndex  = 1; // Start in world 1

	mRenderReferencePoint = Vector2D();
	mWorldMapOffsetPoint  = Vector2D(1, 1);

	mScrollingRight = false;
	mScrollingLeft  = false;
	mScrollingSpeedMultiplier = 5.0f;

	mHitRightBoundary  = false;
	mHitLeftBoundary   = false;
	mHitTopBoundary    = false;
	mHitBottomBoundary = false;

	mMarioPowerUpState = CHARACTER_MAP_POWER_UP_STATE::SMALL;
	mLuigiPowerUpState = CHARACTER_MAP_POWER_UP_STATE::SMALL;

	mCurrentlyPlayingAsMario = true;
}

// ---------------------------------------------------------------------- //

GameManager_SMB3::~GameManager_SMB3()
{
	delete mInstance;
	mInstance = nullptr;
}

// ---------------------------------------------------------------------- //

GameManager_SMB3* GameManager_SMB3::GetInstance()
{
	if (!mInstance)
		mInstance = new GameManager_SMB3();

	return mInstance;
}

// ---------------------------------------------------------------------- //

bool GameManager_SMB3::LoadInSaveDataFromFile()
{
	std::fstream dataFile;
	std::string dataFromFile;
	//unsigned int placeHolderInt;

	dataFile.open("SDL_Mario_Project/Saves/SaveData.txt", std::ios::in);
	if (dataFile.good())
	{
		// The data is formatted like this: World Gotten to, Mario lives, Luigi lives, Mario score, Luigi score, 
		dataFile >> dataFromFile;

		if (dataFromFile == "")
		{
			std::cout << "The save file does not contain the correct data, or it is formatted incorrectly" << std::endl;
			return false;
		}
		else
		{
			std::stringstream ss(dataFromFile);
			ss >> mCurrentWorldIndex; // World
			ss >> mCurrentLivesMario; // Mario lives
			ss >> mCurrentLivesLuigi; // Luigi lives
			ss >> mCurrentScoreMario; // Mario score
			ss >> mCurrentScoreLuigi; // luigi score
			
		}
	}
	else
	{
		std::cout << "Failed to open save file, doesn't exist, or is not in the correct file directory, or is not called 'SaveData.txt'." << std::endl;
		return false;
	}

	// If there are no errors then return that it succeeded
	std::cout << "Read in the save data correctly, loading into the world now." << std::endl;
	return true;
}

// ---------------------------------------------------------------------- //

void GameManager_SMB3::SaveDataOutToFile()
{
	std::ofstream saveFile("SDL_Mario_Project/Saves/SaveData.txt");

	if (!saveFile)
	{
		std::cout << "Failed to create the save data file. " << std::endl;
		return;
	}

	saveFile << mCurrentWorldIndex << " " << mCurrentLivesLuigi << " " << mCurrentLivesLuigi << " " << mCurrentScoreMario << " " << mCurrentScoreLuigi;

	saveFile.close();
}

// ---------------------------------------------------------------------- //

std::string GameManager_SMB3::GetCurrentWorldMapFilePath()
{
	// -1 is the warp world
	if (mCurrentWorldIndex != -1)
	{
		return "SDL_Mario_Project/Worlds/World " + mCurrentWorldIndex;
	}
	else
	{
		return "SDL_Mario_Project/Worlds/Warp World";
	}
}

// ---------------------------------------------------------------------- //

void GameManager_SMB3::Update(const float deltaTime)
{
	if (mScrollingLeft)
	{
		AddToReferencePoint(Vector2D(-RESOLUTION_OF_SPRITES * (double)deltaTime * mScrollingSpeedMultiplier, 0));
	}
	else if (mScrollingRight)
	{
		AddToReferencePoint(Vector2D(RESOLUTION_OF_SPRITES * (double)deltaTime * mScrollingSpeedMultiplier, 0));
	}
}

// ---------------------------------------------------------------------- //