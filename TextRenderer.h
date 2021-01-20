#ifndef _TEXT_RENDERER_H_
#define _TEXT_RENDERER_H_

#include "Commons_SMB3.h"

class Texture2D;

#include <string>
#include <SDL.h>

class TextRenderer final
{
public:
	TextRenderer() = delete;
	TextRenderer(SDL_Renderer* renderer, const char* filePathToFont, const unsigned int spritesOnWidth, const unsigned int spritesOnHeight);
	~TextRenderer();

	void RenderFromRight(const std::string textToRender, const unsigned int charactersPerLine, const Vector2D topRightPosition);
	void RenderFromLeft(const std::string textToRender, const unsigned int charactersPerLine, const Vector2D topLeftPosition);


private:
	unsigned int GetSpriteIDFromChar(char valueToGet);

	Texture2D*   mFontSpriteSheet;

	unsigned int mSingleSpriteWidth;
	unsigned int mSingleSpriteHeight;

	unsigned int mSpritesOnWidth;
	unsigned int mSpritesOnHeight;

	SDL_Rect     mSourceRectPlaceholder;
	SDL_Rect     mDestRectPlaceholder;
};

#endif //_TEXT_RENDERER_H_