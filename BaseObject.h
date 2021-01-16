#ifndef _BASE_OBJECT_H_
#define _BASE_OBJECT_H_

#include "Game_Maths.h"

#include <iostream>
#include <string.h>

class BaseObject abstract
{
public:
	BaseObject() = delete;
	BaseObject(const Vector2D spawnPosition, const bool startSpawnedInLevel);
	virtual ~BaseObject();

	virtual BaseObject* Clone(std::string dataForNewObject) = 0;

	virtual void Render();
	virtual bool Update(const float deltaTime, const Vector2D playerPosition);

	bool         GetIsSpawnedInLevel() const            { return mIsCurrentlySpawnedInLevel; }
	void         SetIsSpawnedInLevel(const bool newVal) { mIsCurrentlySpawnedInLevel = newVal; }

	Vector2D     GetSpawnPosition()               const { return mSpawnPosition; }
	
	void         SetInstanceLocked(const bool newVal)   { mIsInstanceLocked = newVal; }
	bool         GetIsInstanceLocked()            const { return mIsInstanceLocked; }

protected:
	Vector2D            mCurrentPosition;
	Vector2D	        mSpawnPosition;

	bool                mIsCurrentlySpawnedInLevel;
	bool                mIsInstanceLocked;
};

#endif // !_BASE_OBJECT_SMB3_H_
