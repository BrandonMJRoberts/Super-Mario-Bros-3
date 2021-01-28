#ifndef _GAME_SCREEN_SMB3_H_
#define _GAME_SCREEN_SMB3_H_

#include <SDL.h>
#include <string>

#include "Commons_SMB3.h"

// -------------------------------------------------------------------------------------- //

struct ReturnDataFromGameScreen
{
public:
	ReturnDataFromGameScreen();
	ReturnDataFromGameScreen(SCREENS_SMB3 screenType, std::string filePath);

	std::string  GetFilePath()     { return mFilePathToLoadNext; }
	SCREENS_SMB3 GetScreenToGoTo() { return mScreenToTransitionToNext; }

private:
	std::string  mFilePathToLoadNext;
	SCREENS_SMB3 mScreenToTransitionToNext;
};

// -------------------------------------------------------------------------------------- //

class GameScreen_SMB3
{
public:
	GameScreen_SMB3();
	GameScreen_SMB3(const SDL_Renderer* renderer);
	virtual ~GameScreen_SMB3();

	virtual void Render();
	virtual ReturnDataFromGameScreen Update(const float deltaTime, SDL_Event e);

private:
	const SDL_Renderer* mRenderer;

};

#endif // !_GAME_SCREEN_SMB3_H_

