#include "GameScreenLevel1.h"
#include "Texture2D.h"
#include "CharacterMario.h"
#include "CharacterLuigi.h"
#include "Collisions.h"

#include <iostream>

// --------------------------------------------------------------------------------------------- //

GameScreenLevel1::GameScreenLevel1(SDL_Renderer* renderer) : GameScreen(renderer)
{
	SetUpLevel();
}

// --------------------------------------------------------------------------------------------- //

GameScreenLevel1::~GameScreenLevel1()
{
	delete mBackgroundTexture;
	mBackgroundTexture = NULL;

	delete mMario;
	mMario = NULL;
}

// --------------------------------------------------------------------------------------------- //

bool GameScreenLevel1::SetUpLevel() 
{
	mBackgroundTexture = new Texture2D(mRenderer);
	if (!mBackgroundTexture->LoadFromFile("SDL_Mario_Project/Mario Bros 1 Images/BackgroundMB.png"))
	{
		std::cout << "Failed to load the background texture!" << std::endl;
		//return false;
	}

	mMario = new CharacterMario(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/Mario.png", Vector2D());

	if (mMario == NULL)
	{
		std::cout << "Failed to create the mario character for the level." << std::endl;
		//return false;
	}

	mLuigi = new CharacterLuigi(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/Luigi.png", Vector2D(0, 0));

	if (mLuigi == NULL)
	{
		std::cout << "Failed to create the luigi character for the level." << std::endl;
		//return false;
	}

	return true;
}

// --------------------------------------------------------------------------------------------- //

void GameScreenLevel1::Update(float deltaTime, SDL_Event e)
{
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