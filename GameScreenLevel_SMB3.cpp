#include "GameScreenLevel_SMB3.h"

#include "GameManager_SMB3.h"
#include "Constants_SMB3.h"
#include "LevelArea.h"
#include "Commons_SMB3.h"

#include "PlayableCharacter.h"

#include <SDL.h>
#include <filesystem>
#include <iostream>

// --------------------------------------------------------------------------------------------------------------------------- //

GameScreenLevel_SMB3::GameScreenLevel_SMB3(SDL_Renderer* renderer
	, const char* levelFilePath 
	, const bool  playingAsMario) 
	: GameScreen_SMB3(renderer)
	, mCurrentLevelAreaID(0)    // Default the level area to being zero in case there is not one named 'Overworld'
	, mPlayer(nullptr)
{
	// Setup the lookup table
	InitialiseLookUpTable();

	// First loop through all folders contained within the file path presented, each folder is a different level area
	// To do this you use the std::filesystem include
	// This is specifically a c++17 feature (it is experimental in c++14)
	for (const auto& entry : std::filesystem::directory_iterator(levelFilePath))
	{
		bool thisIsStartingArea = false;

		// Now we have the file path and the amount of folders then we can setup the level areas
		mAreas.push_back(new LevelAreas(entry.path().u8string().c_str(), thisIsStartingArea, renderer, mConversionFromCharToIntIndexMap));

		// If this is the starting area then set this to be the current index
		if (thisIsStartingArea)
			mCurrentLevelAreaID = mAreas.size() - 1;
	}

	// Now create the player that will be in these levels
	if (playingAsMario)
	{
		mPlayer = new PlayableCharacter(renderer, "SDL_Mario_Project/Characters/Mario/In Game Mario/SmallMarioSpriteSheet.png", mAreas[mCurrentLevelAreaID]->GetInitialSpawnPoint(), Vector2D(16, 1));
	}
	else
	{
		mPlayer = new PlayableCharacter(renderer, "SDL_Mario_Project/Characters/Luigi/In Game Luigi/SmallLuigiSpriteSheet.png", mAreas[mCurrentLevelAreaID]->GetInitialSpawnPoint(), Vector2D(16, 1));
	}
}

// --------------------------------------------------------------------------------------------------------------------------- //

GameScreenLevel_SMB3::~GameScreenLevel_SMB3()
{
	for (unsigned int i = 0; i < mAreas.size(); i++)
		delete mAreas[i];

	mAreas.clear();

	delete mPlayer;
	mPlayer = nullptr;
}

// --------------------------------------------------------------------------------------------------------------------------- //

void GameScreenLevel_SMB3::Render()
{
	// Render the current area we are in
	if (mAreas.size() > mCurrentLevelAreaID)
		mAreas[mCurrentLevelAreaID]->Render(mPlayer->GetRenderReferencePoint());
	else
	{
		std::cout << "Render Error: The current area does not exist." << std::endl;
	}

	// Render the player
	if (mPlayer)
		mPlayer->Render();
}

// --------------------------------------------------------------------------------------------------------------------------- //

ReturnDataFromGameScreen GameScreenLevel_SMB3::Update(const float deltaTime, SDL_Event e)
{
	// Now handle input
	HandleInput(deltaTime, e);

	// Update the game manager
	GameManager_SMB3::GetInstance()->Update(deltaTime);

	// Update the area we are currently in if it exists
	if (mAreas.size() > mCurrentLevelAreaID)
	{
		mAreas[mCurrentLevelAreaID]->Update(deltaTime, e, mPlayer);

		if(mPlayer)
			mPlayer->Update(deltaTime, e, mAreas[mCurrentLevelAreaID]->GetLevelBounds());
	}
	else
	{
		std::cout << "Update Error: The current area does not exist. Returning to the main menu." << std::endl;
		return ReturnDataFromGameScreen(SCREENS_SMB3::MAIN_MENU, "");
	}

	return ReturnDataFromGameScreen();
}

// --------------------------------------------------------------------------------------------------------------------------- //

