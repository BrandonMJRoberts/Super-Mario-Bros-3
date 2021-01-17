#ifndef _COIN_SMB3_H_
#define _COIN_SMB3_H_

#include "CollectableObject.h"

struct SDL_Renderer;

class Coin_SMB3 final : public CollectableObject
{
public:
	Coin_SMB3() = delete;
	Coin_SMB3(const Vector2D     spawnPosition, 
		      const bool         startSpawnedInLevel, 
		      SDL_Renderer*      renderer, 
		      const char* const  filePathToSpriteSheet, 
		      const unsigned int amountOfSpritesOnWidth, 
		      const unsigned int amountOfSpritesOnHeight,
			  const unsigned int collisionBoxWidth,
		      const unsigned int collisionBoxHeight,
		      const bool         collectableCanMove,
		      const float        timePerAnimationFrame);
	~Coin_SMB3() override;

	void Render(const Vector2D renderReferencePoint) override;
	bool Update(const float deltaTime, const Vector2D playerPosition) override;

	virtual BaseObject* Clone(std::string dataLine) override;

private:
};

#endif