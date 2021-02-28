#include "Transition.h"

#include <iostream>
#include <SDL.h>

// --------------------------------------------------------------------------- //

Transition::Transition() : kFrameTime(0.0f), mSpriteSheet(nullptr), mTimeTillFrame(0.0f)
{

}

// --------------------------------------------------------------------------- //

Transition::Transition(const Vector2D startPos, const float frameTime, const char* filePathForSpriteSheet, SDL_Renderer* renderer) 
	: kFrameTime(frameTime), 
	mPosition(startPos), 
	mSpriteSheet(nullptr), 
	mTimeTillFrame(frameTime)
{
	mSpriteSheet = new Texture2D(renderer);
	if (!mSpriteSheet->LoadFromFile(filePathForSpriteSheet))
	{
		std::cout << "Failed to load the transition sprite sheet: " << std::endl;
		return;
	}
}

// --------------------------------------------------------------------------- //

Transition::~Transition()
{
	delete mSpriteSheet;
	mSpriteSheet = nullptr;
}

// --------------------------------------------------------------------------- //

bool Transition::Update(const float deltaTime)
{
	mTimeTillFrame -= deltaTime;

	if (mTimeTillFrame <= 0.0f)
	{
		mTimeTillFrame = kFrameTime;
	}

	return true;
}

// --------------------------------------------------------------------------- //

void Transition::Render()
{
	// Render the sprite sheet in the correct position
	mSpriteSheet->Render(mPosition, SDL_FLIP_NONE, 0.0f);
}

// --------------------------------------------------------------------------- //