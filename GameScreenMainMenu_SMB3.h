#ifndef _GAME_SCREEN_MENU_MENU_SMB3_H_
#define _GAME_SCREEN_MENU_MENU_SMB3_H_

#include <string>

#include "GameScreen_SMB3.h"

class GameScreenMainMenu_SMB3 final : public GameScreen_SMB3
{
public:
	GameScreenMainMenu_SMB3();
	~GameScreenMainMenu_SMB3();

	void Render() override;
	ReturnDataFromGameScreen Update(const float deltaTime, SDL_Event e) override;
private:
};

#endif // !_GAME_SCREEN_MENU_MENU_SMB3_H_
