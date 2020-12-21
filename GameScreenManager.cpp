#include "GameScreenManager.h"

#include "GameScreen.h"
#include "GameScreenLevel1.h"

// ------------------------------------------------------------------------------------------------------ //

GameScreenManager::GameScreenManager(SDL_Renderer* renderer, SCREENS startScreen)
{
	mRenderer = renderer;

	ChangeScreen(startScreen);
}

// ------------------------------------------------------------------------------------------------------ //

GameScreenManager::~GameScreenManager()
{
	mRenderer = NULL;

	delete mCurrentScreen;
	mCurrentScreen = NULL;
}

// ------------------------------------------------------------------------------------------------------ //

void GameScreenManager::Render()
{
	if(mCurrentScreen != NULL)
		mCurrentScreen->Render();
}

// ------------------------------------------------------------------------------------------------------ //

void GameScreenManager::Update(float deltaTime, SDL_Event e)
{
	if (mCurrentScreen != NULL)
		mCurrentScreen->Update(deltaTime, e);
}

// ------------------------------------------------------------------------------------------------------ //

void GameScreenManager::ChangeScreen(SCREENS newScreen)
{
	if (mCurrentScreen != NULL)
	{
		delete mCurrentScreen;
	}

	switch (newScreen)
	{
	case SCREENS::INTRO:
		break;

	case SCREENS::MENU:
		break;

	case SCREENS::GAMEOVER:
		break;

	case SCREENS::HIGHSCORES:
		break;

	case SCREENS::LEVEL1:
		mCurrentScreen = (GameScreen*)(new GameScreenLevel1(mRenderer));
	break;

	case SCREENS::LEVEL2:
		break;

	default:
		break;
	}
}

// ------------------------------------------------------------------------------------------------------ //