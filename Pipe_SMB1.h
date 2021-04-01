#ifndef _PIPE_SMB1_H_
#define _PIPE_SMB1_H_

#include "RenderObject.h"

struct SDL_Renderer;

// -------------------------------------------------------------------------------- //

enum class PIPE_FACING_DIRECTION_SMB1
{
	RIGHT = 0,
	LEFT,
};

// -------------------------------------------------------------------------------- //

class PIPE_SMB1 final : public RenderObject
{
public:
	PIPE_SMB1(SDL_Renderer* renderer, const char* filePathToSpriteSheet, PIPE_FACING_DIRECTION_SMB1 facingDirection, Vector2D bottomLeftPosition, const float timePerFrame);
	~PIPE_SMB1();

	void Update(const float deltaTime) override;

	void SetIsDoingAnimation(bool collisionFromLeft);

private:
	Texture2D* GetSpriteSheet() override { return mSpriteSheet; }

	void UpdatePhysics() override { ; }

	static Texture2D*   mSpriteSheet;
	static unsigned int mPipeCount;

	PIPE_FACING_DIRECTION_SMB1 mPipeFacingDirection;

	bool         mDoingAnimation;
	bool         mAnimationDirectionIsRight;
};

// -------------------------------------------------------------------------------- //

#endif