void GameScreenLevel_SMB3::InitialiseLookUpTable()
{
	char valueToConvert = '/';
	int  intValue;

	while (valueToConvert != 'z')
	{
		valueToConvert++;

		intValue = int(valueToConvert);

		if (intValue > 47 && intValue < 58)
		{
			mConversionFromCharToIntIndexMap[valueToConvert] = intValue - 48;
		}
		else if (intValue > 64 && intValue < 91)
		{
			mConversionFromCharToIntIndexMap[valueToConvert] = intValue - 55;
		}
		else if (intValue > 96 && intValue < 123)
		{
			mConversionFromCharToIntIndexMap[valueToConvert] = intValue - 61;
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
	mConversionFromCharToIntIndexMap['-'] = 150;
}

// --------------------------------------------------------------------------------------------------------------------------- //

void GameScreenLevel_SMB3::HandleInput(const float deltaTime, SDL_Event e)
{
	// Get the reference once so we dont have to multiple times later on
	GameManager_SMB3* GM = GameManager_SMB3::GetInstance();

	// Convert the global position into a grid position first so we are not repeatly doing it
	Vector2D gridPos     = Commons_SMB3::ConvertFromRealPositionToGridPositionReturn(GM->GetRenderReferencePoint(), RESOLUTION_OF_SPRITES);

	// Perform the boundary checks before checking for input
	if (gridPos.x <= 0.0f)
	{
		GM->SetRenderReferencePointX(0.0f); // Set the position
		GM->SetScrollingLeft(false);        // Stop movement
		GM->SetHasHitLeftBoundary(true);    // Set has hit the boundary
	}
	else if(gridPos.x > 0.0f)
	{
		GM->SetHasHitLeftBoundary(false); // Set has not hit the boundary
	}

	if (gridPos.x + BACKGROUND_SPRITE_RENDER_WIDTH >= mAreas[mCurrentLevelAreaID]->GetLevelWidth())
	{
		// Only bother with the right check if we have not hit the left boundary at the same time
		if (!GM->GetHasHitLeftBoundary())
		{
			GM->SetRenderReferencePointX((mAreas[mCurrentLevelAreaID]->GetLevelWidth() - BACKGROUND_SPRITE_RENDER_WIDTH) * RESOLUTION_OF_SPRITES); // Set the restriction
		}

		GM->SetScrollingRight(false); // Stop scrolling
		GM->SetHasHitRightBoundary(true); // Set has hit the boundary
	}
	else
	{
		GM->SetHasHitRightBoundary(false); // Enable movement
	}

	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case SDLK_a:
			// Boundary check
			if (!GM->GetHasHitLeftBoundary())
			{
				GM->SetScrollingLeft(true);
				GM->SetScrollingRight(false);
			}
		break;

		case SDLK_d:
			// Boundary check
			if (!GM->GetHasHitRightBoundary())
			{
				GM->SetScrollingLeft(false);
				GM->SetScrollingRight(true);	
			}
		break;

		case SDLK_s:
			//GM->AddToReferencePoint(Vector2D(0, -RESOLUTION_OF_SPRITES * (double)deltaTime));
		break;

		case SDLK_w:
			//GM->AddToReferencePoint(Vector2D(0, RESOLUTION_OF_SPRITES * (double)deltaTime));
		break;

		case SDLK_LSHIFT:
			GM->SetScrollingSpeedMultiplier(3.0f);
		break;

		default:
			break;
		}
	break;

	case SDL_KEYUP:
		switch (e.key.keysym.sym)
		{
		case SDLK_a:
			GM->SetScrollingLeft(false);
		break;

		case SDLK_d:
			GM->SetScrollingRight(false);
		break;

		case SDLK_s:
			//GM->AddToReferencePoint(Vector2D(0, -RESOLUTION_OF_SPRITES * (double)deltaTime));
		break;

		case SDLK_w:
			//GM->AddToReferencePoint(Vector2D(0, RESOLUTION_OF_SPRITES * (double)deltaTime));
		break;

		case SDLK_LSHIFT:
			GM->SetScrollingSpeedMultiplier(1.0f);
		break;

		default:
			break;
		}

	break;

	default:
	break;
	}
}

// --------------------------------------------------------------------------------------------------------------------------- //