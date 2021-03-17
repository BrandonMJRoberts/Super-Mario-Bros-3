#include "GameScreenLevel_SMB3.h"

#include "GameManager_SMB3.h"
#include "Constants_SMB3.h"
#include "LevelArea.h"
#include "Commons_SMB3.h"

#include "PlayableCharacter.h"

#include "Observer.h"

#include "VisualTransitionHandler.h"

#include <SDL.h>
#include <filesystem>
#include <iostream>

// --------------------------------------------------------------------------------------------------------------------------- //

GameScreenLevel_SMB3::GameScreenLevel_SMB3(SDL_Renderer* renderer
	, const char* levelFilePath
	, const bool  playingAsMario
	, Observer&   HUD_Observer
    , LEVEL_TYPE  levelType
    , Audio_Player* audioPlayerRef
    , Observer*     hudObserver)

	: GameScreen_SMB3(renderer, audioPlayerRef)
	, mCurrentLevelAreaID(0)    // Default the level area to being zero in case there is not one named 'Overworld'
	, mPlayer(nullptr)
	, mTransitionHandler(nullptr)
{
	AddObserver(hudObserver);

	// Setup the lookup table
	InitialiseLookUpTable();

	// Create the transition handler
	mTransitionHandler = new VisualTransitionHandler(nullptr, renderer);

	if(mTransitionHandler)
		AddObserver(mTransitionHandler);

	// Setup the vector of observes
	std::vector<Observer*> observers = { (Observer*)audioPlayerRef, hudObserver, mTransitionHandler};

	// First loop through all folders contained within the file path presented, each folder is a different level area
	// To do this you use the std::filesystem include
	// This is specifically a c++17 feature (it is experimental in c++14)
	for (const auto& entry : std::filesystem::directory_iterator(levelFilePath))
	{
		bool thisIsStartingArea = false;

		// Now we have the file path and the amount of folders then we can setup the level areas
		mAreas.push_back(new LevelAreas(entry.path().u8string().c_str(), thisIsStartingArea, renderer, mConversionFromCharToIntIndexMap, observers));

		// If this is the starting area then set this to be the current index
		if (thisIsStartingArea)
		{
			mCurrentLevelAreaID = mAreas.size() - 1;
			Notify(SUBJECT_NOTIFICATION_TYPES::SETUP_MAIN_LEVEL, mAreas[mAreas.size() - 1]->GetNameOfArea());
		}
		else
		{
			Notify(SUBJECT_NOTIFICATION_TYPES::SETUP_SUB_LEVEL, mAreas[mAreas.size() - 1]->GetNameOfArea());
		}
	}

	// Now create the player that will be in these levels
	if (playingAsMario)
	{
		mPlayer = new PlayableCharacter(renderer, "SDL_Mario_Project/Characters/Mario/In Game Mario/SmallMarioSpriteSheet.png", mAreas[mCurrentLevelAreaID]->GetInitialSpawnPoint(),  mAreas[mCurrentLevelAreaID]->GetLevelBounds(), 0.12f);
		mPlayer->AddObserver(&HUD_Observer);
		mPlayer->AddObserver((Observer*)audioPlayerRef);
	}
	else
	{
		mPlayer = new PlayableCharacter(renderer, "SDL_Mario_Project/Characters/Luigi/In Game Luigi/SmallLuigiSpriteSheet.png", mAreas[mCurrentLevelAreaID]->GetInitialSpawnPoint(), mAreas[mCurrentLevelAreaID]->GetLevelBounds(), 0.12f);
		mPlayer->AddObserver(&HUD_Observer);
		mPlayer->AddObserver((Observer*)audioPlayerRef);
	}

	// Add the current player to the handler
	if (mTransitionHandler)
	{
		mTransitionHandler->SetCurrentPlayer(mPlayer);
	}

	Notify(SUBJECT_NOTIFICATION_TYPES::PLAY_MAIN_LEVEL_MUSIC, "");
}

// --------------------------------------------------------------------------------------------------------------------------- //

GameScreenLevel_SMB3::~GameScreenLevel_SMB3()
{
	for (unsigned int i = 0; i < mAreas.size(); i++)
		delete mAreas[i];

	mAreas.clear();

	delete mPlayer;
	mPlayer = nullptr;

	delete mTransitionHandler;
	mTransitionHandler = nullptr;
}

// --------------------------------------------------------------------------------------------------------------------------- //

