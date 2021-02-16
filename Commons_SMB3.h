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

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

enum class MOVEMENT_DIRECTION : char
{
	NONE = 0,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

enum class ITEM_TYPES : unsigned int
{
	MUSHROOM = 0,
	FIRE_FLOWER,


	P_SWITCH,

};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

enum class END_CARD_TYPES : char
{
	EMPTY = 0,
	MUSHROOM,
	FLOWER,
	STAR
};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

enum class SUBJECT_NOTIFICATION_TYPES
{
	TAKE_LIFE = 0,
	ADD_LIFE,
	ADD_SCORE,
	TAKE_SCORE,
	ADD_MONEY,
	ADD_END_CARD,

	UPDATE_P_METER,

	INCREMENT_WORLD_ID,

	SET_PAUSED,
	SET_UNPAUSED,

	SETUP_WORLD_MAP,
	SETUP_MAIN_LEVEL,
	SETUP_SUB_LEVEL,

	PLAY_WORLD_MAP_MUSIC,
	PLAY_MAIN_LEVEL_MUSIC,
	PLAY_SUB_AREA_MUSIC,

	PLAYER_MOVED_ON_WORLD_MAP,
	AIRSHIP_MOVES_ON_MAP,
	HAMMER_BROS_MOVE_AROUND_MAP,

	ENTERING_LEVEL,

	JUMPED_OFF_ENEMY,
	BROKEN_BLOCK,
	BUMPED_BLOCK,

	LEVEL_CLEAR,
	ENTERING_PIPE,
	BOSS_DEFEATED,
	COMPLETED_WORLD,
	GAME_OVER,

	BONUS_NO_MATCH,
	BONUS_MATCH,
	SPADE_MATCH,

	COIN_COLLECTED,

	FIRE_FIREBALL,
	FROG_MARIO_WALKS,

	LOW_TIMER,

	OPEN_INVENTORY,

	PLAYER_JUMPED,
	PLAYER_KICKED,

	INTO_NEW_WORLD_MAP,

	SPAWN_MUSHROOM,

	PLAYER_TRANSFORM_INTO_RACOON,
	PLAYER_SWIPE_OF_TAIL,
	TEXT_DISPLAYING,

	THWOMP_HIT_GROUND,
	CANNON_FIRES_CANNON_BALL,
	VINE_GROWS,
	USE_WARP_WISTLE,

	GAME_SELECT_COIN_SFX,
	PLAY_GAME_SELECT_MUSIC
};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

enum class LEVEL_TYPE
{
	OVERWORLD = 0,
	UNDER_WATER,
	OVERWORLD_2,

	AIR_SHIP,

	SPADE_PUZZLE,
	HAMMER_BROTHER,
	MINI_FORTRESS

};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

enum class PIPE_TYPE : char
{
	BLACK_BLUE = 0,
	DEFAULT_GREEN,
	BLACK_WHITE,
	LIGHT_GREEN,
	LIGHT_BROWN,
	DARK_BROWN,
	LIGHT_GREY,
	DARK_GREEN,
	DARK_GREY,
	BRIGHT_ORANGE,
	VERY_DARK_GREEN,
	WHITE_GREEN
};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

enum class FADING_STATE
{
	NONE = 0,
	IN,
	OUT
};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

struct Area_Transition_Data final
{
	int          areaToGoTo;
	unsigned int spawnpointIDToGoTo;
	bool         goToWorldMap;
	bool         LevelComplete;
};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

struct MovementPrevention
{
	MovementPrevention() { StopXMovement = true; StopYMovement = true; givesJump = false; }
	MovementPrevention(bool stopX, bool stopY, bool givesJump) { StopXMovement = stopX; StopYMovement = stopY; this->givesJump = givesJump; }

	bool StopYMovement;
	bool StopXMovement;
	bool givesJump;
};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

struct ObjectCollisionHandleData final
{
	ObjectCollisionHandleData() { shouldDeleteObject = false;        dimensionalMovementBlocking = MovementPrevention(); completedLevel = false; this->givesJumpLeway = false; }
	ObjectCollisionHandleData(bool deleteObject, bool stopMovementX, bool stopMovementY, bool LevelComplete, bool givesJumpLeway) { shouldDeleteObject = deleteObject; dimensionalMovementBlocking = MovementPrevention(stopMovementX, stopMovementY, givesJumpLeway); completedLevel = LevelComplete; this->givesJumpLeway = givesJumpLeway; }

	bool               shouldDeleteObject;
	bool               completedLevel;
	bool               givesJumpLeway;
	MovementPrevention dimensionalMovementBlocking;
};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

struct TwoDimensionalCollision final
{
	TwoDimensionalCollision()                                                                           { collisionDataPrimary = MOVEMENT_DIRECTION::NONE; collisionDataSecondary = MOVEMENT_DIRECTION::NONE; }
	TwoDimensionalCollision(MOVEMENT_DIRECTION primaryCollision, MOVEMENT_DIRECTION secondaryCollision, Vector2D playerPriorPos) { collisionDataPrimary = primaryCollision; collisionDataSecondary = secondaryCollision; playerPriorPosition = playerPriorPos; }

	MOVEMENT_DIRECTION collisionDataPrimary;
	MOVEMENT_DIRECTION collisionDataSecondary;
	Vector2D           playerPriorPosition;
};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

struct CollisionReturnData final
{
	CollisionReturnData(bool occured, TwoDimensionalCollision dir) { collisionOccured = occured; directionOfCollision = dir; }

	bool                    collisionOccured;
	TwoDimensionalCollision directionOfCollision;
};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

// For the common functions they need to be encapsulated in a namespace to avoid re-definition errors
namespace Commons_SMB3
{
	void ConvertFromRealPositionToGridPositionRef(Vector2D& position, const unsigned int ResolutionOfSprites);

	Vector2D ConvertFromRealPositionToGridPositionReturn(Vector2D position, const unsigned int ResolutionOfSprites);

	void ConvertFromGridPositionToRealPositionRef(Vector2D& position, const unsigned int ResolutionOfSprites);

	Vector2D ConvertFromGridPositionToRealPositionReturn(Vector2D position, const unsigned int ResolutionOfSprites);

	void RemoveHyphonsFromString(char*& line, unsigned int length);

	void ReplaceHyphonsWithSpacesInString(char*& line, unsigned int length);

	double Max(double val1, double val2);
	double Min(double val1, double val2);
}

// ------------------------------------------------ //

#endif