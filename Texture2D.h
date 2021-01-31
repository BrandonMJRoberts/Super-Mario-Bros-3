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
	Texture2D(SDL_Renderer* renderer, bool hasAlpha = false);
	~Texture2D();

	bool LoadFromFile(std::string path);
	void Free();

	// Render functions
	void Render(const Vector2D newPosition, const SDL_RendererFlip flip, double angle = 0.0f) const;
	void Render(SDL_Rect portionOfSpriteSheet, SDL_Rect destRect, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0f);

	// Alpha functions
	void SetBlendMode(SDL_BlendMode blendMode);
	void SetAlpha(Uint8 alpha);

	int  GetHeight() const { return mHeight; }
	int  GetWidth()  const { return mWidth;  } 

	bool GetHasAlpha() const { return mHasAlpha; }

	std::string GetFilePath() const { return mFilePathToSpriteSheet; }

private:
	SDL_Renderer* mRenderer;
	SDL_Texture*  mTexture;

	std::string   mFilePathToSpriteSheet;

	int           mWidth;
	int           mHeight;

	bool		  mHasAlpha;
};

#endif // !_TEXTURE_2D_H
