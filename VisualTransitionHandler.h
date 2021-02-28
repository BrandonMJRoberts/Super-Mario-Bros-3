#ifndef _VISUAL_TRANSITION_HANDLER_H_
#define _VISUAL_TRANSITION_HANDLER_H_

#include "Observer.h"
#include "PlayableCharacter.h"

#include "FadeInOutTransition.h"

struct SDL_Renderer;

class VisualTransitionHandler final : public Observer
{

public:
	VisualTransitionHandler() = delete;
	VisualTransitionHandler(PlayableCharacter* currentPlayer, SDL_Renderer* renderer);
	~VisualTransitionHandler();

	void OnNotify(SUBJECT_NOTIFICATION_TYPES notification, const std::string dataLine) override;

	void SetCurrentPlayer(PlayableCharacter* newPlayerPtr) { mPlayerReference = newPlayerPtr; if (mPlayerReference) newPlayerPtr->AddObserver(this); }

	void Update(const float deltaTime);
	void Render();

private:
	PlayableCharacter* mPlayerReference;

	FadeInOutTransition*        mFadeInOutTransition;

	bool               mFadingOut;
	bool               mFadingIn;
};

#endif