#include "BaseObject.h"

#include "Texture2D.h"

// ------------------------------------------------------------------------ //

BaseObject::BaseObject(const Vector2D spawnPosition, const bool startSpawnedInLevel)
: mCurrentPosition(0, 0)
, mSpawnPosition(spawnPosition)
, mIsCurrentlySpawnedInLevel(startSpawnedInLevel)
{
	if (startSpawnedInLevel)
		mCurrentPosition = spawnPosition;
}

// ------------------------------------------------------------------------ //

BaseObject::~BaseObject()
{
}

// ------------------------------------------------------------------------ //

void BaseObject::Render()
{

}

// ------------------------------------------------------------------------ //

bool BaseObject::Update(const float deltaTime, const Vector2D playerPosition)
{
	return false;
}

// ------------------------------------------------------------------------ //