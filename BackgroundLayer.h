#ifndef _BACKGROUND_LAYER_H_
#define _BACKGROUND_LAYER_H_

#include <string>
#include <map>
#include "Commons_SMB3.h"

class  Texture2D;
struct SDL_Renderer;

// The background - purely for visuals
class BackgroundLayer final
{
public:
	BackgroundLayer() = delete;
	BackgroundLayer(std::string filePathToDataFile, std::string filePathToSpriteSheet, std::map<char, unsigned int> lookupConversion, SDL_Renderer* renderer, Vector2D offset);
	~BackgroundLayer();

	void         Render();
	int          GetLevelEndingType()    { return mLevelEndType; }
	unsigned int GetLevelWidth()         { return mLevelWidth;   }
	unsigned int GetLevelHeight()        { return mLevelHeight; }

private:
	bool LoadInDataFromFile(std::string filePath, std::map<char, unsigned int> lookupConversion);
	bool CheckAllDataLoaded();

	const char*    mFilePathToSpriteSheet;
	Texture2D*     mSpriteSheet;

	unsigned int** mBackgroundSpriteIndexStore;

	unsigned int   mLevelWidth;  // Read in from the data file
	unsigned int   mLevelHeight; // Read in from the data file
	int            mLevelEndType;

	unsigned int   mSpriteOnSpriteSheetWidth;  // Read in from the data file
	unsigned int   mSpriteOnSpriteSheetHeight; // Read in from the data file

	unsigned int   mAountOfSpritesOnSpriteSheetWidth;
	unsigned int   mAountOfSpritesOnSpriteSheetHeight;

	Vector2D       mOffsetFromTopLeft;
};

#endif