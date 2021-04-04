#include "Crab.h"

#include "Texture2D.h"
#include "LevelMap.h"

Texture2D*   Crab::mSpriteSheet = nullptr;
unsigned int Crab::mCrabCount = 0;

// ------------------------------------------------------ //

Crab::Crab(SDL_Renderer* renderer, const char* filePathToSpriteSheet, const float timePerFrame, Vector2D startPos, Vector2D collisionBox) 
	: RenderObject(0, 2, 0, timePerFrame, startPos, 7, 6, collisionBox)
{
	// Load in the sprite sheet
	if (mCrabCount == 0)
	{
		mSpriteSheet = new Texture2D(renderer);
		if (!mSpriteSheet->LoadFromFile(filePathToSpriteSheet))
		{
			std::cout << "Failed to load sprite sheet for coins" << std::endl;
		}
	}

	mCrabCount++;

	SetupRenderRects();
}

// ------------------------------------------------------ //

Crab::~Crab() 
{
	if (mCrabCount == 1)
	{
		delete mSpriteSheet;
		mSpriteSheet = nullptr;
	}

	mCrabCount--;
}

// ------------------------------------------------------ //

void Crab::UpdatePhysics(const float deltaTime, LevelMap* levelMap)
{
	Vector2D movementDistance = mVelocity * deltaTime;

	// Check for collisions
	if (   levelMap->GetCollisionTileAt(int(mPosition.y + movementDistance.y),  int(mPosition.x)) == 1 // left check
		|| levelMap->GetCollisionTileAt(int(mPosition.y + movementDistance.y),  int(mPosition.x + mCollisionBox.x)) == 1) // right check
	{
		mVelocity.y = 0.0f;
	}
	else if(levelMap->GetCollisionTileAt(int(mPosition.y + movementDistance.y),  int(mPosition.x)) == '1' ||  // left check
		    levelMap->GetCollisionTileAt(int(mPosition.y + movementDistance.y),  int(mPosition.x + mCollisionBox.x)) == '1') // right check
	{
		// Up collision bounce back down
		mVelocity.y = 1.0f;
	}
	else
	{
		mVelocity.y += CHARACTER_GRAVITY * deltaTime;

		// No y collisions
		mPosition.y += movementDistance.y;
	}

	// Now for x checks
	if (   levelMap->GetCollisionTileAt(int(mPosition.y - mCollisionBox.y), int(mPosition.x)) == 1  // top left check
		|| levelMap->GetCollisionTileAt(int(mPosition.y                  ), int(mPosition.x)) == 1) // bottom left check
	{
		mVelocity.x = 0.0f;
	}
	else if (levelMap->GetCollisionTileAt(int(mPosition.y - mCollisionBox.y), int(mPosition.x + mCollisionBox.x)) == 1 ||  // top right check
		     levelMap->GetCollisionTileAt(int(mPosition.y                  ), int(mPosition.x + mCollisionBox.x)) == 1) // bottom right check
	{
		// Up collision bounce back down
		mVelocity.x = 0.0f;
	}
	else
	{
		mVelocity.x = 3.0f;

		// No y collisions
		mPosition.x += movementDistance.x;
	}
}

// ------------------------------------------------------ //