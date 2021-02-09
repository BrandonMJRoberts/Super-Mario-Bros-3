
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
#include "GameScreen_GameSelect.h"

SDL_Window*              gWindow			   = nullptr;
SDL_Renderer*            gRenderer		       = nullptr;

GameScreenManager*       gameScreenManager     = nullptr;
GameScreenManager_SMB3*  gameSceenManager_SMB3 = nullptr;
GameScreen_GameSelect*   gameSelectScreen      = nullptr;
 
Uint32 gOldTime;

bool InitSDL();
bool Update();
void Render();
void CloseSDL();

bool GameIsMario3 = true;
bool GameSelected = false;

// -------------------------------------------------- //

bool InitSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		std::cout << "SDL did not initialise: " << SDL_GetError() << std::endl;
		return false;
	}
	else
	{
		// Did initialise correctly
		gWindow = SDL_CreateWindow("Game Select", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH_SMB3, SCREEN_HEIGHT_SMB3, SDL_WINDOW_SHOWN);

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

			// Setup the audio mixer - Frequence, sample format, hardware channels (2 = sterio), size of audio chunks in bytes 
			if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
			{
				std::cout << "Could not setup the audio mixer. Error: " << SDL_GetError() << std::endl;
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

	if (GameIsMario3)
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

	if (GameIsMario3)
		gameSceenManager_SMB3->Render();
	else
		gameScreenManager->Render();

	SDL_RenderPresent(gRenderer);
}

// -------------------------------------------------- //

void CloseSDL()
{
	SDL_DestroyRenderer(gRenderer);
	gRenderer = nullptr;

	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;

	if (GameIsMario3)
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
	Mix_Quit();
}

// -------------------------------------------------- //

int main(int argx, char* args[])
{
	if (InitSDL())
	{
		// ---------------------------------------------------------------- /

		// First we need to setup the game select screen
		SDL_Event e;
		Uint32    newTime;
		GameSelectReturnData returnData;

		gameSelectScreen = new GameScreen_GameSelect(gRenderer);

		gOldTime = SDL_GetTicks();

		// Loop until a game is selected
		while (!GameSelected)
		{
			newTime = SDL_GetTicks();

			gameSelectScreen->Render();

			SDL_PollEvent(&e);

			returnData = gameSelectScreen->Update(float(newTime - gOldTime) / 1000.0f, e);

			GameSelected = returnData.gameSelected;
			GameIsMario3 = returnData.gameSelectedIsMario3;

			if (returnData.quitEverything)
			{
				CloseSDL();

				return 0;
			}

			gOldTime = newTime;
		}

		// ---------------------------------------------------------------- /

		delete gameSelectScreen;
		gameSelectScreen = nullptr;

		// Change the title of the window
		if (GameIsMario3)
		{
			SDL_SetWindowTitle(gWindow, "Super Mario Bros 3");
			SDL_SetWindowSize(gWindow, SCREEN_WIDTH_SMB3, SCREEN_HEIGHT_SMB3);
		}
		else
		{
			SDL_SetWindowTitle(gWindow, "Mario Bros");
			SDL_SetWindowSize(gWindow, SCREEN_WIDTH, SCREEN_HEIGHT);
		}

		// Now the game is selected we can play the real game
		if(GameIsMario3)
			gameSceenManager_SMB3 = new GameScreenManager_SMB3(gRenderer);
		else
			gameScreenManager     = new GameScreenManager(gRenderer, SCREENS::LEVEL1);

		// ---------------------------------------------------------------- /

		// Loop through and play the actual game
		gOldTime = SDL_GetTicks();

		bool quit = false;

		while (!quit)
		{
			Render();
			quit = Update();
		}

		// ---------------------------------------------------------------- /
	}

	CloseSDL();

	return 0;
}

// -------------------------------------------------- //
