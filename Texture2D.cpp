#include "Texture2D.h"
#include <SDL_image.h>
#include <iostream>

// -------------------------------------------------------------------------------------------------------------------------- //

Texture2D::Texture2D(SDL_Renderer* renderer, bool hasAlpha)
: mRenderer(renderer)
, mTexture(nullptr)
, mWidth(0)
, mHeight(0)
, mFilePathToSpriteSheet("")
, mHasAlpha(hasAlpha)
{
	if (mHasAlpha)
	{
		SetBlendMode(SDL_BLENDMODE_BLEND);
	}
}

// -------------------------------------------------------------------------------------------------------------------------- //

Texture2D::~Texture2D()
{
	Free();

	mRenderer = NULL;
}

// -------------------------------------------------------------------------------------------------------------------------- //

bool Texture2D::LoadFromFile(std::string path)
{
	Free();

	// Quick exit if nothing is passed in
	if (path == "")
		return false;

	// Load the image
	SDL_Surface* Surface = IMG_Load(path.c_str());
	if (Surface != NULL && mRenderer != NULL)
	{
		// Set the colour key - the transparent colour
		SDL_SetColorKey(Surface, SDL_TRUE, SDL_MapRGB(Surface->format, 0, 0xFF, 0xFF));

		mTexture = SDL_CreateTextureFromSurface(mRenderer, Surface);
		if (mTexture == NULL)
		{
			std::cout << "Unable to create the texture from surface: Error: " << SDL_GetError() << std::endl;
			std::cout << "File path: " << path << std::endl;
			return false;
		}

		mWidth  = Surface->w;
		mHeight = Surface->h;

		SDL_FreeSurface(Surface);

		// Store the file path so we can tell if this texture has already been loaded
		mFilePathToSpriteSheet = path;
	}
	else
	{
		std::cout << "Unable to create texture from surface: Error: " << IMG_GetError() << std::endl;
		std::cout << "File path: " << path << std::endl;
		return false;
	}

	return (mTexture != NULL);
}

// -------------------------------------------------------------------------------------------------------------------------- //

void Texture2D::Free()
{
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
	}

	mWidth  = 0;
	mHeight = 0;
}

// -------------------------------------------------------------------------------------------------------------------------- //

void Texture2D::Render(const Vector2D newPosition, SDL_RendererFlip flip, double angle) const
{
	SDL_Rect renderLocation = {(int)newPosition.x, (int)newPosition.y, mWidth, mHeight};

	SDL_RenderCopyEx(mRenderer, mTexture, NULL, &renderLocation, angle, NULL, flip);
}

// -------------------------------------------------------------------------------------------------------------------------- //

void Texture2D::Render(SDL_Rect srcRect, SDL_Rect destRect, SDL_RendererFlip flip, double angle)
{
	SDL_RenderCopyEx(mRenderer, mTexture, &srcRect, &destRect, angle, NULL, flip);
}

// -------------------------------------------------------------------------------------------------------------------------- //

void Texture2D::SetBlendMode(SDL_BlendMode blendMode)
{
	SDL_SetTextureBlendMode(mTexture, blendMode);
}

// -------------------------------------------------------------------------------------------------------------------------- //

void Texture2D::SetAlpha(Uint8 alpha)
{
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

// -------------------------------------------------------------------------------------------------------------------------- //