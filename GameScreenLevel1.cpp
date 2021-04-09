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
#include "Coin.h"

#include "RenderObject.h"

#include <iostream>

// --------------------------------------------------------------------------------------------- //

GameScreenLevel1::GameScreenLevel1(SDL_Renderer* renderer) 
	: GameScreen(renderer)
	, mPowBlock(nullptr)
	, mMario(nullptr)
	, mLuigi(nullptr)
	, mLevelMap(nullptr)
	, mPipes{nullptr, nullptr}

	, mTimePerEnemySpawn(4.0f)
	, mTimeRemainingTillSpawn(0.0f)

	, mEnemysSpawnedTotal(0)
	, kEnemysPerSpeedUp(10)

	, mEnemySpawnPointLeft(-0.9f, 1.95f)
	, mEnemySpawnPointRight(15.9f, 1.95f)

	, mBackgroundSprite(nullptr)

	, mSpawningLeftSide(true)
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

	for (unsigned int i = 0; i < 2; i++)
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
	mMario = new Character(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/Mario.png", Vector2D(5.0f, 12.9f), 7, 2, mLevelMap, Vector2D(1.0f, 1.4), 0.1f);
	if (!mMario)
	{
		std::cout << "mMario failed to load." << std::endl;
		mMario = nullptr;
	}

	// Create luigi
	mLuigi = new Character(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/Luigi.png", Vector2D(7.0f, 12.9f), 7, 2, mLevelMap, Vector2D(1.0f, 1.4), 0.1f);
	if (!mLuigi)
	{
		std::cout << "mLuigi failed to load." << std::endl;
		mLuigi = nullptr;
	}

	mLuigi = nullptr;

	// Create the pow block
	mPowBlock = new POW(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/PowBlock.png", Vector2D(7.5, 9));

	// Load in the pipes
	mPipes[0] = new PIPE_SMB1(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/Pipe.png", PIPE_FACING_DIRECTION_SMB1::LEFT, Vector2D(14.0f, 2.1f), 0.1f, Vector2D(2.0f, 2.0f));
	mPipes[1] = new PIPE_SMB1(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/Pipe.png", PIPE_FACING_DIRECTION_SMB1::RIGHT, Vector2D(0.0f, 2.1f), 0.1f, Vector2D(2.0f, 2.0f));


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
			if (mLevelObjects[i]->Update(deltaTime, mLevelMap))
			{
				delete mLevelObjects[i];
				mLevelObjects[i] = nullptr;

				continue;
			}

			// Check for collisions with mario
			CheckForMarioCollision(mMario, mLevelObjects[i]->GetPosition(), mLevelObjects[i]->GetCollisionBox(), false);

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

	// Now update the coins
	for (unsigned int i = 0; i < mCoins.size(); i++)
	{
		if (mCoins[i])
		{
			mCoins[i]->Update(deltaTime, mLevelMap);

			if (CheckForMarioCollision(mMario, mCoins[i]->GetPosition(), mCoins[i]->GetCollisionBox(), true))
			{
				delete mCoins[i];
				mCoins[i] = nullptr;
			}
		}
		else
		{
			mCoins.erase(mCoins.begin() + i);
		}
	}

	// Spawn enemies
	EnemySpawnCheck(deltaTime);
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

	for (unsigned int i = 0; i < mLevelObjects.size(); i++)
	{
		if(mLevelObjects[i])
			mLevelObjects[i]->Render();
	}

	// Now render the coins
	for (unsigned int i = 0; i < mCoins.size(); i++)
	{
		if(mCoins[i])
			mCoins[i]->Render();
	}

	// Render the pipes
	for (unsigned int i = 0; i < 2; i++)
	{
		if (mPipes[i])
			mPipes[i]->Render();
	}

	if (mMario)
		mMario->Render();

	if (mLuigi)
		mLuigi->Render();
}

// --------------------------------------------------------------------------------------------- //

void GameScreenLevel1::CheckForPOWCollision()
{
	if (!mMario || !mPowBlock)
		return;

	Vector2D powBlockPos = mPowBlock->GetPosition(), marioPos = mMario->GetPosition();

	if (   Collisions::Instance()->Box(Rect2D(powBlockPos, mPowBlock->GetCollisionBox()), Rect2D(marioPos, mMario->GetCollisionBox()))
		&& marioPos.y >= powBlockPos.y + mPowBlock->GetCollisionBox().y)
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
	Vector2D spawnPoint;

	// First choose which side to spawn at
	if (mSpawningLeftSide)
		spawnPoint = mEnemySpawnPointLeft;
	else
		spawnPoint = mEnemySpawnPointRight;

	switch (enemy)
	{
	case EnemyType::CRAB:
		mLevelObjects.push_back(new Crab(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/Crab.png", 0.09f, spawnPoint, Vector2D(1.0f, 1.0f), mSpawningLeftSide, 3.0f, rand() % 3));
	break;

	case EnemyType::FREEZIE:
		mLevelObjects.push_back(new Freezie(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/Freezie.png", 0.1f, spawnPoint, Vector2D(1.0f, 1.0f), mSpawningLeftSide, 2.5f));
	break;

	case EnemyType::FIGHTER_FLY:
		mLevelObjects.push_back(new FighterFly(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/FighterFly.png", 0.12f, spawnPoint, Vector2D(1.0f, 1.0f), mSpawningLeftSide, 3.5f, rand() % 3));
	break;

	case EnemyType::SPINY:
		mLevelObjects.push_back(new Spiny(mRenderer, "SDL_Mario_Project/Mario Bros 1 Images/Spiny.png", 0.18f, spawnPoint, Vector2D(1.0f, 1.0f), mSpawningLeftSide, 1.0f, rand() % 3));
	break;

	default:
	break;
	}

	mSpawningLeftSide = !mSpawningLeftSide;

	// There is a random chance of a coin spawning at the same time as an enemy so check for this occasion here
	if (rand() % 10 == 1)
	{
		// First choose which side to spawn at
		if (mSpawningLeftSide)
			spawnPoint = mEnemySpawnPointLeft;
		else
			spawnPoint = mEnemySpawnPointRight;

		// 1 in 10 chance
		mCoins.push_back(new Coin(mRenderer, spawnPoint, "SDL_Mario_Project/Mario Bros 1 Images/Coin.png", 0.07f, Vector2D(1.0f, 1.0f)));

		mSpawningLeftSide = !mSpawningLeftSide;
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

bool GameScreenLevel1::CheckForMarioCollision(Character* player, Vector2D position, Vector2D collisionBox, bool isACoin)
{
	if (!player)
		return false;

	Vector2D marioPos = player->GetPosition();

	// X check
	if (   marioPos.x > position.x + collisionBox.x
		|| marioPos.x + player->GetCollisionBox().x < position.x)
	{
		return false;
	}

	// y check
	if (   marioPos.y - player->GetCollisionBox().y > position.y
		|| marioPos.y                               < position.y - collisionBox.y)
	{
		return false;
	}

	// set mario as hit
	if (isACoin)
		player->AddToScore();
	else
		player->SetHasBeenHit();

	return true;
}

// --------------------------------------------------------------------------------------------- //