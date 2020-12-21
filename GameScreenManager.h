#ifndef _GAMESCREEN_MANAGER_H_
#define _GAMESCREEN_MANAGER_H_

#include "Commons.h"
#include <SDL.h>

class GameScreen;
class GameScreenLevel1;

class GameScreenManager
{
public:
	GameScreenManager(SDL_Renderer* renderer, SCREENS startScreen);
	~GameScreenManager();

	void Render();
	void Update(float deltaTime, SDL_Event e);

	void ChangeScreen(SCREENS newScreen);
private:
	SDL_Renderer* mRenderer;
	GameScreen*   mCurrentScreen;
};

#endif // !_GAMESCREEN_MANAGER_H_

