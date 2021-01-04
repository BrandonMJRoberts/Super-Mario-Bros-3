#ifndef _SPAWN_POINT_SMB3_H_
#define _SPAWN_POINT_SMB3_H_

#include "BaseObject.h"

class SpawnPoint final : public BaseObject 
{
public:
	SpawnPoint() = delete;
	SpawnPoint(const Vector2D spawnPosition, const bool startSpawnedInLevel, const unsigned int spawnPointIndex);
	~SpawnPoint() override;

	BaseObject* Clone(std::string dataLineForClone) override;

	const unsigned int GetSpawnPointIndex()            const { return mSpawnPointIndex; }
	const bool         IsIndex(unsigned int testIndex) const { if (testIndex == mSpawnPointIndex) return true; return false; }

private:
	const unsigned int mSpawnPointIndex;
};

#endif