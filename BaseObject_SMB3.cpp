#include "BaseObject_SMB3.h"

#include "Texture2D.h"

// ------------------------------------------------------------------------ //

BaseObject_SMB3::BaseObject_SMB3(const SDL_Renderer* renderer)
{
	// Set the default values
	mCurrentAnimationIndex    = 0;
	mFirstAnimationIndex      = 0;
	mLastAnimationIndex       = 0;
	mAmountOfSpritesWidth     = 0;
	mAmountOfSpritesHeight    = 0;

	mRenderer                 = nullptr;
	mSpriteSheet              = nullptr;

	mIsCurrentlySpawnedInLevel = false;
}

// ------------------------------------------------------------------------ //

BaseObject_SMB3::~BaseObject_SMB3()
{
	if (mSpriteSheet)
		delete mSpriteSheet;

	mSpriteSheet = nullptr;
	mRenderer    = nullptr;
}

// ------------------------------------------------------------------------ //

bool BaseObject_SMB3::Update(const float deltaTime)
{
	// This needs to handle the spawning/despawning of objects in the level area
	if (mIsCurrentlySpawnedInLevel)
	{
		// Now check if it is outside of the playable area

	}
	else
	{
		// Now check if it is within the playable area
	}

	return false;
}

// ------------------------------------------------------------------------ //

void BaseObject_SMB3::Render()
{

}

// ------------------------------------------------------------------------ //