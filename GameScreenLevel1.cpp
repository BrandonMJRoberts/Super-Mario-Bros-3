#include "GameScreenLevel1.h"
#include "Texture2D.h"
#include "Character.h"
#include "Collisions.h"

#include "LevelMap.h"

#include "POW_SMB1.h"
#include "Pipe_SMB1.h"

#include "Crab.h"
#include "Spiny.h"
#include "Freezie.h"
#include "FighterFly.h"

#include "RenderObject.h"

#include <iostream>

// --------------------------------------------------------------------------------------------- //

GameScreenLevel1::GameScreenLevel1(SDL_Renderer* renderer) 
	: GameScreen(renderer)
	, mPowBlock(nullptr)
	, mMario(nullptr)
	, mLuigi(nullptr)
	, mLevelMap(nullptr)
	, mPipes{nullptr, nullptr, nullptr, nullptr}

	, mTimePerEnemySpawn(2.0f)
	, mTimeRemainingTillSpawn(0.0f)

	, mEnemysSpawnedTotal(0)
	, kEnemysPerSpeedUp(5)

	, mEnemySpawnPointLeft(0.0f, 0.0f)
	, mEnemySpawnPointRight(11.0f, 0.0f)

	, mBackgroundSprite(nullptr)
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

	delete mBackgroundSprite;
	mBackgroundSprite = nullptr;
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
	mMario = new Character(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/Mario.png", Vector2D(5, 0), 7, 2, mLevelMap, Vector2D(1.0f, 1.3125));
	if (!mMario)
	{
		std::cout << "mMario failed to load." << std::endl;
		mMario = nullptr;
	}

	// Create luigi
	mLuigi = new Character(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/Luigi.png", Vector2D(7, 0), 7, 2, mLevelMap, Vector2D(1.0f, 1.3125));
	if (!mLuigi)
	{
		std::cout << "mLuigi failed to load." << std::endl;
		mLuigi = nullptr;
	}

	mLuigi = nullptr;

	// Create the pow block
	mPowBlock = new POW(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/PowBlock.png", Vector2D(7.5, 9));

	// Load in the pipes
	mPipes[0] = new PIPE_SMB1(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/Pipe.png", PIPE_FACING_DIRECTION_SMB1::LEFT, Vector2D(), 0.1f);
	mPipes[1] = new PIPE_SMB1(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/Pipe.png", PIPE_FACING_DIRECTION_SMB1::LEFT, Vector2D(), 0.1f);
	mPipes[2] = new PIPE_SMB1(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/Pipe.png", PIPE_FACING_DIRECTION_SMB1::RIGHT, Vector2D(), 0.1f);
	mPipes[3] = new PIPE_SMB1(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/Pipe.png", PIPE_FACING_DIRECTION_SMB1::RIGHT, Vector2D(), 0.1f);


	mBackgroundSprite = new Texture2D(mRenderer);
	if (!mBackgroundSprite->LoadFromFile("SDL_Mario_Project/Mario Bros 1 Images/Background 1.png"))
	{
		std::cout << "Failed to load in background" << std::endl;
		return false;
	}

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

	// Now update the pow block
	if (mPowBlock)
	{
		// Check for collisions with the pow block
		CheckForPOWCollision();

		mPowBlock->Update(deltaTime, mLevelMap);
	}

	// Now update the level objects
	for (unsigned int i = 0; i < mLevelObjects.size(); i++)
	{
		if (mLevelObjects[i])
		{
			mLevelObjects[i]->Update(deltaTime, mLevelMap);

			// Now check to see if the object is too far off the screen. If so delete it
			if (mLevelObjects[i]->GetPosition().y - mLevelObjects[i]->GetCollisionBox().y > mLevelMap->GetLevelHeight())
			{
				delete mLevelObjects[i];
				mLevelObjects.erase(mLevelObjects.begin() + i);
			}
		}
		else
		{
			mLevelObjects.erase(mLevelObjects.begin() + i);
		}
		
	}

	// Spawn enemies
	EnemySpawnCheck(deltaTime);

	// Collisions check between the characters
	//if (Collisions::Instance()->Circle(mMario, mLuigi))
	//{
	//	std::cout << "collisions" << std::endl;
	//}
}

// --------------------------------------------------------------------------------------------- //

void GameScreenLevel1::Render()
{
	mBackgroundSprite->Render(Vector2D(0.0f, -250.0f), SDL_FLIP_NONE, 0.0f);

	if (mLevelMap)
		mLevelMap->Render();

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

	if (mMario)
		mMario->Render();

	if (mLuigi)
		mLuigi->Render();

	for (unsigned int i = 0; i < mLevelObjects.size(); i++)
	{
		if(mLevelObjects[i])
			mLevelObjects[i]->Render();
	}
}

// --------------------------------------------------------------------------------------------- //

void GameScreenLevel1::CheckForPOWCollision()
{
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

void GameScreenLevel1::SpawnNewEnemy(EnemyType enemy)
{
	// First choose which side to spawn at
	Vector2D spawnPoint = mEnemySpawnPointLeft;

	switch (enemy)
	{
	case EnemyType::CRAB:
		mLevelObjects.push_back(new Crab(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/Crab.png", 0.1f, spawnPoint));
	break;

	case EnemyType::FREEZIE:
		mLevelObjects.push_back(new Freezie(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/Freezie.png", 0.1f, spawnPoint, Vector2D(1.0f, 1.0f)));
	break;

	case EnemyType::FIGHTER_FLY:
		mLevelObjects.push_back(new FighterFly(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/FighterFly.png", 0.1f, spawnPoint, Vector2D(1.0f, 1.0f)));
	break;

	case EnemyType::SPINY:
		mLevelObjects.push_back(new Spiny(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/Spiny.png", 0.1f, spawnPoint, Vector2D(1.0f, 1.0f)));
	break;

	default:
	break;
	}
}

// --------------------------------------------------------------------------------------------- //

void GameScreenLevel1::EnemySpawnCheck(const float deltaTime)
{
	// update the timer
	mTimeRemainingTillSpawn -= deltaTime;

	if (mTimeRemainingTillSpawn <= 0.0f)
	{
		SpawnNewEnemy((EnemyType)(rand() % (unsigned int)EnemyType::MAX));

		mTimeRemainingTillSpawn = mTimePerEnemySpawn;
		mEnemysSpawnedTotal++;

		if (mEnemysSpawnedTotal % kEnemysPerSpeedUp == 0)
		{
			mTimePerEnemySpawn -= 0.1f;

			if (mTimePerEnemySpawn < 1.0f)
			{
				mTimePerEnemySpawn = 1.0f;
			}
		}
	}
}

// --------------------------------------------------------------------------------------------- //