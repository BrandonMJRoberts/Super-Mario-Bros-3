#include "TextRenderer.h"

#include "Texture2D.h"

// ------------------------------------------------------------------------- //

TextRenderer::TextRenderer(SDL_Renderer* renderer, const char* filePathToFont, const unsigned int spritesOnWidth, const unsigned int spritesOnHeight)
	: mSingleSpriteWidth(0)
	, mSingleSpriteHeight(0)
	, mFontSpriteSheet(nullptr)
	, mSourceRectPlaceholder()
	, mDestRectPlaceholder()
	, mSpritesOnWidth(spritesOnWidth)
	, mSpritesOnHeight(spritesOnHeight)
{
	mFontSpriteSheet = new Texture2D(renderer);
	mFontSpriteSheet->LoadFromFile(filePathToFont);

	if (mFontSpriteSheet)
	{
		mSingleSpriteWidth  = mFontSpriteSheet->GetWidth()  / spritesOnWidth;
		mSingleSpriteHeight = mFontSpriteSheet->GetHeight() / spritesOnHeight;

		mDestRectPlaceholder.w   = mSingleSpriteWidth;
		mSourceRectPlaceholder.w = mSingleSpriteWidth;

		mDestRectPlaceholder.h   = mSingleSpriteHeight;
		mSourceRectPlaceholder.h = mSingleSpriteHeight;
	}
}

// ------------------------------------------------------------------------- //

TextRenderer::~TextRenderer()
{
	delete mFontSpriteSheet;
	mFontSpriteSheet = nullptr;
}

// ------------------------------------------------------------------------- //

void TextRenderer::RenderFromLeft(const std::string textToRender, const unsigned int charactersPerLine, const Vector2D topLeftPosition)
{
	unsigned int spriteID;

	mDestRectPlaceholder.y = topLeftPosition.y;
	mDestRectPlaceholder.x = topLeftPosition.x;

	// Loop through all of the characters that are needed to be rendered
	for (unsigned int i = 0; i < textToRender.size(); i++)
	{
		// First convert the char into a sprite ID
	    spriteID = GetSpriteIDFromChar(textToRender[i]);

		// Now calculate the correct position on the sprite sheet
		mSourceRectPlaceholder.x =    (spriteID % mSpritesOnWidth) * mSingleSpriteWidth;
		mSourceRectPlaceholder.y = int(spriteID / mSpritesOnWidth) * mSingleSpriteHeight;

		// Now render the sprite
		mFontSpriteSheet->Render(mSourceRectPlaceholder, mDestRectPlaceholder, SDL_FLIP_NONE, 0.0f);

		// Shift the render position across
		mDestRectPlaceholder.x += mSingleSpriteWidth;
	}
}

// ------------------------------------------------------------------------- //

void TextRenderer::RenderFromRight(const std::string textToRender, const unsigned int charactersPerLine, const Vector2D topRightPosition)
{
	unsigned int spriteID;

	mDestRectPlaceholder.y = topRightPosition.y;
	mDestRectPlaceholder.x = topRightPosition.x;

	// Loop through all of the characters that are needed to be rendered
	for (int i = textToRender.size() - 1; i >= 0; i--)
	{
		// First convert the char into a sprite ID
		spriteID = GetSpriteIDFromChar(textToRender[i]);

		// Now calculate the correct position on the sprite sheet
		mSourceRectPlaceholder.x =    (spriteID % mSpritesOnWidth) * mSingleSpriteWidth;
		mSourceRectPlaceholder.y = int(spriteID / mSpritesOnWidth) * mSingleSpriteHeight;

		// Now render the sprite
		mFontSpriteSheet->Render(mSourceRectPlaceholder, mDestRectPlaceholder, SDL_FLIP_NONE, 0.0f);

		// Shift the render position across
		mDestRectPlaceholder.x -= mSingleSpriteWidth;
	}
}

// ------------------------------------------------------------------------- //

unsigned int TextRenderer::GetSpriteIDFromChar(char valueToGet)
{
	// First convert the char into an int
	int asciiValue = int(valueToGet);

	// First push it back so that our zero lines up with the ascii table's zero
	valueToGet -= 48;

	// If this char is not from 0->9 then we need to shift it a bit more
	if (valueToGet > 9)
		valueToGet -= 7;

	return valueToGet;
}

// ------------------------------------------------------------------------- //