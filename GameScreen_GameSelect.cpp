#include "GameScreen_GameSelect.h"

#include "Texture2D.h"
#include "Constants_SMB3.h"

// ---------------------------------------------------------------------------------------------- //

GameScreen_GameSelect::GameScreen_GameSelect(SDL_Renderer* renderer) 
: mMario3Game(nullptr)
, mMarioBrosGame(nullptr)
, mCoinSpriteSheet(nullptr)
, mSelectionGameSprite(nullptr)

, mSelectGamePosition(SCREEN_WIDTH_SMB3 / 2, 0)
, mRenderer(renderer)

, mLeftOptionPosition(0,         150)
, mRightOptionPosition(double(SCREEN_WIDTH_SMB3 / 4.0f) * 3, 150)
, mCentreOptionPosition(SCREEN_WIDTH_SMB3 / 2,               150)
, mCoinPosition(SCREEN_WIDTH_SMB3 / 2,                       double(SCREEN_HEIGHT_SMB3 / 2) - 50.0f)

, mCurrentlySelectedGame(0)
, mCurrentFrameCoin(0)
, mCurrentlyScrolling(false)
, mInIntro(true)

, mSingleSpriteWidthCoin(0)
, mSingleSpriteHeightCoin(0)

, mDelayBeforeGamesShow(1.1f)
, kTimePerCoinFrame(0.12f)
, mTimeTillAutoStart(5.0f)
, mTimeRemainingTillCoinFrame(kTimePerCoinFrame)

, mPressedToStart(false)
, mCoinLoopFinished(false)
{
	// Load in the sprites
	mMario3Game          = new Texture2D(renderer);
	mMarioBrosGame       = new Texture2D(renderer);
	mSelectionGameSprite = new Texture2D(renderer);
	mCoinSpriteSheet     = new Texture2D(renderer);

	mMario3Game->LoadFromFile("SDL_Mario_Project/Game Select/Super Mario 3.png");
	mMarioBrosGame->LoadFromFile("SDL_Mario_Project/Game Select/Mario Bros.png");
	mSelectionGameSprite->LoadFromFile("SDL_Mario_Project/Game Select/Select Game.png");
	mCoinSpriteSheet->LoadFromFile("SDL_Mario_Project/Game Select/Coin.png");

	if (mCoinSpriteSheet)
	{
		mSingleSpriteWidthCoin  = mCoinSpriteSheet->GetWidth() / 4;
		mSingleSpriteHeightCoin = mCoinSpriteSheet->GetHeight();
	}

	mAudioPlayer = new Audio_Player();
}

// ---------------------------------------------------------------------------------------------- //

GameScreen_GameSelect::~GameScreen_GameSelect()
{
	delete mMario3Game;
	mMario3Game = nullptr;

	delete mMarioBrosGame;
	mMarioBrosGame = nullptr;

	delete mSelectionGameSprite;
	mSelectionGameSprite = nullptr;

	delete mCoinSpriteSheet;
	mCoinSpriteSheet = nullptr;

	mRenderer = nullptr;
}

// ---------------------------------------------------------------------------------------------- //

void GameScreen_GameSelect::Render()
{
	SDL_RenderClear(mRenderer);

	if (mInIntro)
	{
		SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0x00);

		if (mCoinSpriteSheet)
		{
			SDL_Rect destRect{ int(mCoinPosition.x) - int(mSingleSpriteWidthCoin / 2), int(mCoinPosition.y) - int(mSingleSpriteHeightCoin / 2), (int)mSingleSpriteWidthCoin, (int)mSingleSpriteHeightCoin };
			SDL_Rect sourceRect{ int(mCurrentFrameCoin * mSingleSpriteWidthCoin), 0, (int)mSingleSpriteWidthCoin, (int)mSingleSpriteHeightCoin };

			mCoinSpriteSheet->Render(sourceRect, destRect, SDL_FLIP_NONE, 0.0f);
		}
	}
	else
	{
		// First clear the screen
		SDL_SetRenderDrawColor(mRenderer, 0x42, 0x71, 0xB5, 0x00);

			if (mMarioBrosGame)
				mMarioBrosGame->Render(Vector2D(mLeftOptionPosition.x - (mMarioBrosGame->GetWidth() / 2), mLeftOptionPosition.y), SDL_FLIP_NONE, 0.0f);

			if (mMario3Game)
				mMario3Game->Render(Vector2D(mCentreOptionPosition.x - (mMario3Game->GetWidth() / 2), mCentreOptionPosition.y), SDL_FLIP_NONE, 0.0f);

			if (mSelectionGameSprite)
				mSelectionGameSprite->Render(Vector2D(mSelectGamePosition.x - (mSelectionGameSprite->GetWidth() / 2), mSelectGamePosition.y), SDL_FLIP_NONE, 0.0f);
	}

	SDL_RenderPresent(mRenderer);
}

// ---------------------------------------------------------------------------------------------- //

GameSelectReturnData GameScreen_GameSelect::Update(float deltaTime, SDL_Event e)
{
	if (mInIntro)
	{
		if (mPressedToStart)
		{
			if (!mCoinLoopFinished)
			{
				mTimeRemainingTillCoinFrame -= deltaTime;

				if (mTimeRemainingTillCoinFrame <= 0.0f)
				{
					mTimeRemainingTillCoinFrame = kTimePerCoinFrame;

					mCurrentFrameCoin++;

					if (mCurrentFrameCoin > 3)
					{
						mCurrentFrameCoin = 0;
						mCoinLoopFinished = true;
					}
				}
			}
		}
		else
		{
			mTimeTillAutoStart -= deltaTime;

			if (mTimeTillAutoStart <= 0.0f)
			{
				mPressedToStart = true;

				if (mAudioPlayer)
					mAudioPlayer->OnNotify(SUBJECT_NOTIFICATION_TYPES::GAME_SELECT_COIN_SFX, "");
			}
		}

		if (mCoinLoopFinished)
		{
			mDelayBeforeGamesShow -= deltaTime;

			if (mDelayBeforeGamesShow <= 0.0f)
			{
				mInIntro = false;

				mAudioPlayer->OnNotify(SUBJECT_NOTIFICATION_TYPES::PLAY_GAME_SELECT_MUSIC, "");
			}
		}
	}

	switch (e.type)
	{
		case SDL_QUIT:
			return GameSelectReturnData(false, false, true);
		break;

		if (!mCurrentlyScrolling)
		{
			case SDL_KEYDOWN:
				if (mInIntro)
				{
					mPressedToStart = true;

					if (mAudioPlayer)
						mAudioPlayer->OnNotify(SUBJECT_NOTIFICATION_TYPES::GAME_SELECT_COIN_SFX, "");

					return GameSelectReturnData();
				}

				switch (e.key.keysym.sym)
				{
					case SDLK_a:
					case SDLK_LEFT:
						ScrollSelectedGame(false);
					break;

					case SDLK_RIGHT:
					case SDLK_d:
						ScrollSelectedGame(true);
					break;

					case SDLK_RETURN:
						if (mCurrentlySelectedGame == 0)
							return GameSelectReturnData(true, true, false);
						else
							return GameSelectReturnData(true, false, false);
					break;
				}
			break;
		}
	}

	return GameSelectReturnData();
}

// ---------------------------------------------------------------------------------------------- //

void GameScreen_GameSelect::ScrollSelectedGame(const unsigned int optionScrollingTo)
{
	mCurrentlySelectedGame = optionScrollingTo;

	mCurrentlyScrolling = true;
}

// ---------------------------------------------------------------------------------------------- //