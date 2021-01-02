#ifndef _OBJECT_LAYER_H_
#define _OBJECT_LAYER_H_

#include <string>
#include <vector>
#include <map>

#include <SDL.h>

#include "Game_Maths.h"

class BaseObject;

class ObjectLayer final
{
public:
	ObjectLayer() = delete;
	ObjectLayer(std::string filePathToDataFile, SDL_Renderer* renderer);
	~ObjectLayer();

	void Render();
	void Update(const float deltaTime, SDL_Event e, Vector2D playerPosition);

	Vector2D GetInitialSpawnPoint() const;
	Vector2D GetSpawnPoint(unsigned int spawnPointIndex) const;

private:
	bool LoadInDataFromFile(std::string filePath);

	// Loops through all objects that have not been created and spawns them if needed
	void CheckIfShouldSpawnObject();
	void CheckForDestroyingObjects();

	void InstantiateNameConversions();
	void DestroyAllNameConversions();

	// When spawned the objects get moved to the unspawned list - unless they are one of a select few exceptions (such as spawn points)
	std::vector<BaseObject*> mUnspawnedObjectsInLevel;
	std::vector<BaseObject*> mSpawnedObjectsInLevel;

	std::map<std::string, BaseObject*> mNameToObjectConversion;

	SDL_Renderer*                mRenderer;

};

#endif
