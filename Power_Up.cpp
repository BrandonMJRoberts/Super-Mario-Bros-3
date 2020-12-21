#include "Power_Up.h"
#include "Texture2D.h"

// --------------------------------------------------------------------------------------------------------- //

PowerUp::PowerUp()
{
	// Setting the default starting values
	mVelocity		           = Vector2D();
	mVelocity		           = Vector2D();

	mRenderer		           = nullptr;
	mSprite			           = nullptr;

	mPowerUpType               = POWER_UP_TYPE::NONE;

	mIsFacingRight             = true;
	mIsUsingGravity			   = false;

	mCurrentSpriteIndex		   = 0;
	mFinalAnimationSpriteIndex = 0;
	mStartAnimationSpriteIndex = 0;

	mHeight = 0;
	mWidth  = 0;
}

// --------------------------------------------------------------------------------------------------------- //

PowerUp::PowerUp(SDL_Renderer* renderer, const Vector2D startPosition, const POWER_UP_TYPE startPowerUpType, const Vector2D startVelocity)
{
	// Setting the starting values
	mVelocity      = startVelocity;
	mPosition      = startPosition;
	mRenderer	   = renderer;
	mPowerUpType   = startPowerUpType;
	mIsFacingRight = true;
	mWidth         = 32; // May need to change in the future
	mHeight        = 32; // May need to change in the future

	// Setting this power up's specific data
	switch (startPowerUpType)
	{
	case POWER_UP_TYPE::NONE:
		mCurrentSpriteIndex = 0;
		mFinalAnimationSpriteIndex = 0;
		mStartAnimationSpriteIndex = 0;

		mIsUsingGravity = false;
	break;

	case POWER_UP_TYPE::FIRE_FLOWER:
		mCurrentSpriteIndex        = 0;
		mFinalAnimationSpriteIndex = 0;
		mStartAnimationSpriteIndex = 0;

		mIsUsingGravity = false;
	break;

	case POWER_UP_TYPE::FROG_SUIT:
		mCurrentSpriteIndex		   = 0;
		mFinalAnimationSpriteIndex = 0;
		mStartAnimationSpriteIndex = 0;

		mIsUsingGravity            = false;
	break;

	case POWER_UP_TYPE::TANOOKI_SUIT:
		mCurrentSpriteIndex		   = 0;
		mFinalAnimationSpriteIndex = 0;
		mStartAnimationSpriteIndex = 0;

		mIsUsingGravity			   = false;
	break;

	case POWER_UP_TYPE::HAMMER_SUIT:
		mCurrentSpriteIndex		   = 0;
		mFinalAnimationSpriteIndex = 0;
		mStartAnimationSpriteIndex = 0;

		mIsUsingGravity			   = false;
	break;

	case POWER_UP_TYPE::MUSHROOM:
		mCurrentSpriteIndex		   = 0;
		mFinalAnimationSpriteIndex = 0;
		mStartAnimationSpriteIndex = 0;

		mIsUsingGravity			   = true;
	break;

	case POWER_UP_TYPE::STAR:
		mCurrentSpriteIndex		   = 0;
		mFinalAnimationSpriteIndex = 0;
		mStartAnimationSpriteIndex = 0;

		mIsUsingGravity			   = true;
	break;

	case POWER_UP_TYPE::SUPER_LEAF:
		mCurrentSpriteIndex		   = 0;
		mFinalAnimationSpriteIndex = 0;
		mStartAnimationSpriteIndex = 0;

		mIsUsingGravity			   = true;
	break;

	default:
		mCurrentSpriteIndex		   = 0;
		mFinalAnimationSpriteIndex = 0;
		mStartAnimationSpriteIndex = 0;

		mIsUsingGravity			   = false;
	break;
	}

	// Setup the sprite 
	mSprite = new Texture2D(mRenderer);
	if (!mSprite->LoadFromFile("SDL_Mario_Project/Power Ups/SpriteSheet.png"))
	{
		std::cout << "Failed to load the sprite file path for a Power Up: SDL_Mario_Project/Power Ups/SpriteSheet.png" << std::endl;
	}
}

// --------------------------------------------------------------------------------------------------------- //

PowerUp::~PowerUp()
{
	delete mSprite;
	mSprite = nullptr;
}

// --------------------------------------------------------------------------------------------------------- //

void PowerUp::Render()
{
	if(mIsFacingRight)
		mSprite->Render(mPosition, SDL_FLIP_NONE, 0.0f);
	else
		mSprite->Render(mPosition, SDL_FLIP_HORIZONTAL, 0.0f);
}

// --------------------------------------------------------------------------------------------------------- //

void PowerUp::Update(const float deltaTime)
{

}

// --------------------------------------------------------------------------------------------------------- //