#include "GameScreenLevel1.h"
#include "Texture2D.h"
#include "Character.h"
#include "Collisions.h"

#include "LevelMap.h"

#include <iostream>

// --------------------------------------------------------------------------------------------- //

GameScreenLevel1::GameScreenLevel1(SDL_Renderer* renderer) : GameScreen(renderer)
{
	if (!SetUpLevel())
	{
		std::cout << "Failed to setup the level." << std::endl;
	}
}

// --------------------------------------------------------------------------------------------- //

GameScreenLevel1::~GameScreenLevel1()
{
	delete mBackgroundTexture;
	mBackgroundTexture = nullptr;

	delete mMario;
	mMario = nullptr;

	delete mLuigi;
	mLuigi = nullptr;

	delete mLevelMap;
	mLevelMap = nullptr;
}

// --------------------------------------------------------------------------------------------- //

bool GameScreenLevel1::SetUpLevel() 
{
	// Load in the collision map for this level
	mLevelMap = new LevelMap("SDL_Mario_Project/Collision Maps/Level1.txt", "SDL_Mario_Project/Background Sprites/Mario Bros 1 Images/Background Sprites Level 1.png", mRenderer);

	// If no collision map then dont load the level
	if (!mLevelMap)
		return false;

	// Load in the background texture
	mBackgroundTexture = new Texture2D(mRenderer);
	if (!mBackgroundTexture->LoadFromFile("SDL_Mario_Project/Mario Bros 1 Images/BackgroundMB.png"))
	{
		std::cout << "Background texture failed to load." << std::endl;
		mBackgroundTexture = nullptr;
		return false;
	}

	// Create mario
	mMario = new Character(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/Mario.png", Vector2D(5, 4), 1, 1, mLevelMap, Vector2D(1.0f, 1.3125));
	if (!mMario)
	{
		std::cout << "mMario failed to load." << std::endl;
		mMario = nullptr;
		return false;
	}

	// Create luigi
	mLuigi = new Character(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/Luigi.png", Vector2D(7, 4), 1, 1, mLevelMap, Vector2D(1.0f, 1.3125));
	if (!mLuigi)
	{
		std::cout << "mLuigi failed to load." << std::endl;
		mLuigi = nullptr;
		return false;
	}

	// If we get here then everything has loaded correctly
	return true;
}

// --------------------------------------------------------------------------------------------- //

void GameScreenLevel1::Update(float deltaTime, SDL_Event e)
{
	// Update the characters
	mMario->Update(deltaTime, e);
	mLuigi->Update(deltaTime, e);

	// Collisions check between the characters
	if (Collisions::Instance()->Circle(mMario, mLuigi))
	{
		std::cout << "collisions" << std::endl;
	}
}

// --------------------------------------------------------------------------------------------- //

void GameScreenLevel1::Render()
{
	mBackgroundTexture->Render(Vector2D(), SDL_FLIP_NONE, 0.0f);
	mMario->Render();
	mLuigi->Render();
}

// --------------------------------------------------------------------------------------------- //