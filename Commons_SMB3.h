#ifndef _COMMONS_SMB3_H_
#define _COMMONS_SMB3_H_

#include "Game_Maths.h"

// ------------------------------------------------ //

enum class POWER_UP_TYPE : char
{
	MUSHROOM     = 0x00,
	FIRE_FLOWER  = 0x01,
	FROG_SUIT    = 0x02,
	HAMMER_SUIT  = 0x04,
	STAR         = 0x08,
	SUPER_LEAF   = 0x10,
	TANOOKI_SUIT = 0x20,
	NONE         = 0x40
};

// ------------------------------------------------ //

enum class SCREENS_SMB3 : char
{
	WORLD_MAP = 0x00,
	LEVEL     = 0x01,
	MAIN_MENU = 0x02,
	SAME      = 0x04
};


// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

enum class CHARACTER_MAP_POWER_UP_STATE : char
{
	SMALL     = 0x00,
	MUSHROOM  = 0x01,
	FROG      = 0x02,
	HAMMER    = 0x04,
	FIRE      = 0x08,
	TANOOKI   = 0x10,
	P_TANOOKI = 0x20,
	LEAF      = 0x40
};

// ------------------------------------------------ //

// For the common functions they need to be encapsulated in a namespace to avoid re-definition errors
namespace Commons_SMB3
{
	void ConvertFromRealPositionToGridPositionRef(Vector2D& position, const unsigned int ResolutionOfSprites);

	Vector2D ConvertFromRealPositionToGridPositionReturn(Vector2D position, const unsigned int ResolutionOfSprites);

	void ConvertFromGridPositionToRealPositionRef(Vector2D& position, const unsigned int ResolutionOfSprites);

	Vector2D ConvertFromGridPositionToRealPositionReturn(Vector2D position, const unsigned int ResolutionOfSprites);
}

// ------------------------------------------------ //

#endif