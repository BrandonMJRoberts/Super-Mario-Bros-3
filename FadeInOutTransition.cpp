#include "FadeInOutTransition.h"

// --------------------------------------------------------------------------- //

FadeInOutTransition::FadeInOutTransition() : Transition(), mCurrentAlpha(0), mFadingState(FADING_STATE::NONE), mAccumulatedAlphaFade(0), mAlphaFadeSpeed(0)
{

}

// --------------------------------------------------------------------------- //

FadeInOutTransition::FadeInOutTransition(const Vector2D startPos, const float frameTime, const char* filePathForSpriteSheet, SDL_Renderer* renderer, const FADING_STATE startFadingState, float alphaSpeed)
	: Transition(startPos, frameTime, filePathForSpriteSheet, renderer)
	, mCurrentAlpha(0)
	, mFadingState(startFadingState)
	, mAlphaFadeSpeed(alphaSpeed)
	, mAccumulatedAlphaFade(0)
{
	if (startFadingState == FADING_STATE::IN)
		mCurrentAlpha = 255;
	else
		mCurrentAlpha = 0;

	mSpriteSheet->SetBlendMode(SDL_BLENDMODE_BLEND);
	mSpriteSheet->SetAlpha(mCurrentAlpha);
}

// --------------------------------------------------------------------------- //

FadeInOutTransition::~FadeInOutTransition()
{

}

// --------------------------------------------------------------------------- //

void FadeInOutTransition::Render()
{
	mSpriteSheet->Render(Vector2D(), SDL_FLIP_NONE, 0.0f);
}

// --------------------------------------------------------------------------- //

bool FadeInOutTransition::Update(const float deltaTime)
{
	if (mFadingState != FADING_STATE::NONE)
	{
		mAccumulatedAlphaFade += (mAlphaFadeSpeed * deltaTime);

		if (mAccumulatedAlphaFade > 1.0f)
		{
			if (mFadingState == FADING_STATE::IN)
			{
				if ((int)mCurrentAlpha - (int)mAccumulatedAlphaFade < 0)
				{
					mCurrentAlpha = 0;
					mFadingState  = FADING_STATE::NONE;

					mSpriteSheet->SetAlpha(mCurrentAlpha);

					return true;
				}
				else
				{
					mCurrentAlpha -= (int)mAccumulatedAlphaFade;

					mAccumulatedAlphaFade -= (int)mAccumulatedAlphaFade;

					mSpriteSheet->SetAlpha(mCurrentAlpha);

					return false;
				}
			}
			else
			{
				if ((int)mCurrentAlpha - (int)mAccumulatedAlphaFade > 255)
				{
					mCurrentAlpha = 255;
					mFadingState  = FADING_STATE::NONE;

					mSpriteSheet->SetAlpha(mCurrentAlpha);

					return true;
				}
				else
				{
					mCurrentAlpha += (int)mAccumulatedAlphaFade;

					mAccumulatedAlphaFade -= (int)mAccumulatedAlphaFade;

					mSpriteSheet->SetAlpha(mCurrentAlpha);

					return false;
				}
			}
		}

		return false;
	}

	return true;
	
}

// --------------------------------------------------------------------------- //
