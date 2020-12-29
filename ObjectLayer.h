#ifndef _OBJECT_LAYER_H_
#define _OBJECT_LAYER_H_

#include <string>
#include <vector>

#include "BaseObject_SMB3.h"

class ObjectLayer final
{
public:
	ObjectLayer() = delete;
	ObjectLayer(std::string filePathToDataFile, SDL_Renderer* renderer);
	~ObjectLayer();

	void Render(const Vector2D gridReferencePoint);
	void Update(const float deltaTime, SDL_Event e);

	Vector2D GetInitialSpawnPoint() const;
	Vector2D GetSpawnPoint(unsigned int spawnPointIndex) const;

private:
	bool LoadInDataFromFile(std::string filePath);

	// Loops through all objects that have not been created and spawns them if needed
	void CheckIfShouldSpawnObject();
	void CheckForDestroyingObjects();

	// When spawned the objects get moved to the unspawned list - unless they are one of a select few exceptions (such as spawn points)
	std::vector<BaseObject_SMB3> mUnspawnedObjectsInLevel;
	std::vector<BaseObject_SMB3> mSpawnedObjectsInLevel;

	SDL_Renderer*                mRenderer;

};

#endif
