#ifndef _GAMESCREEN_H_
#define _GAMESCREEN_H_

#include <SDL.h>

class GameScreen
{
public:
	GameScreen(SDL_Renderer* renderer);
	~GameScreen();

	virtual void Render();
	virtual void Update(float deltaTime, SDL_Event e);

protected:
	SDL_Renderer* mRenderer;


};

#endif // !_GAMESCREEN_H
