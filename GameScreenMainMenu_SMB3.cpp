#include "GameScreenMainMenu_SMB3.h"

// -------------------------------------------------------------------------------- //

GameScreenMainMenu_SMB3::GameScreenMainMenu_SMB3() 
	: mBackground(nullptr), 
	  mSelectorSprite(nullptr), 
	  mSinglePlayerOption(nullptr),
	  mTwoPlayerOption(nullptr),
	  mNintendoCopyRight(nullptr),
	  mCurtainSprite(nullptr),
	  mGround(nullptr),
	  mAudioPlayer(nullptr),

	  GameScreen_SMB3()
{
	
}

// -------------------------------------------------------------------------------- //

GameScreenMainMenu_SMB3::GameScreenMainMenu_SMB3(Audio_Player* audioPlayerRef) 
	: mBackground(nullptr),
	  mSelectorSprite(nullptr),
	  mSinglePlayerOption(nullptr),
	  mTwoPlayerOption(nullptr),
	  mNintendoCopyRight(nullptr),
	  mCurtainSprite(nullptr),
	  mGround(nullptr), 
	  mAudioPlayer(audioPlayerRef),

	  GameScreen_SMB3(nullptr, audioPlayerRef)
{
	if(mAudioPlayer)
		mAudioPlayer->OnNotify(SUBJECT_NOTIFICATION_TYPES::PLAY_MAIN_MENU_MUSIC, "");
}

// -------------------------------------------------------------------------------- //

GameScreenMainMenu_SMB3::~GameScreenMainMenu_SMB3()
{
	delete mBackground;
	mBackground = nullptr;

	delete mSelectorSprite;
	mSelectorSprite = nullptr;

	delete mSinglePlayerOption;
	mSinglePlayerOption = nullptr;

	delete mTwoPlayerOption;
	mTwoPlayerOption = nullptr;

	delete mNintendoCopyRight;
	mNintendoCopyRight = nullptr;

	delete mCurtainSprite;
	mCurtainSprite = nullptr;

	delete mGround;
	mGround = nullptr;
}

// -------------------------------------------------------------------------------- //

void GameScreenMainMenu_SMB3::Render()
{

}

// -------------------------------------------------------------------------------- //

void GameScreenMainMenu_SMB3::HandleInput(SDL_Event e)
{
	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case SDLK_s:
		case SDLK_DOWN:

		break;

		case SDLK_w:
		case SDLK_UP:

		break;

		case SDLK_RETURN:
			mAudioPlayer->OnNotify(SUBJECT_NOTIFICATION_TYPES::GAME_SELECT_COIN_SFX, "");
		break;
		}
	break;
	}
}

// -------------------------------------------------------------------------------- //

ReturnDataFromGameScreen GameScreenMainMenu_SMB3::Update(const float deltaTime, SDL_Event e)
{
	HandleInput(e);

	return ReturnDataFromGameScreen();
}

// -------------------------------------------------------------------------------- //