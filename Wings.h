#ifndef _WINGS_H_
#define _WINGS_H_

#include "BaseAnimation.h"

struct SDL_Renderer;

class Wings final : public BaseAnimation
{
public:
	Wings() = delete;
	Wings(Vector2D startPos, SDL_Renderer* renderer);
	~Wings();

	bool Update(const float deltaTime) override;
	void Flap();
	void LoseWings();

	Texture2D* GetSpriteSheet()    override { return mThisSpriteSheet; }

	void SetPosition(const Vector2D newPos) { mPosition = newPos; }
	bool GetHasLostWings()            const { return mLostWings; }

private:
	static Texture2D*   mThisSpriteSheet;
	static unsigned int mThisObjectCounter;

	bool                mLostWings;
};

#endif