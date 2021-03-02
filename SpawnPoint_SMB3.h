#ifndef _SPAWN_POINT_SMB3_H_
#define _SPAWN_POINT_SMB3_H_

#include "BaseObject.h"

class SpawnPoint  
{
public:
	SpawnPoint() = delete;
	SpawnPoint(std::string dataLine);
	~SpawnPoint();

	void               Instantiate(std::string dataLineForClone);

	const Vector2D     GetPosition()                   const { return mPosition; }
	const unsigned int GetSpawnPointIndex()            const { return mSpawnPointIndex; }
	const bool         IsIndex(unsigned int testIndex) const { if (testIndex == mSpawnPointIndex) return true; return false; }

private:
	Vector2D     mPosition;
	unsigned int mSpawnPointIndex;
};

#endif