#ifndef _LEVEL_AREA_H_
#define _LEVEL_AREA_H_

#include <string>
#include <map>
#include <SDL.h>

#include "Commons_SMB3.h"

class BackgroundLayer;
class InteractableLayer;
class ObjectLayer;
class PlayableCharacter;

class LevelAreas final
{
public:
	LevelAreas() = delete;
	LevelAreas(std::string areaFilePath, bool& isStartingArea, SDL_Renderer* renderer, std::map<char, unsigned int> ConversionFromCharToIntIndexMap);
	~LevelAreas();

	void Render(Vector2D gridReferencePoint);
	void Update(const float deltaTime, SDL_Event e, PlayableCharacter* player);

	unsigned int GetLevelWidth() const { return mLevelWidth; }

	Vector2D     GetInitialSpawnPoint() const;

private:
	std::string ReplaceDoubleBackslashWithFrontSlash(std::string areaFilePath);
	std::string CalculateNameOfArea(std::string areaFilePath);

	std::string        mNameOfArea;

	unsigned int       mLevelWidth;

	BackgroundLayer*   mBackgroundLayer;
	BackgroundLayer*   mEndingSection;
	InteractableLayer* mInteractableLayer;
	ObjectLayer*       mObjectLayer;
};

#endif