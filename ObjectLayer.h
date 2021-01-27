#ifndef _OBJECT_LAYER_H_
#define _OBJECT_LAYER_H_

#include <string>
#include <vector>
#include <map>

#include <SDL.h>

#include "Game_Maths.h"

#include "SpawnPoint_SMB3.h"

class BaseObject;
class InteractableLayer;

class ObjectLayer final
{
public:
	ObjectLayer() = delete;
	ObjectLayer(std::string filePathToDataFile, SDL_Renderer* renderer, InteractableLayer* interactionLayer);
	~ObjectLayer();

	void Render(const Vector2D gridReferencePoint);
	void Update(const float deltaTime, SDL_Event e, Vector2D playerPosition);

	Vector2D GetInitialSpawnPoint() const;
	Vector2D GetSpawnPoint(unsigned int spawnPointIndex) const;

private:
	bool LoadInDataFromFile(std::string filePath);
	bool InPlayArea(const Vector2D testPosition, const Vector2D gridReferencePoint);

	// Loops through all objects that have not been created and spawns them if needed
	void CheckIfObjectsShouldSpawn(const Vector2D gridReferencePoint);
	void UpdateSpawnedObjects(const float deltaTime, Vector2D playerPos);

	void InstantiateNameConversions();
	void DestroyAllNameConversions();

	// When spawned the objects get moved to the unspawned list - unless they are one of a select few exceptions (such as spawn points)
	std::vector<BaseObject*> mUnspawnedObjectsInLevel;
	std::vector<BaseObject*> mSpawnedObjectsInLevel;

	// Spawnpoints
	std::vector<SpawnPoint>  mSpawnPoints;

	std::map<std::string, BaseObject*> mNameToObjectConversion;

	// Renderer
	SDL_Renderer*                mRenderer;
	InteractableLayer*           mInteractionLayer;

};

#endif
