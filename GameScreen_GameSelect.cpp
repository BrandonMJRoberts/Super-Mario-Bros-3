#include "GameScreen_GameSelect.h"

#include "Texture2D.h"
#include "Constants_SMB3.h"

// ---------------------------------------------------------------------------------------------- //

GameScreen_GameSelect::GameScreen_GameSelect(SDL_Renderer* renderer) 
: mMarioCoin(nullptr)
, mLuigiCoin(nullptr)
, mSelectionSprite(nullptr)

, mRenderer(renderer)

, mMarioCoinPosition(SCREEN_WIDTH_SMB3 / 8, 250)
, mLuigiCoinPosition((SCREEN_WIDTH_SMB3 / 8) * 5, 250)

, mSelectionSpriteCurrentPosition(0, 0)

, mCurrentlySelectedGame(0)
, mCurrentFrameCoin(0)

, mSingleSpriteWidthCoin(0)
, mSingleSpriteHeightCoin(0)

, kTimePerCoinFrame(0.1f)
, mTimeRemainingTillCoinFrame(kTimePerCoinFrame)

, mPressedToStart(false)
, mCoinLoopFinished(false)

, mFadeTransition(nullptr)
, mAudioPlayer(nullptr)

, mTimeTillGameStarts(1.0f)
{
	// Load in the sprites
	mMarioCoin           = new Texture2D(renderer);
	mLuigiCoin           = new Texture2D(renderer);
	mSelectionSprite     = new Texture2D(renderer);

	mSelectionSprite->LoadFromFile("SDL_Mario_Project/Game Select/SelectionSprite.png");
	mLuigiCoin->LoadFromFile("SDL_Mario_Project/Game Select/CoinLuigiFace.png");
	mMarioCoin->LoadFromFile("SDL_Mario_Project/Game Select/CoinMarioFace.png");

	if (mMarioCoin)
	{
		mSingleSpriteWidthCoin  = mMarioCoin->GetWidth() / 4;
		mSingleSpriteHeightCoin = mMarioCoin->GetHeight();
	}

	mAudioPlayer    = new Audio_Player();

	if (mAudioPlayer)
		mAudioPlayer->OnNotify(SUBJECT_NOTIFICATION_TYPES::PLAY_GAME_SELECT_MUSIC, "");

	mFadeTransition = new FadeInOutTransition(Vector2D(), 0.0f, "SDL_Mario_Project/Transitions/BlackFadeImage.png", renderer, FADING_STATE::IN, 500.0f);

	mSourceRectMario = SDL_Rect{ 0, 0, (int)mSingleSpriteWidthCoin, (int)mSingleSpriteHeightCoin};
	mSourceRectLuigi = SDL_Rect{ 0, 0, (int)mSingleSpriteWidthCoin, (int)mSingleSpriteHeightCoin };

	mDestRectMario   = SDL_Rect{ (int)mMarioCoinPosition.x, (int)mMarioCoinPosition.y, (int)mSingleSpriteWidthCoin, (int)mSingleSpriteHeightCoin};
	mDestRectLuigi   = SDL_Rect{ (int)mLuigiCoinPosition.x, (int)mLuigiCoinPosition.y, (int)mSingleSpriteWidthCoin, (int)mSingleSpriteHeightCoin };


	mSelectionSpriteCurrentPosition.y = 150;

	UpdateSelectionSpritePosition();
}

// ---------------------------------------------------------------------------------------------- //

GameScreen_GameSelect::~GameScreen_GameSelect()
{
	delete mMarioCoin;
	mMarioCoin = nullptr;

	delete mLuigiCoin;
	mLuigiCoin = nullptr;

	delete mSelectionSprite;
	mSelectionSprite = nullptr;

	delete mAudioPlayer;
	mAudioPlayer = nullptr;

	delete mFadeTransition;
	mFadeTransition = nullptr;

	mRenderer = nullptr;
}

// ---------------------------------------------------------------------------------------------- //

