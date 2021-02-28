#ifndef _FADE_IN_OUT_TRANSITION_H_
#define _FADE_IN_OUT_TRANSITION_H_

#include "Transition.h"

class FadeInOutTransition final : public Transition
{
public:
	FadeInOutTransition();
	FadeInOutTransition(const Vector2D startPos, const float frameTime, const char* filePathForSpriteSheet, SDL_Renderer* renderer, const FADING_STATE startFadingState, float alphaSpeed);
	~FadeInOutTransition();
	
	void Render() override;
	bool Update(const float deltaTime) override;

	void SetFadingState(FADING_STATE newState) { mFadingState = newState; }

private:

	const float  mAlphaFadeSpeed;
	float        mAccumulatedAlphaFade;

	Uint8        mCurrentAlpha;
	FADING_STATE mFadingState;
};

#endif