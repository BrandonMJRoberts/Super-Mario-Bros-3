#include "BorderWorldMap.h"

#include "Texture2D.h"
#include "GameManager_SMB3.h"
#include "Constants_SMB3.h"

#include <SDL.h>

// --------------------------------------------------------------------------------- //

WorldMapBorder::WorldMapBorder(unsigned int width, unsigned int height, SDL_Renderer* renderer, Vector2D offset)
{
	mSpriteSheet = new Texture2D(renderer);
	if (!mSpriteSheet->LoadFromFile("SDL_Mario_Project/Worlds/BorderSprites.png"))
	{
		std::cout << "Failed to load the world map border image." << std::endl;
		return;
	}

	mWidth  = width;
	mHeight = height;

	mOffset = offset;
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
		Vector2D gridReferencePoint = Commons_SMB3::ConvertFromRealPositionToGridPositionReturn(GameManager_SMB3::GetInstance()->GetWorldMapRenderOffset() - Vector2D(1, 1), RESOLUTION_OF_SPRITES);

		// Now get the distance from this position to the next one, so that we can scroll smoothly
		Vector2D interGridPositionOffset = Vector2D(((int(gridReferencePoint.x) - gridReferencePoint.x) * RESOLUTION_OF_SPRITES), ((int(gridReferencePoint.y) - gridReferencePoint.y) * RESOLUTION_OF_SPRITES));

		SDL_Rect portionOfSpriteSheet, destRect;

		// Setup default values like this as SDL_Rect doesnt have a useful constructor
		portionOfSpriteSheet.x = 0;
		portionOfSpriteSheet.y = 0;
		portionOfSpriteSheet.w = RESOLUTION_OF_SPRITES;
		portionOfSpriteSheet.h = RESOLUTION_OF_SPRITES;

		destRect.x             = (int)interGridPositionOffset.x;
		destRect.y             = (int)interGridPositionOffset.y + int(mOffset.y * RESOLUTION_OF_SPRITES);
		destRect.w             = RESOLUTION_OF_SPRITES;
		destRect.h             = RESOLUTION_OF_SPRITES;

		// Loop through from the game's current global offset
		for (int row = (int)gridReferencePoint.y; row < gridReferencePoint.y + BACKGROUND_SPRITE_RENDER_HEIGHT; row++)
		{
			// Checks to remove scope errors and to make sure we are only rendering what is on the screen
			if (row >= (int)mHeight || row < 0)
				continue;

			for (int col = (int)(gridReferencePoint.x - mOffset.x); col < (gridReferencePoint.x + BACKGROUND_SPRITE_RENDER_WIDTH) - mOffset.x; col++)
			{
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

					mSpriteSheet->Render(portionOfSpriteSheet, destRect);
				}

				destRect.x += RESOLUTION_OF_SPRITES;
			}

			destRect.y += RESOLUTION_OF_SPRITES;
			destRect.x  = (int)interGridPositionOffset.x;
		}
	}
}

// --------------------------------------------------------------------------------- //