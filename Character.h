#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#include <SDL.h>
#include <iostream>
#include "Commons.h"
#include "Game_Maths.h"

class Texture2D;

class Character abstract
{
public:
	Character() = delete;
	Character(SDL_Renderer* renderer, std::string imagePath, Vector2D startingPosition);
	~Character();

	virtual void Render();
	virtual void Update(float deltaTime, SDL_Event e);

	void     SetPosition(Vector2D newPos);
	Vector2D GetPosition() { return mPosition; }

	double GetCollisionRadius() { return mCollisionRadius; }

protected:
	void MoveLeft(float deltaTime);
	void MoveRight(float deltaTime);

	void AddGravity(float deltaTime);
	void Jump(float deltaTime);

	SDL_Renderer* mRenderer;
	Vector2D      mPosition;
	Texture2D*    mTexture;

	bool    mMovingLeft;
	bool    mMovingRight;

	bool    mJumping;
	bool    mCanJump;
	float   mJumpForce;

	FACING  mFacingDirection;

	double mCollisionRadius;

};

#endif _CHARACTER_H_