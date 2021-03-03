#include "BaseObject.h"

#include "Texture2D.h"
#include "ObjectLayer.h"

// ------------------------------------------------------------------------ //

BaseObject::BaseObject(const Vector2D spawnPosition, const bool startSpawnedInLevel)
: mCurrentPosition(0, 0)
, mSpawnPosition(spawnPosition)
, mIsCurrentlySpawnedInLevel(startSpawnedInLevel)
, mIsInstanceLocked(false)
{
	if (startSpawnedInLevel)
		mCurrentPosition = spawnPosition;
}

// ------------------------------------------------------------------------ //

BaseObject::~BaseObject()
{
}

// ------------------------------------------------------------------------ //

void BaseObject::Render(const Vector2D renderReferencePoint)
{

}

// ------------------------------------------------------------------------ //

bool BaseObject::Update(const float deltaTime, const Vector2D playerPosition, InteractableLayer* interactionLayer, ObjectLayer* objectLayer)
{
	return false;
}

// ------------------------------------------------------------------------ //