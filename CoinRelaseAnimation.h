#ifndef _COIN_RELEASE_ANIMATION_H_
#define _COIN_RELEASE_ANIMATION_H_

#include "Game_Maths.h"

#include "BaseAnimation.h"

class  Texture2D;
struct SDL_Renderer;

class CoinReleaseAnimation final : public BaseAnimation
{
public:
	CoinReleaseAnimation() = delete;
	CoinReleaseAnimation(Vector2D startPos, SDL_Renderer* renderer);
	~CoinReleaseAnimation();

	bool Update(const float deltaTime) override;

	Texture2D* GetSpriteSheet() override { return mThisSpriteSheet; }

	void       SetReleased();

private:
	static Texture2D* mThisSpriteSheet;
	static unsigned int mThisObjectCounter;

	bool mReleased;
};

#endif