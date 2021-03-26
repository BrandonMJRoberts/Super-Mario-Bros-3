#include "GameScreenLevel1.h"
#include "Texture2D.h"
#include "Character.h"
#include "Collisions.h"

#include "LevelMap.h"

#include "POW_SMB1.h"
#include "Pipe_SMB1.h"

#include <iostream>

// --------------------------------------------------------------------------------------------- //

GameScreenLevel1::GameScreenLevel1(SDL_Renderer* renderer) 
	: GameScreen(renderer)
	, mPowBlock(nullptr)
	, mMario(nullptr)
	, mLuigi(nullptr)
	, mLevelMap(nullptr)
	, mPipes{nullptr, nullptr, nullptr, nullptr}
{
	if (!SetUpLevel())
	{
		std::cout << "Failed to setup the level." << std::endl;
	}
}

// --------------------------------------------------------------------------------------------- //

GameScreenLevel1::~GameScreenLevel1()
{
	delete mMario;
	mMario = nullptr;

	delete mLuigi;
	mLuigi = nullptr;

	delete mLevelMap;
	mLevelMap = nullptr;

	delete mPowBlock;
	mPowBlock = nullptr;

	for (unsigned int i = 0; i < 4; i++)
	{
		delete mPipes[i];
		mPipes[i] = nullptr;
	}
}

// --------------------------------------------------------------------------------------------- //

bool GameScreenLevel1::SetUpLevel()
{
	// Load in the collision map for this level
	mLevelMap = new LevelMap("SDL_Mario_Project/Level Maps SMB1/CollisionMapLevel1.txt", "SDL_Mario_Project/Mario Bros 1 Images/Background Sprites Level 1.png", mRenderer);

	// If no collision map then dont load the level
	if (!mLevelMap)
		return false;

	// Create mario
	mMario = new Character(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/Mario.png", Vector2D(5, 0), 1, 1, mLevelMap, Vector2D(1.0f, 1.3125));
	if (!mMario)
	{
		std::cout << "mMario failed to load." << std::endl;
		mMario = nullptr;
	}

	// Create luigi
	mLuigi = new Character(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/Luigi.png", Vector2D(7, 0), 1, 1, mLevelMap, Vector2D(1.0f, 1.3125));
	if (!mLuigi)
	{
		std::cout << "mLuigi failed to load." << std::endl;
		mLuigi = nullptr;
	}

	mLuigi = nullptr;

	// Create the pow block
	mPowBlock = new POW(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/PowBlock.png", Vector2D(7.5, 9));

	// If we get here then everything has loaded correctly
	return true;
}

// --------------------------------------------------------------------------------------------- //

void GameScreenLevel1::Update(float deltaTime, SDL_Event e)
{
	// Update the characters
	if (mMario)
		mMario->Update(deltaTime, e);

	if (mLuigi)
		mLuigi->Update(deltaTime, e);

	// Check for collisions with the pow block
	CheckForPOWCollision();

	// Now update the pow block
	if (mPowBlock)
	{
		mPowBlock->Update(deltaTime);
	}

	// Collisions check between the characters
	//if (Collisions::Instance()->Circle(mMario, mLuigi))
	//{
	//	std::cout << "collisions" << std::endl;
	//}
}

// --------------------------------------------------------------------------------------------- //

void GameScreenLevel1::Render()
{
	if (mLevelMap)
		mLevelMap->Render();

	if (mMario)
		mMario->Render();

	if (mLuigi)
		mLuigi->Render();

	if (mPowBlock)
	{
		mPowBlock->Render();
	}

	// Render the pipes
	for (unsigned int i = 0; i < 4; i++)
	{
		if (mPipes[i])
			mPipes[i]->Render();
	}
}

// --------------------------------------------------------------------------------------------- //

void GameScreenLevel1::CheckForPOWCollision()
{
	if (!mPowBlock)
		return;

	if (Collisions::Instance()->Box(Rect2D(mPowBlock->GetPosition(), mPowBlock->GetCollisionBox()), Rect2D(mMario->GetPosition(), mMario->GetCollisionBox())))
	{
		// There is a collision so now check that mario is going upwards and is in the correct position
		

		// There is a collision so tell the POW block that this has happened
		if (mPowBlock->SetHasBeenHit())
		{
			delete mPowBlock;
			mPowBlock = nullptr;
		}
	}
}

// --------------------------------------------------------------------------------------------- //