#ifndef _GAMESCREEN_LEVEL_1_H_
#define _GAMESCREEN_LEVEL_1_H_

#include "GameScreen.h"
#include "Commons.h"

#include <SDL.h>
#include <vector>

class Character;
class LevelMap;
class POW;
class PIPE_SMB1;
class RenderObject;

// ------------------------------------------------------------- //

enum class EnemyType
{
	CRAB = 0,
	FIGHTER_FLY,
	FREEZIE,
	SPINY
};

// ------------------------------------------------------------- //

class GameScreenLevel1 final : public GameScreen
{
public:
	GameScreenLevel1(SDL_Renderer* renderer);
	~GameScreenLevel1();

	void Render() override;
	void Update(float deltaTime, SDL_Event e) override;

private:
	void            CheckForPOWCollision();

	void            SpawnNewEnemy(EnemyType enemy);
	void			EnemySpawnCheck(const float deltaTime);

	// init
	bool            SetUpLevel();

	// Players
	Character*      mMario;
	Character*      mLuigi;

	// Level map
	LevelMap*       mLevelMap;

	// Pow and pipes
	POW*            mPowBlock;
	PIPE_SMB1*      mPipes[4];

	// Enemies and coins that can spawn
	std::vector<RenderObject*> mLevelObjects;

	// Enemy spawning timers
	float           mTimePerEnemySpawn;
	float           mTimeRemainingTillSpawn;

	unsigned int       mEnemysSpawnedTotal;
	const unsigned int kEnemysPerSpeedUp;

	// Spawn points
	const Vector2D     mEnemySpawnPointLeft;
	const Vector2D     mEnemySpawnPointRight;
};


#endif // !_GAMESCREEN_LEVEL_1_H_


