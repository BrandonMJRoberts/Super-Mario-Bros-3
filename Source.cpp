
#define GAME_IS_MARIO_3 true

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <iostream>

#include "Constants.h"
#include "Constants_SMB3.h"

#include "Commons.h"
#include "Commons_SMB3.h"

#include "Texture2D.h"

#include "GameScreenManager.h"
#include "GameScreenManager_SMB3.h"

SDL_Window*              gWindow			   = nullptr;
SDL_Renderer*            gRenderer		       = nullptr;
GameScreenManager*       gameScreenManager     = nullptr;
GameScreenManager_SMB3*  gameSceenManager_SMB3 = nullptr;

Uint32 gOldTime;

bool InitSDL();
bool Update();
void Render();
void CloseSDL();

// -------------------------------------------------- //

bool InitSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "SDL did not initialise: " << SDL_GetError() << std::endl;
		return false;
	}
	else
	{
		if (GAME_IS_MARIO_3)
		{
			gWindow = SDL_CreateWindow("Super Mario 3", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH_SMB3, SCREEN_HEIGHT_SMB3, SDL_WINDOW_SHOWN);
		}
		else
		{
			// Did initialise correctly
			gWindow = SDL_CreateWindow("Super Mario 3", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		}

		if (gWindow == NULL)
		{
			std::cout << "Window was not created: " << SDL_GetError() << std::endl;
			return false;
		}

		gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

		if (gRenderer != NULL)
		{
			int imageFlags = IMG_INIT_PNG;

			if (!(IMG_Init(imageFlags) && imageFlags))
			{
				std::cout << "SDL_Image could not initialise. Error: " << SDL_GetError() << std::endl;
				return false;
			}
		}
		else
		{
			std::cout << "SDL_Renderer could not initialse: Error: " << SDL_GetError() << std::endl;
			return false;
		}
	}

	return true;
}

// -------------------------------------------------- //

bool Update()
{
	SDL_Event e;

	SDL_PollEvent(&e);

	switch (e.type)
	{
	case SDL_QUIT:
		return true;
	break;
	}
	
	Uint32 newTime = SDL_GetTicks();

	if (GAME_IS_MARIO_3)
		gameSceenManager_SMB3->Update((float)(newTime - gOldTime) / 1000.0f, e);
	else
		gameScreenManager->Update((float)(newTime - gOldTime) / 1000.0f, e);

	gOldTime = newTime;

	return false;
}

// -------------------------------------------------- //

void Render()
{
	// First clear the screen
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(gRenderer);

	if (GAME_IS_MARIO_3)
		gameSceenManager_SMB3->Render();
	else
		gameScreenManager->Render();

	SDL_RenderPresent(gRenderer);
}

// -------------------------------------------------- //

void CloseSDL()
{
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;

	SDL_DestroyRenderer(gRenderer);
	gRenderer = nullptr;

	if (GAME_IS_MARIO_3)
	{
		delete gameSceenManager_SMB3;
		gameSceenManager_SMB3 = nullptr;
	}
	else
	{
		delete gameScreenManager;
		gameScreenManager = nullptr;
	}

	IMG_Quit();
	SDL_Quit();
}

// -------------------------------------------------- //

int main(int argx, char* args[])
{
	if (InitSDL())
	{
		if(GAME_IS_MARIO_3)
			gameSceenManager_SMB3 = new GameScreenManager_SMB3(gRenderer);
		else
			gameScreenManager = new GameScreenManager(gRenderer, SCREENS::LEVEL1);

		gOldTime = SDL_GetTicks();

		bool quit = false;

		while (!quit)
		{
			Render();
			quit = Update();
		}
	}

	CloseSDL();

	return 0;
}

// -------------------------------------------------- //
