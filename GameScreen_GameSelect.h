#ifndef _GAMESCREEN_GAMESELECT_H_
#define _GAMESCREEN_GAMESELECT_H_

#include <SDL.h>

#include "Commons.h"
#include "Commons_SMB3.h"
#include "AudioPlayer.h"

class Texture2D;

class GameScreen_GameSelect
{
public:
	GameScreen_GameSelect(SDL_Renderer* renderer);
	~GameScreen_GameSelect();

	void Render();
	GameSelectReturnData Update(float deltaTime, SDL_Event e);

private:
	void ScrollSelectedGame(const unsigned int optionScrollingTo);

	SDL_Renderer*    mRenderer;

	Texture2D*       mMario3Game;
	Texture2D*		 mMarioBrosGame;
	Texture2D*		 mSelectionGameSprite;

	Audio_Player*    mAudioPlayer;

	Texture2D*		 mCoinSpriteSheet;
	unsigned int     mSingleSpriteWidthCoin;
	unsigned int     mSingleSpriteHeightCoin;

	const float      kTimePerCoinFrame;
	float            mTimeRemainingTillCoinFrame;
	unsigned int	 mCurrentFrameCoin;

	float            mTimeTillAutoStart;
	float			 mDelayBeforeGamesShow;

	const Vector2D   mLeftOptionPosition;
	const Vector2D   mCentreOptionPosition;
	const Vector2D   mRightOptionPosition;
	const Vector2D   mSelectGamePosition;
	const Vector2D	 mCoinPosition;

	unsigned int     mCurrentlySelectedGame;
	bool             mCurrentlyScrolling;

	bool             mInIntro;
	bool			 mPressedToStart;
	bool			 mCoinLoopFinished;

};


#endif
