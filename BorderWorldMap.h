#ifndef _BORDER_WORLD_MAP_H_
#define _BORDER_WORLD_MAP_H_

#include "Commons_SMB3.h"

class Texture2D;
struct SDL_Renderer;

class WorldMapBorder final
{
public:
	WorldMapBorder() = delete;
	WorldMapBorder(unsigned int width, unsigned int height, SDL_Renderer* renderer);
	~WorldMapBorder();

	void Render();

private:
	Texture2D* mSpriteSheet;

	Vector2D mOffset;

	unsigned int mWidth;
	unsigned int mHeight;
};


#endif