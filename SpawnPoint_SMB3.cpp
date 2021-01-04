#include "SpawnPoint_SMB3.h"

// ------------------------------------------------------------------------------------------------------------------------ //

SpawnPoint::SpawnPoint(const Vector2D spawnPosition, const bool startSpawnedInLevel, const unsigned int spawnPointIndex) 
: BaseObject(spawnPosition
, startSpawnedInLevel)
, mSpawnPointIndex(spawnPointIndex)
{

}

// ------------------------------------------------------------------------------------------------------------------------ //

SpawnPoint::~SpawnPoint()
{

}

// ------------------------------------------------------------------------------------------------------------------------ //

BaseObject* SpawnPoint::Clone(std::string dataForClone)
{
	return nullptr;
}

// ------------------------------------------------------------------------------------------------------------------------ //