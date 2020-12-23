#include "BorderWorldMap.h"

#include "Texture2D.h"
#include "GameManager_SMB3.h"
#include "Constants_SMB3.h"

#include <SDL.h>

// --------------------------------------------------------------------------------- //

WorldMapBorder::WorldMapBorder(unsigned int width, unsigned int height, SDL_Renderer* renderer, Vector2D offset)
{
	mWidth  = width;
	mHeight = height;
	mOffset = offset;

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
		// First convert the actual position into a grid position
		Vector2D gridReferencePoint = Commons_SMB3::ConvertFromRealPositionToGridPositionReturn(GameManager_SMB3::GetInstance()->GetWorldMapRenderOffset(), RESOLUTION_OF_SPRITES);

		// Now get the distance from this position to the next one, so that we can scroll smoothly
		Vector2D interGridPositionOffset = Vector2D(((int(gridReferencePoint.x) - gridReferencePoint.x) * RESOLUTION_OF_SPRITES), ((int(gridReferencePoint.y) - gridReferencePoint.y) * RESOLUTION_OF_SPRITES));

		// Set the default values
		SDL_Rect portionOfSpriteSheet {0, 0, RESOLUTION_OF_SPRITES, RESOLUTION_OF_SPRITES};
		SDL_Rect destRect { (int)interGridPositionOffset.x + int(mOffset.x * RESOLUTION_OF_SPRITES),
			                (int)interGridPositionOffset.y + int(mOffset.y * RESOLUTION_OF_SPRITES),
							 RESOLUTION_OF_SPRITES,
						     RESOLUTION_OF_SPRITES };

		// Loop through from the game's current global offset
		for (int row = (int)gridReferencePoint.y; row < gridReferencePoint.y + BACKGROUND_SPRITE_RENDER_HEIGHT; row++)
		{
			// Checks to remove scope errors and to make sure we are only rendering what is on the screen
			if (row >= (int)mHeight || row < 0)
				continue;

			// Now loop through all of the columns of this row
			for (int col = (int)gridReferencePoint.x; col < gridReferencePoint.x + BACKGROUND_SPRITE_RENDER_WIDTH; col++)
			{
				// Bounds checks
				if (col >= (int)mWidth || col < 0)
				{
					// Make sure to add the offset on even if we are not rendering this column
					destRect.x += RESOLUTION_OF_SPRITES;
					continue;
				}

				// Only render if it is a border piece
				if (row == 0 || row == mHeight - 1 || col == 0 || col == mWidth - 1)
				{
					// Top left check
					if (row == 0)
					{
						if (col == 0)
						{
							// Then render the correct sprite based on the column and row
							portionOfSpriteSheet.x = 0;
							portionOfSpriteSheet.y = 0;
						}
						else if (col == mWidth - 1) // Top right check
						{
							portionOfSpriteSheet.x = 3 * RESOLUTION_OF_SPRITES;
							portionOfSpriteSheet.y = RESOLUTION_OF_SPRITES;
						}
						else // Top middle piece
						{
							portionOfSpriteSheet.x = 2 * RESOLUTION_OF_SPRITES;
							portionOfSpriteSheet.y = RESOLUTION_OF_SPRITES;
						}
					}
					else if (row == mHeight - 1) // Bottom check
					{
						if (col == 0)
						{
							// Then render the correct sprite based on the column and row
							portionOfSpriteSheet.x = 2 * RESOLUTION_OF_SPRITES;
							portionOfSpriteSheet.y = 0;
						}
						else if (col == mWidth - 1) // Top right check
						{
							portionOfSpriteSheet.x = 0;
							portionOfSpriteSheet.y = RESOLUTION_OF_SPRITES;
						}
						else // Bottom middle piece
						{
							portionOfSpriteSheet.x = 3 * RESOLUTION_OF_SPRITES;
							portionOfSpriteSheet.y = 0;
						}
					}
					else if (col == 0) // Left wall check
					{
						portionOfSpriteSheet.x = RESOLUTION_OF_SPRITES;
						portionOfSpriteSheet.y = 0;
					}
					else // Right wall check
					{
						portionOfSpriteSheet.x = RESOLUTION_OF_SPRITES;
						portionOfSpriteSheet.y = RESOLUTION_OF_SPRITES;
					}

					// Now render the sprite
					mSpriteSheet->Render(portionOfSpriteSheet, destRect);
				}

				destRect.x += RESOLUTION_OF_SPRITES;
			}

			// Move along the 
			destRect.y += RESOLUTION_OF_SPRITES;
			destRect.x  = (int)interGridPositionOffset.x + int(mOffset.y * RESOLUTION_OF_SPRITES);
		}
	}
	else
		std::cout << "Tried to render the background border without a sprite map!" << std::endl;
}

// --------------------------------------------------------------------------------- //