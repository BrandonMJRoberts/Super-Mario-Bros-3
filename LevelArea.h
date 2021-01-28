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

class Audio_Player;

class LevelAreas final
{
public:
	LevelAreas() = delete;
	LevelAreas(std::string areaFilePath, bool& isStartingArea, SDL_Renderer* renderer, std::map<char, unsigned int> ConversionFromCharToIntIndexMap, Audio_Player* audioPlayer);
	~LevelAreas();

	void Render(Vector2D gridReferencePoint);
	Area_Transition_Data Update(const float deltaTime, SDL_Event e, PlayableCharacter* player);

	unsigned int GetLevelWidth()  const { return mLevelWidth; }

	Vector2D     GetLevelBounds() const { return Vector2D(mLevelWidth, mLevelHeight); }

	Vector2D     GetInitialSpawnPoint() const;

	const Vector2D    GetSpawnPointPosition(unsigned int ID);

	InteractableLayer* GetInteractionLayer() { return mInteractableLayer; }
	ObjectLayer*       GetObjectLayer()      { return mObjectLayer; }

	void               PlayMusicForArea(Audio_Player* audioPlayer);

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