#include "GameScreenWorldMap_SMB3.h"

#include "GameManager_SMB3.h"

#include "WorldMapMario.h"
#include "Constants_SMB3.h"

#include "AudioPlayer.h"

#include <SDL.h>
#include <string>

// ----------------------------------------------------------------------------- //

GameScreen_WorldMap_SMB3::GameScreen_WorldMap_SMB3(SDL_Renderer* renderer, Audio_Player* audioPlayerRef) : GameScreen_SMB3(renderer, audioPlayerRef)
{
	// First setup the internal conversion system this program uses
	SetupConversionTable();

	// Then construct the file path for the current world
	std::string filePath = "SDL_Mario_Project/Worlds/World_" + std::to_string(GameManager_SMB3::GetInstance()->GetCurrentWorldIndex());

	// Needs to load in the background data file 
	mBackground = new BackgroundLayer_WorldMap(filePath, 
		                                       renderer, 
		                                       mConversionTable);

	// The level also needs a border around the world maps, so create on of those of the correct width and height of the background
	mBorder     = new WorldMapBorder(mBackground->GetWidth()  + 2, 
		                             mBackground->GetHeight() + 2, 
		                             renderer);

	// Need to load in the node Map
	mNodeMap    = new NodeMap_WorldMap(filePath + "/Node Map Layer.txt");

	// Now we need to setup the player character for the world map - default this to being mario
	mPlayer     = new WorldMapMario("SDL_Mario_Project/Characters/Mario/World Map Mario/World Map Mario Sprite Sheet.png", 
		                             renderer, 
		                             mNodeMap->GetSpawnPoint(),
		                             5, 
		                             4,
									 0.25f);

	// Notify observers that we have completed setup of the world map, and that we need to play the music
	Notify(SUBJECT_NOTIFICATION_TYPES::SETUP_WORLD_MAP, std::to_string(GameManager_SMB3::GetInstance()->GetCurrentWorldIndex()));
	Notify(SUBJECT_NOTIFICATION_TYPES::PLAY_WORLD_MAP_MUSIC, std::to_string(GameManager_SMB3::GetInstance()->GetCurrentWorldIndex()));
}

// ----------------------------------------------------------------------------- //

GameScreen_WorldMap_SMB3::~GameScreen_WorldMap_SMB3()
{
	delete mBackground;
	mBackground = nullptr;

	delete mNodeMap;
	mNodeMap = nullptr;

	delete mPlayer;
	mPlayer = nullptr;

	delete mBorder;
	mBorder = nullptr;
}

// ----------------------------------------------------------------------------- //

void GameScreen_WorldMap_SMB3::Render()
{
	// First render the background 
	if(mBackground)
		mBackground->Render();

	if (mBorder)
		mBorder->Render();

	if (mPlayer)
		mPlayer->Render();
}

// ----------------------------------------------------------------------------- //

ReturnDataFromGameScreen GameScreen_WorldMap_SMB3::Update(const float deltaTime, SDL_Event e)
{
	// Need to update the background layer for animations
	if (mBackground)
		mBackground->Update(deltaTime, e);

	// Now we need to update the player for animations and movement decisions
	if (mPlayer)
	{
		mPlayer->Update(deltaTime, *mNodeMap, e);
	}

	// If the player presses enter and is on a level then we want to enter the level
	switch (e.type)
	{
		case SDL_KEYDOWN:
			switch (e.key.keysym.sym)
			{
			case SDLK_RETURN:
				// Now we need to get if the player is on a level space, and if it is enter the level requested
				char nodeMapValue = mNodeMap->GetSpecificDataPoint(mPlayer->GetGridPosition());

				if (mNodeMap->GetValueIsLevel(nodeMapValue))
				{
					// Then return that we want to return into a level
					return ReturnDataFromGameScreen(SCREENS_SMB3::LEVEL, mNodeMap->GetLevelFilePath(nodeMapValue));
				}
			break;
			}
		break; 
	}
	
	return ReturnDataFromGameScreen();
}

// ----------------------------------------------------------------------------- //

void GameScreen_WorldMap_SMB3::SetupConversionTable()
{
	char valueToConvert = '/';
	int  intValue;

	while (valueToConvert != 'z')
	{
		valueToConvert++;

		intValue = int(valueToConvert);

		if (intValue > 47 && intValue < 58)
		{
			mConversionTable[valueToConvert] = intValue - 48;
		}
		else if (intValue > 64 && intValue < 91)
		{
			mConversionTable[valueToConvert] = intValue - 55;
		}
		else if (intValue > 96 && intValue < 123)
		{
			mConversionTable[valueToConvert] = intValue - 61;
		}
		else if (intValue >= 58 && intValue <= 64)
		{
			valueToConvert += 6;
			continue;
		}
		else
		{
			valueToConvert += 5;
			continue;
		}
	}

	// Now we need to add the index for the hyphon
	mConversionTable['-'] = 150;
}

// ----------------------------------------------------------------------------- //