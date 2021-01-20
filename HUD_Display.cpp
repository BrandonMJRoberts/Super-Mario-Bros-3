#include "Hud_Display.h"

#include "Texture2D.h"
#include "TextRenderer.h"

#include <sstream>
#include <SDL.h>

// ------------------------------------------------------------------------ //

HUD_Display::HUD_Display(SDL_Renderer* renderer) : Observer()
	, mLivesCounterOffset(147, 621)
	, mScoreCounterOffset(336, 621)
	, mMoneyCounterOffset(455, 597)
	, mCurrentWorldOffset(147, 597)
	, mPMeterOffset(189, 597)
	, mTimerCounterOffset(455, 621)
	, mFirstEndCardOffset(500, 579)
	, mBackgroundSpriteOffset(0, 550)
	, mEndCards { END_CARD_TYPES::EMPTY, END_CARD_TYPES::EMPTY, END_CARD_TYPES::EMPTY }
	, mTimeRemaming(0.0f)
	, mCurrentWorldID(0)
	, mCurrentEndCardCount(0)
	, mPMeterFillAmount(0)
	, mFontRenderer(nullptr)
	, mEndCardsSingleSpriteWidth(0)
	, mEndCardsSingleSpriteHeight(0)
{
	// Now load in the sprite sheets needed
	LoadInSprites(renderer);

	// Now create the text renderer
	mFontRenderer = new TextRenderer(renderer, "SDL_Mario_Project/Fonts and HUD/Font.png", 12, 3);

	if (!mFontRenderer)
	{
		std::cout << "Failed to create the font renderer for the HUD!" << std::endl;
		return;
	}

	if (mEndCardsSpriteSheet)
	{
		mEndCardsSingleSpriteWidth  = mEndCardsSpriteSheet->GetWidth() / 2;
		mEndCardsSingleSpriteHeight = mEndCardsSpriteSheet->GetHeight() / 2;
	}
}

// ------------------------------------------------------------------------ //

HUD_Display::~HUD_Display()
{
	DeleteAllSprites();
}

// ------------------------------------------------------------------------ //

void HUD_Display::LoadInSprites(SDL_Renderer* renderer)
{
	mBackgroundSprite = new Texture2D(renderer);
	mBackgroundSprite->LoadFromFile("SDL_Mario_Project/Fonts and HUD/HUD.png");

	mPMeterArrowSprite = new Texture2D(renderer);
	mPMeterArrowSprite->LoadFromFile("SDL_Mario_Project/Fonts and HUD/P-Meter Dash.png");

	mCompletePMeterSprite = new Texture2D(renderer);
	mCompletePMeterSprite->LoadFromFile("SDL_Mario_Project/Fonts and HUD/P-Meter P.png");

	mCurrentWorldSprite = new Texture2D(renderer);
	mCurrentWorldSprite->LoadFromFile("SDL_Mario_Project/Fonts and HUD/.png");

	mEndCardsSpriteSheet = new Texture2D(renderer);
	mEndCardsSpriteSheet->LoadFromFile("SDL_Mario_Project/Fonts and HUD/End Cards.png");
}

// ------------------------------------------------------------------------ //

void HUD_Display::DeleteAllSprites()
{
	delete mBackgroundSprite;
	mBackgroundSprite = nullptr;

	delete mPMeterArrowSprite;
	mPMeterArrowSprite = nullptr;

	delete mCompletePMeterSprite;
	mCompletePMeterSprite = nullptr;

	delete mCurrentWorldSprite;
	mCurrentWorldSprite = nullptr;

	delete mEndCardsSpriteSheet;
	mEndCardsSpriteSheet = nullptr;
}

// ------------------------------------------------------------------------ //