void GameScreen_GameSelect::Render()
{
	SDL_RenderClear(mRenderer);

	if (mMarioCoin)
	{
		mMarioCoin->Render(mSourceRectMario, mDestRectMario, SDL_FLIP_NONE, 0.0f);
	}

	if (mLuigiCoin)
	{
		mLuigiCoin->Render(mSourceRectLuigi, mDestRectLuigi, SDL_FLIP_NONE, 0.0f);
	}

	if (mSelectionSprite)
		mSelectionSprite->Render(mSelectionSpriteCurrentPosition, SDL_FLIP_NONE);

	if (mFadeTransition)
	{
		mFadeTransition->Render();
	}

	SDL_RenderPresent(mRenderer);
}

// ---------------------------------------------------------------------------------------------- //

GameSelectReturnData GameScreen_GameSelect::Update(float deltaTime, SDL_Event e)
{
	if (!mFadeTransition)
		return GameSelectReturnData();

	if (mPressedToStart)
	{
		if (!mCoinLoopFinished)
		{
			mTimeRemainingTillCoinFrame -= deltaTime;

			if (mTimeRemainingTillCoinFrame <= 0.0f)
			{
				// Reset the time remaining
				mTimeRemainingTillCoinFrame = kTimePerCoinFrame;

				// Increment the frame ID
				if(mCurrentFrameCoin + 1 < 4)
					mCurrentFrameCoin++;
				else
				{
					mCurrentFrameCoin = 0;
					mCoinLoopFinished = true;

					if(mFadeTransition)
						mFadeTransition->SetFadingState(FADING_STATE::OUT);
				}

				// Set the new sprite position
				if (mCurrentlySelectedGame == 0)
					mSourceRectMario.x = mSingleSpriteWidthCoin * mCurrentFrameCoin;
				else
					mSourceRectLuigi.x = mSingleSpriteWidthCoin * mCurrentFrameCoin;
			}
		}
		else
		{
			mTimeTillGameStarts -= deltaTime;

			if (mTimeTillGameStarts <= 0.0f)
			{
				if (mCurrentlySelectedGame == 0)
					return GameSelectReturnData(true, true, false);

				return GameSelectReturnData(true, false, false);
			}
		}
	}

	// If the fade in is not complete then dont take any input from the user
	if (mFadeTransition->GetFadingState() == FADING_STATE::NONE && !mPressedToStart)
	{
		// Handle to see if the player has done any input
		switch (e.type)
		{
		case SDL_QUIT:
			return GameSelectReturnData(false, false, true);
		break;

		case SDL_KEYDOWN:
			switch (e.key.keysym.sym)
			{
			case SDLK_RIGHT:
			case SDLK_d:
				if (mCurrentlySelectedGame == 0)
					mCurrentlySelectedGame = 1;

				UpdateSelectionSpritePosition();
			break;

			case SDLK_LEFT:
			case SDLK_a:
				if (mCurrentlySelectedGame == 1)
					mCurrentlySelectedGame = 0;

				UpdateSelectionSpritePosition();
			break;

			case SDLK_RETURN:
				if(mAudioPlayer)
					mAudioPlayer->OnNotify(SUBJECT_NOTIFICATION_TYPES::GAME_SELECT_COIN_SFX, "");

				mPressedToStart = true;
			break;

			case SDLK_ESCAPE:
				return GameSelectReturnData(false, false, true);
			break;
			}
		break;
		}
	}
	else
	{
		mFadeTransition->Update(deltaTime);
	}

	return GameSelectReturnData();
}

// ---------------------------------------------------------------------------------------------- //

void GameScreen_GameSelect::UpdateSelectionSpritePosition()
{
	if (mCurrentlySelectedGame == 0)
	{
		mSelectionSpriteCurrentPosition.x = mMarioCoinPosition.x + (mSingleSpriteWidthCoin / 4);
	}
	else
	{
		mSelectionSpriteCurrentPosition.x = mLuigiCoinPosition.x + (mSingleSpriteWidthCoin / 4);
	}
}

// ---------------------------------------------------------------------------------------------- //