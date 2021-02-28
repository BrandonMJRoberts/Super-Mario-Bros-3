#include "VisualTransitionHandler.h"

#include <SDL.h>

// ------------------------------------------------------------------------------------------------------------ //

VisualTransitionHandler::VisualTransitionHandler(PlayableCharacter* currentPlayer, SDL_Renderer* renderer) 
	: mPlayerReference(currentPlayer), 
	  mFadeInOutTransition(nullptr)
	, mFadingOut(false)
	, mFadingIn(false)
{
	mFadeInOutTransition = new FadeInOutTransition(Vector2D(), 0.0f, "SDL_Mario_Project/Transitions/BlackFadeImage.png", renderer, FADING_STATE::IN, 200.0f);
}

// ------------------------------------------------------------------------------------------------------------ //

VisualTransitionHandler::~VisualTransitionHandler()
{
	mPlayerReference = nullptr;
}

// ------------------------------------------------------------------------------------------------------------ //

void VisualTransitionHandler::OnNotify(SUBJECT_NOTIFICATION_TYPES notification, const std::string dataLine)
{
	switch (notification)
	{
	case SUBJECT_NOTIFICATION_TYPES::LEVEL_CLEAR:
		//mFadingOut = true;

		//if(mFadeInOutTransition)
		//	mFadeInOutTransition->SetFadingState(FADING_STATE::OUT);
	break;

	case SUBJECT_NOTIFICATION_TYPES::ENTERING_PIPE:
		
		// Tell the player that they are entering a pipe and to act accordingly
		mPlayerReference->SetEnteringPipe(MOVEMENT_DIRECTION::DOWN);


	break;

	case SUBJECT_NOTIFICATION_TYPES::SETUP_MAIN_LEVEL:
		mFadingIn = true;
	break;

	}
}

// ------------------------------------------------------------------------------------------------------------ //

void VisualTransitionHandler::Update(const float deltaTime)
{
	if (mFadeInOutTransition)
	{
		mFadeInOutTransition->Update(deltaTime);
	}
}

// ------------------------------------------------------------------------------------------------------------ //

void VisualTransitionHandler::Render()
{
	if ((mFadingIn || mFadingOut) && mFadeInOutTransition)
	{
		mFadeInOutTransition->Render();
	}
}

// ------------------------------------------------------------------------------------------------------------ //