void GameScreenLevel_SMB3::Render()
{
	// Render the current area we are in
	if (mAreas.size() > mCurrentLevelAreaID && mPlayer)
	{
		Vector2D refPoint = mPlayer->GetRenderReferencePoint();

		if (mTransitionHandler->GetIsPipeTransition() || mPlayer->GetIsExitingPipe())
		{
			mAreas[mCurrentLevelAreaID]->RenderBackground(refPoint);
			mAreas[mCurrentLevelAreaID]->RenderGround(refPoint);

			// Render the player
			mPlayer->Render();

			mAreas[mCurrentLevelAreaID]->RenderObjects(refPoint);
		}
		else
		{
			mAreas[mCurrentLevelAreaID]->RenderAll(refPoint);

			// Render the player
			mPlayer->Render();
		}

		if (mTransitionHandler)
			mTransitionHandler->Render();
	}
	else
	{
		std::cout << "Render Error: The current area does not exist." << std::endl;
	}
}

// --------------------------------------------------------------------------------------------------------------------------- //

ReturnDataFromGameScreen GameScreenLevel_SMB3::Update(const float deltaTime, SDL_Event e)
{
	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case SDLK_p:
			return ReturnDataFromGameScreen(SCREENS_SMB3::WORLD_MAP, "NOT_COMPLETE");
		}
	break;
	}

	if(mTransitionHandler)
		mTransitionHandler->Update(deltaTime);

	if (mPlayer)
		mPlayer->Update(deltaTime, e, mAreas[mCurrentLevelAreaID]->GetLevelBounds(), mAreas[mCurrentLevelAreaID]->GetInteractionLayer(), mAreas[mCurrentLevelAreaID]->GetObjectLayer());

	// Update the area we are currently in if it exists
	if (mAreas.size() > mCurrentLevelAreaID)
	{
		Area_Transition_Data returnData = mAreas[mCurrentLevelAreaID]->Update(deltaTime, e, mPlayer);

		// Check to see if the level has been completed
		if (returnData.goToWorldMap || (mPlayer && mPlayer->GetShouldDeathTransition()))
		{
			if(returnData.LevelComplete)
				return ReturnDataFromGameScreen(SCREENS_SMB3::WORLD_MAP, "COMPLETE");
			else
				return ReturnDataFromGameScreen(SCREENS_SMB3::WORLD_MAP, "NOT_COMPLETE");
		}

		// Handle the area transition
		TransitionToNewArea(returnData, false);

		// Now check with the transition handler to see if it wants the scene to swap
		returnData = mTransitionHandler->GetTransitionData();

		// Handle the area transition
		TransitionToNewArea(returnData, true);
	}
	else
	{
		std::cout << "Update Error: The current area does not exist. Returning to the main menu." << std::endl;
		return ReturnDataFromGameScreen(SCREENS_SMB3::MAIN_MENU, "");
	}

	return ReturnDataFromGameScreen();
}

// --------------------------------------------------------------------------------------------------------------------------- //

void GameScreenLevel_SMB3::TransitionToNewArea(Area_Transition_Data data, bool pipeTransition)
{
	// -1 means stay in the same area we currently are
	if (data.areaToGoTo != -1)
	{
		mCurrentLevelAreaID = data.areaToGoTo;

		if(mPlayer)
			mPlayer->SpawnIntoNewArea(mAreas[mCurrentLevelAreaID]->GetSpawnPointPosition(data.spawnpointIDToGoTo), mAreas[mCurrentLevelAreaID]->GetLevelBounds(), pipeTransition);

		// Notify the change of music
		if (mCurrentLevelAreaID == 0)
			Notify(SUBJECT_NOTIFICATION_TYPES::PLAY_MAIN_LEVEL_MUSIC, "");
		else
			Notify(SUBJECT_NOTIFICATION_TYPES::PLAY_SUB_AREA_MUSIC, "");
	}
}

// --------------------------------------------------------------------------------------------------------------------------- //

void GameScreenLevel_SMB3::InitialiseLookUpTable()
{
	char valueToConvert = '/';
	int  intValue;

	while (valueToConvert != 'z')
	{
		valueToConvert++;

		intValue = int(valueToConvert);

		if (intValue > 47 && intValue < 58)
		{
			mConversionFromCharToIntIndexMap[valueToConvert] = intValue - 48;
		}
		else if (intValue > 64 && intValue < 91)
		{
			mConversionFromCharToIntIndexMap[valueToConvert] = intValue - 55;
		}
		else if (intValue > 96 && intValue < 123)
		{
			mConversionFromCharToIntIndexMap[valueToConvert] = intValue - 61;
		}
		else if (intValue >= 58 && intValue <= 64)
		{
			valueToConvert += 6;
			continue;
		}
		else
		{
			valueToConvert += 5;
			continue;
		}
	}

	// Now we need to add the index for the hyphon
	mConversionFromCharToIntIndexMap['-'] = 150;
}

// --------------------------------------------------------------------------------------------------------------------------- //

void GameScreenLevel_SMB3::HandleInput(const float deltaTime, SDL_Event e)
{
	
}

// --------------------------------------------------------------------------------------------------------------------------- //