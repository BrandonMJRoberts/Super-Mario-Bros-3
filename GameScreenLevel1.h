#ifndef _GAMESCREEN_LEVEL_1_H_
#define _GAMESCREEN_LEVEL_1_H_

#include "GameScreen.h"
#include "Commons.h"

#include <SDL.h>

class Character;
class LevelMap;
class POW;
class PIPE_SMB1;

class GameScreenLevel1 final : public GameScreen
{
public:
	GameScreenLevel1(SDL_Renderer* renderer);
	~GameScreenLevel1();

	void Render() override;
	void Update(float deltaTime, SDL_Event e) override;

private:
	void            CheckForPOWCollision();

	Character*      mMario;
	Character*      mLuigi;

	LevelMap*       mLevelMap;

	POW*            mPowBlock;
	PIPE_SMB1*      mPipes[4];

	bool            SetUpLevel();
};


#endif // !_GAMESCREEN_LEVEL_1_H_