void HUD_Display::Render()
{ 
	// Render the main background sprite
	if (mBackgroundSprite)
		mBackgroundSprite->Render(mBackgroundSpriteOffset, SDL_FLIP_NONE, 0.0f);

	// Now render the three cards to the right of the main sprite
	if (mEndCardsSpriteSheet)
	{
		Vector2D offset((int)mEndCardsSingleSpriteWidth, 0);

		mDestRectPlaceHolder   = SDL_Rect{ (int)mFirstEndCardOffset.x, (int)mFirstEndCardOffset.y, (int)mEndCardsSingleSpriteWidth, (int)mEndCardsSingleSpriteHeight };
		mSourceRectPlaceHolder = SDL_Rect{0, 0, (int)mEndCardsSingleSpriteWidth, (int)mEndCardsSingleSpriteHeight };

		for (unsigned int i = 0; i < 3; i++)
		{
			mSourceRectPlaceHolder.x = ((int)mEndCards[i] % 2) * (int)mEndCardsSingleSpriteWidth;
			mSourceRectPlaceHolder.y = ((int)mEndCards[i] / 2) * (int)mEndCardsSingleSpriteHeight;

			mEndCardsSpriteSheet->Render(mSourceRectPlaceHolder, mDestRectPlaceHolder, SDL_FLIP_NONE, 0.0f);

			mDestRectPlaceHolder.x += (int)offset.x;
		}
	}

	if (mPMeterArrowSprite && mCompletePMeterSprite)
	{
		for(unsigned int i = 0; i < 5; i++)
			mPMeterArrowSprite->Render(Vector2D(mPMeterOffset.x + double(i * 26), mPMeterOffset.y), SDL_FLIP_NONE, 0.0f);

		mCompletePMeterSprite->Render(Vector2D(350, 597), SDL_FLIP_NONE, 0.0f);
	}

 	if (mFontRenderer)
	{
		std::string outputValue;

		// ---------------------------------------------------------------------- //

		// First render the time remaining
		outputValue = std::to_string((int)mTimeRemaming);

		while (outputValue.size() < 3)
			outputValue.insert(outputValue.begin(), '0');

		mFontRenderer->RenderFromRight(outputValue, 10, mTimerCounterOffset);

		// ---------------------------------------------------------------------- //

		// Now render the money the player has
		mFontRenderer->RenderFromRight(std::to_string((int)mCurrentMoneyCount), 10, mMoneyCounterOffset);

		// ---------------------------------------------------------------------- //

		// Now render the current world ID
		mFontRenderer->RenderFromLeft(std::to_string(mCurrentWorldID), 1, mCurrentWorldOffset);

		// ---------------------------------------------------------------------- //

		// Now render the current live count
		mFontRenderer->RenderFromRight(std::to_string(mLivesRemaining), 10, mLivesCounterOffset);

		// ---------------------------------------------------------------------- //

		// Now render the current score
		outputValue = std::to_string((int)mCurrentScore);

		while(outputValue.size() < 7)
			outputValue.insert(outputValue.begin(), '0');

		mFontRenderer->RenderFromRight(outputValue, 7, mScoreCounterOffset);

		// ---------------------------------------------------------------------- //
	}
}

// ------------------------------------------------------------------------ //

void HUD_Display::Update(const float deltaTime)
{
	if (mTimeRemaming > 0.0f)
		mTimeRemaming -= deltaTime;


}

// ------------------------------------------------------------------------ //

void HUD_Display::Reset()
{
	mTimeRemaming      = 0.0f;
	mCurrentScore      = 0;
	mLivesRemaining    = 5;
	mCurrentMoneyCount = 0;

	for(unsigned int i = 0; i < 3; i++)
		mEndCards[i] = END_CARD_TYPES::EMPTY;
}

// ------------------------------------------------------------------------ //

void HUD_Display::OnNotify(SUBJECT_NOTIFICATION_TYPES notification, std::string data)
{
	std::stringstream dataLine(data);

	switch (notification)
	{
	default:return;

	case SUBJECT_NOTIFICATION_TYPES::ADD_LIFE:           mLivesRemaining++; return;
	case SUBJECT_NOTIFICATION_TYPES::TAKE_LIFE:          mLivesRemaining--; return;
	case SUBJECT_NOTIFICATION_TYPES::INCREMENT_WORLD_ID: mCurrentWorldID++; return;
	case SUBJECT_NOTIFICATION_TYPES::SET_PAUSED:         mPaused = true;    return;
	case SUBJECT_NOTIFICATION_TYPES::SET_UNPAUSED:       mPaused = false;   return;

	case SUBJECT_NOTIFICATION_TYPES::ADD_MONEY:
		unsigned int moneyAmount;
		dataLine >> moneyAmount;
		mCurrentMoneyCount += moneyAmount;
	return;

	case SUBJECT_NOTIFICATION_TYPES::ADD_SCORE:
		unsigned int scoreAmount;
		dataLine >> scoreAmount;
		mCurrentScore += scoreAmount;
	return;

	case SUBJECT_NOTIFICATION_TYPES::TAKE_SCORE:
		unsigned int minusScore;
		dataLine >> minusScore;
		mCurrentScore -= minusScore;
	return;

	case SUBJECT_NOTIFICATION_TYPES::ADD_END_CARD:
		unsigned int cardID;
		dataLine >> cardID;
		mEndCards[mCurrentEndCardCount] = (END_CARD_TYPES)cardID;
	return;

	case SUBJECT_NOTIFICATION_TYPES::UPDATE_P_METER:
		unsigned int newValue;
		dataLine >> newValue;
		mPMeterFillAmount = newValue;
	return;
	}
}

// ------------------------------------------------------------------------ //