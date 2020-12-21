#ifndef _LEVEL_AREA_H_
#define _LEVEL_AREA_H_

#include <string>
#include <map>
#include <SDL.h>

class BackgroundLayer;
class InteractableLayer;
class ObjectLayer;

class LevelAreas final
{
public:
	LevelAreas() = delete;
	LevelAreas(std::string areaFilePath, bool& isStartingArea, SDL_Renderer* renderer, std::map<char, unsigned int> ConversionFromCharToIntIndexMap);
	~LevelAreas();

	void Render();
	void Update(const float deltaTime, SDL_Event e);

	unsigned int GetLevelWidth() { return mLevelWidth; }

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