#include "GameScreenManager_SMB3.h"

#include "GameScreenLevel_SMB3.h"
#include "GameScreenMainMenu_SMB3.h"
#include "GameScreenWorldMap_SMB3.h"

#include "GameManager_SMB3.h"
#include "Commons_SMB3.h"

#include "HUD_Display.h"

// ------------------------------------------------------------------------ //

GameScreenManager_SMB3::GameScreenManager_SMB3() : mRenderer(nullptr), mCurrentScreen(nullptr), mHUD(nullptr), mAudioPlayer(nullptr)
{
}

// ------------------------------------------------------------------------ //

GameScreenManager_SMB3::GameScreenManager_SMB3(SDL_Renderer* renderer) : mRenderer(renderer), mCurrentScreen(nullptr), mHUD(nullptr), mAudioPlayer(nullptr)
{
	// Create the audio player
	mAudioPlayer = new Audio_Player();

	// Create the new hud this high up as it is persistant accross all types of gamescreens - from the world map and into the levels themselves
	mHUD = new HUD_Display(renderer);

	// Default to being in the main menu
	ChangeScreen(SCREENS_SMB3::WORLD_MAP);
}

// ------------------------------------------------------------------------ //

GameScreenManager_SMB3::~GameScreenManager_SMB3()
{
	// Clear up the memory 
	mRenderer = nullptr;

	if (mCurrentScreen)
		delete mCurrentScreen;

	mCurrentScreen = nullptr;

	delete mAudioPlayer;
	mAudioPlayer = nullptr;
}

// ------------------------------------------------------------------------ //

void GameScreenManager_SMB3::Render()
{
	// Render the current screen
	if(mCurrentScreen)
		mCurrentScreen->Render();

	if (mHUD)
		mHUD->Render();
}

// ------------------------------------------------------------------------ //

void GameScreenManager_SMB3::Update(const float deltaTime, SDL_Event e)
{
	if (mCurrentScreen)
		mReturnDataPlaceHolder = mCurrentScreen->Update(deltaTime, e);

	if(mAudioPlayer)
		mAudioPlayer->Update();

	if (mHUD)
		mHUD->Update(deltaTime);

	// Change the screen to the new one requested
	ChangeScreen(mReturnDataPlaceHolder.GetScreenToGoTo(), mReturnDataPlaceHolder.GetFilePath());
}

// ------------------------------------------------------------------------ //

// Data passed in could be a filePath or some data on the previous level
void GameScreenManager_SMB3::ChangeScreen(SCREENS_SMB3 newScreen, std::string dataPassedIn)
{
	// If the screen currently exists then delete it - but only if we are going to be creating a new one
	if (newScreen != SCREENS_SMB3::SAME && mCurrentScreen)
	{
		delete mCurrentScreen;
		mCurrentScreen = nullptr;
	}

	switch (newScreen)
	{
	// Make no change
	default:
	case SCREENS_SMB3::SAME:
	break;

	// Load into a level
	case SCREENS_SMB3::LEVEL:
		mAudioPlayer->OnNotify(SUBJECT_NOTIFICATION_TYPES::ENTERING_LEVEL, "");

		mCurrentScreen = (GameScreen_SMB3*)(new GameScreenLevel_SMB3(mRenderer, dataPassedIn.c_str(), true, *mHUD, LEVEL_TYPE::OVERWORLD, mAudioPlayer, mHUD));
	break;

	// Go to a world map
	case SCREENS_SMB3::WORLD_MAP:
		if(dataPassedIn == "COMPLETE")
			mCurrentScreen = (GameScreen_SMB3*)(new GameScreen_WorldMap_SMB3(mRenderer, mAudioPlayer, mHUD, true));
		else
			mCurrentScreen = (GameScreen_SMB3*)(new GameScreen_WorldMap_SMB3(mRenderer, mAudioPlayer, mHUD, false));
	break;

	// Go back to the main menu
	case SCREENS_SMB3::MAIN_MENU:
		mCurrentScreen = (GameScreen_SMB3*)(new GameScreenMainMenu_SMB3(mAudioPlayer));
	break;
	}
}

// ------------------------------------------------------------------------ //
