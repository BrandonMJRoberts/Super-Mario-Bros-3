#include "SpawnPoint_SMB3.h"

#include <sstream>

// ------------------------------------------------------------------------------------------------------------------------ //

SpawnPoint::SpawnPoint(std::string dataLine) : mSpawnPointIndex(0), mPosition(0, 0)
{
	Instantiate(dataLine);
}

// ------------------------------------------------------------------------------------------------------------------------ //

SpawnPoint::~SpawnPoint()
{

}

// ------------------------------------------------------------------------------------------------------------------------ //

void SpawnPoint::Instantiate(std::string dataForClone)
{
	std::stringstream streamLine(dataForClone);

	streamLine >> mPosition.x >> mPosition.y >> mSpawnPointIndex;
}

// ------------------------------------------------------------------------------------------------------------------------ //