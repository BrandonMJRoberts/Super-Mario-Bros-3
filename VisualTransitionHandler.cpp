#include "VisualTransitionHandler.h"

#include <SDL.h>

#include <sstream>
#include <string>

// ------------------------------------------------------------------------------------------------------------ //

VisualTransitionHandler::VisualTransitionHandler(PlayableCharacter* currentPlayer, SDL_Renderer* renderer) 
	: mPlayerReference(currentPlayer), 
	  mFadeInOutTransition(nullptr)
	, mFadingOut(false)
	, mFadingIn(false)
	, mTimeRemainingForPipeTransition(0.0f)
	, mLevelAreaToGoTo(0)
	, mSpawnPointToGoTo(0)
	, mDoingPipeTransition(false)
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
	std::stringstream extractionLine;
	std::string direction;

	switch (notification)
	{
	case SUBJECT_NOTIFICATION_TYPES::LEVEL_CLEAR:
		mDoingPipeTransition = false;

		//mFadingOut = true;

		//if(mFadeInOutTransition)
		//	mFadeInOutTransition->SetFadingState(FADING_STATE::OUT);
	break;

	case SUBJECT_NOTIFICATION_TYPES::ENTERING_PIPE:
		
		extractionLine = std::stringstream(dataLine);
		
		// First get the level area we are going to, then the direction of transition we are doing
		extractionLine >> mLevelAreaToGoTo >> mSpawnPointToGoTo >> direction;

		// Tell the player that they are entering a pipe and to act accordingly
		if (direction == "DOWN")
		{
			mPlayerReference->SetEnteringPipe(MOVEMENT_DIRECTION::DOWN);
		}
		else if (direction == "UP")
		{
			mPlayerReference->SetEnteringPipe(MOVEMENT_DIRECTION::UP);
		}
		else if (direction == "RIGHT")
		{
			mPlayerReference->SetEnteringPipe(MOVEMENT_DIRECTION::RIGHT);
		}
		else if (direction == "LEFT")
		{
			mPlayerReference->SetEnteringPipe(MOVEMENT_DIRECTION::LEFT);
		}

		mTimeRemainingForPipeTransition = 2.0f;
		mDoingPipeTransition            = true;
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

	if (mDoingPipeTransition)
	{
		mTimeRemainingForPipeTransition -= deltaTime;
		if (mTimeRemainingForPipeTransition < 0.0f)
		{
			mTimeRemainingForPipeTransition = 0.0f;
		}
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

Area_Transition_Data VisualTransitionHandler::GetTransitionData()
{
	if (mDoingPipeTransition && mTimeRemainingForPipeTransition <= 0.0f)
	{
		mDoingPipeTransition = false;
		return Area_Transition_Data{ (int)mLevelAreaToGoTo, mSpawnPointToGoTo, false, false };
	}

	return Area_Transition_Data{ -1, 0, false, false };
}

// ------------------------------------------------------------------------------------------------------------ //