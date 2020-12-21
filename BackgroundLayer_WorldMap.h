#ifndef _BACKGROUNDLAYER_WORLD_MAP_H_
#define _BACKGROUNDLAYER_WORLD_MAP_H_

#include <string>
#include <map>
#include <vector>
#include <SDL.h>

#include "Commons_SMB3.h"

class  Texture2D;
struct SDL_Renderer;

// ------------------------------------------------------------------------------------------------------------------------------------------ //

struct SpriteThatAnimatesData
{
public:
	SpriteThatAnimatesData(unsigned int index, unsigned int len, float timePerChange);

	unsigned int GetCurrentFrameOffset() { return mCurrentFrameOffset; }
	unsigned int GetDataValueIndex()     { return mDataValueIndex; }

	void Update(const float deltaTime);

private:
	unsigned int mAmountOfAnimationFrames;
	unsigned int mCurrentFrameOffset;

	float        mAnimationTimePerFrame;
	float        mTimeRemainingTillChange;
	unsigned int mDataValueIndex;
};

// ------------------------------------------------------------------------------------------------------------------------------------------ //

class BackgroundLayer_WorldMap final
{
public:
	BackgroundLayer_WorldMap() = delete;
	BackgroundLayer_WorldMap(const std::string filePath, SDL_Renderer* renderer, std::map<char, unsigned int> conversionTable);
	~BackgroundLayer_WorldMap();

	void           Render();
	void           Update(const float deltaTime, SDL_Event e);

	unsigned int   GetWidth()  { return mWidth; }
	unsigned int   GetHeight() { return mHeight; }

	void           ReplaceDataPoint(unsigned int row, unsigned int col, unsigned int newVal);

	unsigned int   GetSpecificDataPoint(unsigned int row, unsigned int col);
	unsigned int** GetDataStore() { return mBackgroundIndexStore; }

private:
	void LoadInDataFromFile(std::string filePath, std::map<char, unsigned int> conversionTable);

	Texture2D*     mSpriteSheet;

	unsigned int** mBackgroundIndexStore;

	unsigned int   mWidth;
	unsigned int   mHeight;
	unsigned int   mSpritesOnSpriteSheetWidth;
	unsigned int   mSpritesOnSpriteSheetHeight;
	std::vector<SpriteThatAnimatesData> mAnimatingSpriteData;

	Vector2D       mOffsetFromTopLeft;
};

// ------------------------------------------------------------------------------------------------------------------------------------------ //

#endif