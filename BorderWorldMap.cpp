#include "BorderWorldMap.h"

#include "Texture2D.h"
#include "GameManager_SMB3.h"
#include "Constants_SMB3.h"

#include <SDL.h>

// --------------------------------------------------------------------------------- //

WorldMapBorder::WorldMapBorder(unsigned int width, unsigned int height, SDL_Renderer* renderer)
{
	mWidth  = width;
	mHeight = height;

	mSpriteSheet = new Texture2D(renderer);
	if (!mSpriteSheet->LoadFromFile("SDL_Mario_Project/Worlds/BorderSprites.png"))
	{
		std::cout << "Failed to load the world map border image." << std::endl;
		return;
	}
}

// --------------------------------------------------------------------------------- //

WorldMapBorder::~WorldMapBorder()
{
	delete mSpriteSheet;
	mSpriteSheet = nullptr;
}

// --------------------------------------------------------------------------------- //

void WorldMapBorder::Render()
{
	if (mSpriteSheet)
	{
		SDL_Rect portionOfSpriteSheet{ 0, 0, RESOLUTION_OF_SPRITES, RESOLUTION_OF_SPRITES };
		SDL_Rect destRect            { 0, 0, RESOLUTION_OF_SPRITES, RESOLUTION_OF_SPRITES };

		// Loop through the area that we want to render
		for (int row = 0; row < int(mHeight); row++)
		{
			// As we only want to render the border then check if we are in the border
			if (row == 0 || row == mHeight - 1)
			{
				for (int col = 0; col < int(mWidth); col++)
				{
					if (row == 0)
					{
						if (col == 0)
						{
							portionOfSpriteSheet.x = 0;
							portionOfSpriteSheet.y = 0;
						}
						else if (col == mWidth - 1)
						{
							portionOfSpriteSheet.x = 3 * RESOLUTION_OF_SPRITES;
							portionOfSpriteSheet.y = RESOLUTION_OF_SPRITES;
						}
						else
						{
							portionOfSpriteSheet.x = 2 * RESOLUTION_OF_SPRITES;
							portionOfSpriteSheet.y = RESOLUTION_OF_SPRITES;
						}
					}
					else
					{
						if (col == 0)
						{
							portionOfSpriteSheet.x = 2 * RESOLUTION_OF_SPRITES;
							portionOfSpriteSheet.y = 0;
						}
						else if (col == mWidth - 1)
						{
							portionOfSpriteSheet.x = 0;
							portionOfSpriteSheet.y = RESOLUTION_OF_SPRITES;
						}
						else
						{
							portionOfSpriteSheet.x = 3 * RESOLUTION_OF_SPRITES;
							portionOfSpriteSheet.y = 0;
						}
					}

					// Render the sprite
					mSpriteSheet->Render(portionOfSpriteSheet, destRect);

					// Move the rendering position on in the x-Axis
					destRect.x += RESOLUTION_OF_SPRITES;
				}
			}
			else
			{
				// First render the left wall
				portionOfSpriteSheet.x = RESOLUTION_OF_SPRITES;
				portionOfSpriteSheet.y = 0;

				mSpriteSheet->Render(portionOfSpriteSheet, destRect);

				// Now render the right wall
				portionOfSpriteSheet.x = RESOLUTION_OF_SPRITES;
				portionOfSpriteSheet.y = RESOLUTION_OF_SPRITES;

				destRect.x = (mWidth - 1) * RESOLUTION_OF_SPRITES;

				mSpriteSheet->Render(portionOfSpriteSheet, destRect);
			}

			destRect.y += RESOLUTION_OF_SPRITES;
			destRect.x = 0;
		}
	}
	else
	{
		std::cout << "Tried to render the background border without a sprite map!" << std::endl;
	}
}

// --------------------------------------------------------------------------------- //