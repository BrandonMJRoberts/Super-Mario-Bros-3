#include "TextRenderer.h"

#include "Texture2D.h"
#include <SDL.h>

// ------------------------------------------------------------------------- //

TextRenderer::TextRenderer(SDL_Renderer* renderer, const char* filePathToFont, const unsigned int spritesOnWidth, const unsigned int spritesOnHeight)
	: mSingleSpriteWidth(0)
	, mSingleSpriteHeight(0)
	, mFontSpriteSheet(nullptr)
{
	mFontSpriteSheet = new Texture2D(renderer);
	mFontSpriteSheet->LoadFromFile(filePathToFont);

	if (mFontSpriteSheet)
	{
		mSingleSpriteWidth  = mFontSpriteSheet->GetWidth()  / spritesOnWidth;
		mSingleSpriteHeight = mFontSpriteSheet->GetHeight() / spritesOnHeight;
	}
}

// ------------------------------------------------------------------------- //

TextRenderer::~TextRenderer()
{
	delete mFontSpriteSheet;
	mFontSpriteSheet = nullptr;
}

// ------------------------------------------------------------------------- //

void TextRenderer::RenderFromLeft(std::string textToRender, const unsigned int charactersPerLine, Vector2D topLeftPosition)
{

}

// ------------------------------------------------------------------------- //

void TextRenderer::RenderFromRight(std::string textToRender, const unsigned int charactersPerLine, Vector2D topRightPosition)
{

}

// ------------------------------------------------------------------------- //