#ifndef _GAMESCREEN_GAMESELECT_H_
#define _GAMESCREEN_GAMESELECT_H_

#include <SDL.h>

#include "Commons.h"
#include "Commons_SMB3.h"
#include "AudioPlayer.h"

#include "FadeInOutTransition.h"

class Texture2D;

class GameScreen_GameSelect
{
public:
	GameScreen_GameSelect(SDL_Renderer* renderer);
	~GameScreen_GameSelect();

	void Render();
	GameSelectReturnData Update(float deltaTime, SDL_Event e);

private:
	void UpdateSelectionSpritePosition();

	FadeInOutTransition* mFadeTransition;

	SDL_Renderer*    mRenderer; // Renderer

	// Sprites
	Texture2D*       mMarioCoin;
	Texture2D*		 mLuigiCoin;
	Texture2D*		 mSelectionSprite;

	// Audio Player
	Audio_Player*    mAudioPlayer; 

	// Dimensions of the coins
	unsigned int     mSingleSpriteWidthCoin;
	unsigned int     mSingleSpriteHeightCoin;

	// Coin animation data
	const float      kTimePerCoinFrame;
	float            mTimeRemainingTillCoinFrame;
	unsigned int	 mCurrentFrameCoin;

	float            mTimeTillGameStarts;

	// Positions
	const Vector2D   mMarioCoinPosition;
	const Vector2D   mLuigiCoinPosition;
	Vector2D         mSelectionSpriteCurrentPosition;

	// Game loop variables
	int              mCurrentlySelectedGame;

	SDL_Rect         mDestRectMario, mSourceRectMario;
	SDL_Rect         mDestRectLuigi, mSourceRectLuigi;

	bool			 mPressedToStart;
	bool			 mCoinLoopFinished;

};


#endif
