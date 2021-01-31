#ifndef _TRANSITION_H_
#define _TRANSITION_H_

#include "Commons_SMB3.h"
#include "Texture2D.h"

struct SDL_Renderer;

class Transition abstract
{
public:
	Transition();
	Transition(const Vector2D startPos, const float frameTime, const char* filePathForSpriteSheet, SDL_Renderer* renderer);
	virtual ~Transition();

	virtual void Render();
	virtual bool Update(const float deltaTime);

protected:
	Vector2D    mPosition;
	Texture2D*  mSpriteSheet;

	float       mTimeTillFrame;
	const float kFrameTime;
};

#endif