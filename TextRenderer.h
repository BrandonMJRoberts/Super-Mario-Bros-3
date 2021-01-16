#ifndef _TEXT_RENDERER_H_
#define _TEXT_RENDERER_H_

#include "Commons_SMB3.h"

class Texture2D;
struct SDL_Renderer;

#include <string>

class TextRenderer final
{
public:
	TextRenderer() = delete;
	TextRenderer(SDL_Renderer* renderer, const char* filePathToFont, const unsigned int spritesOnWidth, const unsigned int spritesOnHeight);
	~TextRenderer();

	void RenderFromRight(std::string textToRender, const unsigned int charactersPerLine, Vector2D topRightPosition);
	void RenderFromLeft(std::string textToRender, const unsigned int charactersPerLine, Vector2D topLeftPosition);


private:
	Texture2D*   mFontSpriteSheet;

	unsigned int mSingleSpriteWidth;
	unsigned int mSingleSpriteHeight;
};

#endif //_TEXT_RENDERER_H_