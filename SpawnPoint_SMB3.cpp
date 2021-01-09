#include "SpawnPoint_SMB3.h"

#include <sstream>

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
	std::stringstream streamLine(dataForClone);

	Vector2D spawnPos;
	unsigned int entranceID;

	streamLine >> spawnPos.x >> spawnPos.y >> entranceID;

	return new SpawnPoint(spawnPos, true, entranceID);
}

// ------------------------------------------------------------------------------------------------------------------------ //