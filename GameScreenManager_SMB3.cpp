#include "GameScreenManager_SMB3.h"

#include "GameScreenLevel_SMB3.h"
#include "GameScreenMainMenu_SMB3.h"
#include "GameScreenWorldMap_SMB3.h"

#include "GameManager_SMB3.h"
#include "Commons_SMB3.h"

// ------------------------------------------------------------------------ //

GameScreenManager_SMB3::GameScreenManager_SMB3()
{
	mRenderer      = nullptr;
	mCurrentScreen = nullptr;
}

// ------------------------------------------------------------------------ //

GameScreenManager_SMB3::GameScreenManager_SMB3(SDL_Renderer* renderer)
{
	mRenderer      = renderer;
	mCurrentScreen = nullptr;

	// Default to being in the main menu
	//ChangeScreen(SCREENS_SMB3::LEVEL, "SDL_Mario_Project/Levels/World 1/Level 1");
	ChangeScreen(SCREENS_SMB3::WORLD_MAP, "");
}

// ------------------------------------------------------------------------ //

GameScreenManager_SMB3::~GameScreenManager_SMB3()
{
	mRenderer = nullptr;

	if (mCurrentScreen)
		delete mCurrentScreen;

	mCurrentScreen = nullptr;
}

// ------------------------------------------------------------------------ //

void GameScreenManager_SMB3::Render()
{
	if(mCurrentScreen)
		mCurrentScreen->Render();
}

// ------------------------------------------------------------------------ //

void GameScreenManager_SMB3::Update(const float deltaTime, SDL_Event e)
{
	if (mCurrentScreen)
		mReturnDataPlaceHolder = mCurrentScreen->Update(deltaTime, e);

	// Only change screen if we are swapping to a different screen
	if (mReturnDataPlaceHolder.GetScreenToGoTo() != SCREENS_SMB3::SAME)
		ChangeScreen(mReturnDataPlaceHolder.GetScreenToGoTo(), mReturnDataPlaceHolder.GetFilePath());
}

// ------------------------------------------------------------------------ //

void GameScreenManager_SMB3::ChangeScreen(SCREENS_SMB3 newScreen, std::string filePath)
{
	// If the screen currently exists then delete it
	if (mCurrentScreen)
	{
		delete mCurrentScreen;
		mCurrentScreen = nullptr;
	}

	switch (newScreen)
	{
	case SCREENS_SMB3::SAME:
	break;

	case SCREENS_SMB3::LEVEL:
		mCurrentScreen = (GameScreen_SMB3*)(new GameScreenLevel_SMB3(mRenderer, filePath.c_str(), true));
	break;

	case SCREENS_SMB3::MAIN_MENU:
		mCurrentScreen = (GameScreen_SMB3*)(new GameScreenMainMenu_SMB3());
	break;

	case SCREENS_SMB3::WORLD_MAP:
		mCurrentScreen = (GameScreen_SMB3*)(new GameScreen_WorldMap_SMB3(mRenderer));
	break;

	default:
	break;
	}
}

// ------------------------------------------------------------------------ //
