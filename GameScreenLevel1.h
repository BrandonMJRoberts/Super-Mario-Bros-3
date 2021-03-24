#ifndef _GAMESCREEN_LEVEL_1_H_
#define _GAMESCREEN_LEVEL_1_H_

#include "GameScreen.h"
#include "Commons.h"

#include <SDL.h>

class Texture2D;
class Character;
class LevelMap;

class GameScreenLevel1 final : public GameScreen
{
public:
	GameScreenLevel1(SDL_Renderer* renderer);
	~GameScreenLevel1();

	void Render() override;
	void Update(float deltaTime, SDL_Event e) override;

private:
	Texture2D*      mBackgroundTexture;

	Character*      mMario;
	Character*      mLuigi;

	LevelMap*       mLevelMap;

	bool            SetUpLevel();
};


#endif // !_GAMESCREEN_LEVEL_1_H_


