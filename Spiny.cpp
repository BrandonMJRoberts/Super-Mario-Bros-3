#include "Spiny.h"

#include "Texture2D.h"

#include "LevelMap.h"

Texture2D*   Spiny::mSpriteSheet = nullptr;
unsigned int Spiny::mSpinyCount = 0;

// --------------------------------------------------------- //

Spiny::Spiny(SDL_Renderer* renderer, const char* filePathToSpriteSheet, const float timePerFrame, Vector2D startPos, Vector2D collisionBox) 
	: RenderObject(0, 2, 0, timePerFrame, startPos, 5, 3, collisionBox)
{
	// Load in the sprite sheet
	if (mSpinyCount == 0)
	{
		mSpriteSheet = new Texture2D(renderer);
		if (!mSpriteSheet->LoadFromFile(filePathToSpriteSheet))
		{
			std::cout << "Failed to load sprite sheet for coins" << std::endl;
		}
	}

	mSpinyCount++;

	SetupRenderRects();
}

// --------------------------------------------------------- //

Spiny::~Spiny()
{
	if (mSpinyCount == 1)
	{
		delete mSpriteSheet;
		mSpriteSheet = nullptr;
	}

	mSpinyCount--;
}

// --------------------------------------------------------- //

void Spiny::UpdatePhysics(const float deltaTime, LevelMap* levelMap)
{
	Vector2D movementDistance = mVelocity * deltaTime;

	// Check for collisions
	if (levelMap->GetCollisionTileAt(int(mPosition.y + movementDistance.y), int(mPosition.x)) == '1'
		|| levelMap->GetCollisionTileAt(int(mPosition.y + movementDistance.y), int(mPosition.x + mCollisionBox.x)) == '1') // Down collision
	{
		mVelocity.y = 0.0f;
	}
	else if (levelMap->GetCollisionTileAt(int(mPosition.y - mCollisionBox.y - movementDistance.y), int(mPosition.x)) == '1' ||
		levelMap->GetCollisionTileAt(int(mPosition.y - mCollisionBox.y - movementDistance.y), int(mPosition.x + mCollisionBox.x)) == '1') // Up collision
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
}

// --------------------------------------------------------- //