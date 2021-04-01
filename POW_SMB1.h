#ifndef _POW_SMB1_H_
#define _POW_SMB1_H_

#include "RenderObject.h"

struct SDL_Renderer;

class POW final : public RenderObject
{
public:
	POW(SDL_Renderer* renderer, const char* filePathToSpriteSheet, Vector2D bottomLeftStartPos);
	~POW();

	void     Update(const float deltaTime) override;

	Vector2D GetCollisionBox() const { return mCollisionBox; }
	Vector2D GetPosition()     const { return mPosition;     }

	bool     SetHasBeenHit();

private:
	Texture2D* GetSpriteSheet() override { return mSpriteSheet; }

	void UpdatePhysics() override { ; }

	Texture2D*   mSpriteSheet;

	Vector2D     mStartPosition;

	Vector2D	 mCollisionBox;

	bool	     mDoingBounce;
	bool		 mGoingUp;
};

#endif