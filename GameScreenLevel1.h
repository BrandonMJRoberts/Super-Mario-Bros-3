#ifndef _GAMESCREEN_LEVEL_1_H_
#define _GAMESCREEN_LEVEL_1_H_

#include "GameScreen.h"
#include "Commons.h"

#include "AudioPlayer.h"

#include <SDL.h>
#include <vector>

class Character;
class LevelMap;
class POW;
class PIPE_SMB1;
class RenderObject;
class Texture2D;
class Coin;
class TextRenderer;

// ------------------------------------------------------------- //

enum class EnemyType
{
	CRAB = 0,
	FIGHTER_FLY,
	FREEZIE,
	SPINY, 

	MAX
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

	bool		    CheckForMarioCollision(Character* player, Vector2D position, Vector2D collisionBox, RenderObject* object, bool isACoin);

	// init
	bool            SetUpLevel();

	// Players
	Character*      mMario;
	Character*      mLuigi;

	// Level map
	LevelMap*       mLevelMap;

	// Pow and pipes
	POW*            mPowBlock;
	PIPE_SMB1*      mPipes[2];

	// Enemies and coins that can spawn
	std::vector<RenderObject*> mLevelObjects;
	std::vector<Coin*>         mCoins;

	Texture2D*                 mBackgroundSprite;

	// Enemy spawning timers
	float           mTimePerEnemySpawn;
	float           mTimeRemainingTillSpawn;

	unsigned int       mEnemysSpawnedTotal;
	const unsigned int kEnemysPerSpeedUp;

	// Spawn points
	const Vector2D     mEnemySpawnPointLeft;
	const Vector2D     mEnemySpawnPointRight;
	
	TextRenderer*      mTextRenderer;

	Audio_Player*      mAudioPlayer;

	bool			   mSpawningLeftSide;
};


#endif // !_GAMESCREEN_LEVEL_1_H_


