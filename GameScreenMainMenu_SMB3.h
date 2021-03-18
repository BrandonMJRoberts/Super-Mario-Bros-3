#ifndef _GAME_SCREEN_MENU_MENU_SMB3_H_
#define _GAME_SCREEN_MENU_MENU_SMB3_H_

#include <string>

#include "GameScreen_SMB3.h"

#include "Texture2D.h"

#include "AudioPlayer.h"

class GameScreenMainMenu_SMB3 final : public GameScreen_SMB3
{
public:
	GameScreenMainMenu_SMB3();
	GameScreenMainMenu_SMB3(Audio_Player* audioPlayerRef);
	~GameScreenMainMenu_SMB3();

	void Render() override;
	ReturnDataFromGameScreen Update(const float deltaTime, SDL_Event e) override;

private:
	void HandleInput(SDL_Event e);

	Texture2D* mSelectorSprite;

	Texture2D* mSinglePlayerOption;
	Texture2D* mTwoPlayerOption;

	Texture2D* mNintendoCopyRight;

	Texture2D* mCurtainSprite;
	Texture2D* mGround;

	Texture2D* mBackground;

	Audio_Player* mAudioPlayer;
};

#endif // !_GAME_SCREEN_MENU_MENU_SMB3_H_
