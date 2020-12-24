#ifndef _CONSTANTS_SMB3_H_
#define _CONSTANTS_SMB3_H_

// Actual window dimensions in pixels
#define SCREEN_WIDTH_SMB3 768
#define SCREEN_HEIGHT_SMB3 768

// Rendering boundary 
#define BACKGROUND_SPRITE_RENDER_WIDTH  18
#define BACKGROUND_SPRITE_RENDER_HEIGHT 12

// Used for spawning/despawning enemies
#define LEVEL_BOUNDING_AREA_WIDTH 18
#define LEVEL_BOUNDING_AREA_HEIGHT 14

// Pixel definition of sprites
#define RESOLUTION_OF_SPRITES 48

#define DOUBLE_RESOLUTION_OF_SPRITES (RESOLUTION_OF_SPRITES * 2)

// Capping size of levels
#define MAX_LEVEL_WIDTH 255
#define MAX_LEVEL_HEIGHT 63

// So that when files load in they dont go infinitally
#define FAILSAFE_MAX_COUNT_FILE_LOADING_LOOPS 300

#endif