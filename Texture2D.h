#ifndef _TEXTURE_2D_H
#define _TEXTURE_2D_H

#include <SDL.h>

#include <iostream>
#include "Commons.h"
#include "Commons_SMB3.h"

class Texture2D
{
public:
	Texture2D() = delete;
	Texture2D(SDL_Renderer* renderer);
	~Texture2D();

	bool LoadFromFile(std::string path);
	void Free();
	void Render(Vector2D newPosition, SDL_RendererFlip flip, double angle = 0.0f);
	void Render(SDL_Rect portionOfSpriteSheet, SDL_Rect destRect, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0f);

	int GetHeight() { return mHeight; }
	int GetWidth()  { return mWidth;  } 

private:
	SDL_Renderer* mRenderer;
	SDL_Texture*  mTexture;

	int mWidth;
	int mHeight;
};

#endif // !_TEXTURE_2D_H
