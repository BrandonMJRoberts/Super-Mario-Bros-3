#ifndef _LEVEL_MAP_H_
#define _LEVEL_MAP_H_

#include "Constants.h"
#include "Texture2D.h"

struct SDL_Renderer;

class LevelMap final
{
public:
	LevelMap(const char* filePathToCollisionMap, const char* filePathToSpriteSheet, SDL_Renderer* renderer);
	~LevelMap();

	int  GetCollisionTileAt(unsigned int h, unsigned int w);

	void LoadCollisionMapFromFile(const char* filePath);

	void Render();

	unsigned int GetLevelWidth()  const { return mLevelWidth; }
	unsigned int GetLevelHeight() const { return mLevelHeight; }

private:
	SDL_Renderer* mRenderer;

	Texture2D* mBackgroundSpriteSheet;

	int**         mCollisionMap;

	unsigned int  mLevelWidth;
	unsigned int  mLevelHeight;
};

#endif