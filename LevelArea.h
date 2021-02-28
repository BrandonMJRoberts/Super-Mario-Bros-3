#ifndef _LEVEL_AREA_H_
#define _LEVEL_AREA_H_

#include <string>
#include <map>
#include <SDL.h>

#include "Commons_SMB3.h"

#include <vector>

class BackgroundLayer;
class InteractableLayer;
class ObjectLayer;
class PlayableCharacter;
class Observer;

class LevelAreas final
{
public:
	LevelAreas() = delete;
	LevelAreas(std::string areaFilePath, bool& isStartingArea, SDL_Renderer* renderer, std::map<char, unsigned int> ConversionFromCharToIntIndexMap, std::vector<Observer*> observers);
	~LevelAreas();

	void Render(Vector2D gridReferencePoint);
	Area_Transition_Data Update(const float deltaTime, SDL_Event e, PlayableCharacter* player);

	unsigned int GetLevelWidth()  const { return mLevelWidth; }

	Vector2D     GetLevelBounds() const { return Vector2D(mLevelWidth, mLevelHeight); }

	Vector2D     GetInitialSpawnPoint() const;

	const Vector2D    GetSpawnPointPosition(unsigned int ID);

	InteractableLayer* GetInteractionLayer() { return mInteractableLayer; }
	ObjectLayer*       GetObjectLayer()      { return mObjectLayer; }

	std::string GetNameOfArea() const { return mNameOfArea; }

private:
	std::string ReplaceDoubleBackslashWithFrontSlash(std::string areaFilePath);
	std::string CalculateNameOfArea(std::string areaFilePath);

	std::string        mNameOfArea;

	unsigned int       mLevelWidth;
	unsigned int       mLevelHeight;

	BackgroundLayer*   mBackgroundLayer;
	BackgroundLayer*   mEndingSection;
	InteractableLayer* mInteractableLayer;
	ObjectLayer*       mObjectLayer;
};

#endif