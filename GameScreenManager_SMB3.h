#ifndef _GAME_SCREEN_MANAGER_SMB3_H_
#define _GAME_SCREEN_MANAGER_SMB3_H_

#include <SDL.h>
#include <string>

#include "GameScreen_SMB3.h"

// -------------------------------------------------------------------------------------- //

class GameScreenManager_SMB3 final
{
public:
	GameScreenManager_SMB3();
	GameScreenManager_SMB3(SDL_Renderer* renderer);
	~GameScreenManager_SMB3();

	void Render();
	void Update(const float deltaTime, SDL_Event e);

	void ChangeScreen(SCREENS_SMB3 newScreen, std::string filePath);

private:
	SDL_Renderer*            mRenderer;      // SDL renderer
	GameScreen_SMB3*         mCurrentScreen; // Current screen type

	ReturnDataFromGameScreen mReturnDataPlaceHolder;

};

// -------------------------------------------------------------------------------------- //

#endif // !_GAME_SCREEN_MANAGER_SMB3_H_
