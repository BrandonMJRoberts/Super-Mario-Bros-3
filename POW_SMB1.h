#ifndef _POW_SMB1_H_
#define _POW_SMB1_H_

#include "RenderObject.h"

struct SDL_Renderer;
class LevelMap;

class POW final : public RenderObject
{
public:
	POW(SDL_Renderer* renderer, const char* filePathToSpriteSheet, Vector2D bottomLeftStartPos, Vector2D collisionBox = Vector2D(1.0f, 1.0f));
	~POW();

	bool     Update(const float deltaTime, LevelMap* levelMap) override;

	Vector2D GetCollisionBox()     const { return mCollisionBox; }
	Vector2D GetPosition()         const { return mPosition;     }
	Vector2D GetCollisionBoxOffset() const{ return mCollisionBoxOffset; }

	bool     SetHasBeenHit();

	void     UpdatePhysics(const float deltaTime, LevelMap* levelMap) { ; }

private:
	Texture2D* GetSpriteSheet() override { return mSpriteSheet; }

	Texture2D*   mSpriteSheet;

	Vector2D     mStartPosition;

	Vector2D	 mCollisionBox;
	Vector2D     mCollisionBoxOffset;

	bool	     mDoingBounce;
	bool		 mGoingUp;
};

#endif