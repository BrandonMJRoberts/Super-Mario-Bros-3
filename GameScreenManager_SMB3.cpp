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
}

// ------------------------------------------------------------------------ //

void GameScreenManager_SMB3::Render()
{
	// Render the current screen
	if(mCurrentScreen)
		mCurrentScreen->Render();
}

// ------------------------------------------------------------------------ //

void GameScreenManager_SMB3::Update(const float deltaTime, SDL_Event e)
{
	if (mCurrentScreen)
		mReturnDataPlaceHolder = mCurrentScreen->Update(deltaTime, e);

	// Change the screen to the new one requested
	ChangeScreen(mReturnDataPlaceHolder.GetScreenToGoTo(), mReturnDataPlaceHolder.GetFilePath());
}

// ------------------------------------------------------------------------ //

void GameScreenManager_SMB3::ChangeScreen(SCREENS_SMB3 newScreen, std::string filePath)
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
		mCurrentScreen = (GameScreen_SMB3*)(new GameScreenLevel_SMB3(mRenderer, filePath.c_str(), true));
	break;

	// Go to a world map
	case SCREENS_SMB3::WORLD_MAP:
		mCurrentScreen = (GameScreen_SMB3*)(new GameScreen_WorldMap_SMB3(mRenderer));
	break;

	// Go back to the main menu
	case SCREENS_SMB3::MAIN_MENU:
		mCurrentScreen = (GameScreen_SMB3*)(new GameScreenMainMenu_SMB3());
	break;
	}
}

// ------------------------------------------------------------------------ //
