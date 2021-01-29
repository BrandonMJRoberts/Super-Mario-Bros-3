#include "GameScreen_SMB3.h"

#include "AudioPlayer.h"

#include <SDL.h>

// --------------------------------------------------------------------------- //

GameScreen_SMB3::GameScreen_SMB3() : Subject(), mRenderer(nullptr)
{
	
}

// --------------------------------------------------------------------------- //

GameScreen_SMB3::GameScreen_SMB3(const SDL_Renderer* renderer, Audio_Player* audioPlayerRef) : Subject(), mRenderer(renderer)
{
	AddObserver(audioPlayerRef);
}

// --------------------------------------------------------------------------- //

GameScreen_SMB3::~GameScreen_SMB3()
{
	mRenderer = nullptr;
}

// --------------------------------------------------------------------------- //

void GameScreen_SMB3::Render()
{

}

// --------------------------------------------------------------------------- //

ReturnDataFromGameScreen GameScreen_SMB3::Update(const float deltaTime, SDL_Event e)
{
	return ReturnDataFromGameScreen(SCREENS_SMB3::SAME, "");
}

// --------------------------------------------------------------------------- //

ReturnDataFromGameScreen::ReturnDataFromGameScreen()
{
	mFilePathToLoadNext       = "";
	mScreenToTransitionToNext = SCREENS_SMB3::SAME;
}

// --------------------------------------------------------------------------- //

ReturnDataFromGameScreen::ReturnDataFromGameScreen(SCREENS_SMB3 screenType, std::string filePath)
{
	mFilePathToLoadNext      = filePath;
	mScreenToTransitionToNext = screenType;
}

// --------------------------------------------------------------------------- //
