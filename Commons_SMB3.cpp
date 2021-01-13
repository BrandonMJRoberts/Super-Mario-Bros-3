#include "Commons_SMB3.h"

namespace Commons_SMB3
{
	void ConvertFromRealPositionToGridPositionRef(Vector2D& position, const unsigned int ResolutionOfSprites)
	{
		// The variable is passed in in pixels. We just need to divide by the pixel width and height of each grid element
		position.x /= ResolutionOfSprites;
		position.y /= ResolutionOfSprites;
	}

	// ------------------------------------------------ //

	Vector2D ConvertFromRealPositionToGridPositionReturn(Vector2D position, const unsigned int ResolutionOfSprites)
	{
		// The variable is passed in in pixels. We just need to divide by the pixel width and height of each grid element
		return Vector2D(position.x / ResolutionOfSprites, position.y / ResolutionOfSprites);
	}

	// ------------------------------------------------ //

	void ConvertFromGridPositionToRealPositionRef(Vector2D& position, const unsigned int ResolutionOfSprites)
	{
		// The variable is passed in in pixels. We just need to divide by the pixel width and height of each grid element
		position.x *= ResolutionOfSprites;
		position.y *= ResolutionOfSprites;
	}

	// ------------------------------------------------ //

	Vector2D ConvertFromGridPositionToRealPositionReturn(Vector2D position, const unsigned int ResolutionOfSprites)
	{
		// The variable is passed in in pixels. We just need to divide by the pixel width and height of each grid element
		return Vector2D(position.x * ResolutionOfSprites, position.y * ResolutionOfSprites);
	}

	// ------------------------------------------------ //

	void RemoveHyphonsFromString(char*& line, unsigned int length)
	{
		for (unsigned int i = 0; i < length; i++)
		{
			if (line[i] == '-')
				line[i] = ' ';
		}
	}

	// ------------------------------------------------ //

	void ReplaceHyphonsWithSpacesInString(char*& line, unsigned int length)
	{
		for (unsigned int i = 0; i < length; i++)
		{
			if (line[i] == ' ')
				line[i] = '-';
		}
	}

	// ------------------------------------------------ //
}