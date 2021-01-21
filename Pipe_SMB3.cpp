#include "Pipe_SMB3.h"

unsigned int Pipe::mCurrentSpriteID = 0;
unsigned int Pipe::mStartSpriteID = 0;
unsigned int Pipe::mEndSpriteID = 3;

float        Pipe::mTimeRemainingTillNextFrame = 0.0f;

bool         Pipe::mUpdatedStaticVariables = false;

// ----------------------------------------------------------------------------------------- //

Pipe::Pipe(const Vector2D      spawnPosition,
	const bool          startSpawnedInLevel,
	SDL_Renderer*       renderer,
	const std::string   filePathToSpriteSheet,
	const unsigned int  spritesOnWidth,
	const unsigned int  spritesOnHeight,
	const double        collisionBoxWidth,
	const double        collisionBoxHeight,
	const float		    timePerFrame)

: PhysicalObject(spawnPosition
, startSpawnedInLevel
, renderer
, filePathToSpriteSheet
, spritesOnWidth
, spritesOnHeight
, collisionBoxWidth
, collisionBoxHeight
, timePerFrame)
, mTimePerFrame(timePerFrame)
{

}

// ----------------------------------------------------------------------------------------- //

Pipe::~Pipe()
{

}

// ----------------------------------------------------------------------------------------- //

BaseObject* Pipe::Clone(std::string dataLineForClone)
{
	return nullptr;
}

// ----------------------------------------------------------------------------------------- //

bool Pipe::Update(const float deltaTime, const Vector2D playerPosition)
{
	return false;
}

// ----------------------------------------------------------------------------------------- //

void Pipe::Render(const Vector2D renderReferencePoint)
{

}

// ----------------------------------------------------------------------------------------- //