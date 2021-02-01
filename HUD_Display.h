#ifndef _HUD_DISPLAY_H_
#define _HUD_DISPLAY_H_

#include "Commons_SMB3.h"

#include "Observer.h"

#include <sdl.h>

class Texture2D;
class TextRenderer;

// The HUD is an observer of multiple areas in order to update its data
class HUD_Display final : public Observer
{
public:
	HUD_Display(SDL_Renderer* renderer);
	~HUD_Display() override;

	void Render();
	void Update(const float deltaTime);

	void Reset();

	void OnNotify(SUBJECT_NOTIFICATION_TYPES notification, std::string data) override;

private:
	void LoadInSprites(SDL_Renderer* renderer);
	void DeleteAllSprites();

	unsigned int mCurrentScore;
	unsigned int mLivesRemaining;
	unsigned int mCurrentMoneyCount;
	unsigned int mCurrentWorldID;
	unsigned int mCurrentEndCardCount;
	unsigned int mPMeterFillAmount;

	unsigned int mEndCardsSingleSpriteWidth;
	unsigned int mEndCardsSingleSpriteHeight;

	float        mTimeRemaming;
	bool         mPlayingAsMario;
	bool         mPaused;

	// Array to hold the three current end card types
	END_CARD_TYPES mEndCards[3];

	// The overall background sprite
	Texture2D* mBackgroundSprite;

	// The black/while arrow that builds up over time
	Texture2D* mPMeterArrowSprite;

	// The final bubble with a 'P' in it to signify you are going as fast as possible
	Texture2D* mCompletePMeterSprite;

	// World counter sprite sheet
	Texture2D* mCurrentWorldSprite;

	// Sprite sheet for the end cards
	Texture2D* mEndCardsSpriteSheet;

	Texture2D* mLifeIcon;


	SDL_Rect mDestRectPlaceHolder;
	SDL_Rect mSourceRectPlaceHolder;

	// Positions for specific element rendering 
	const Vector2D mLivesCounterOffset;
	const Vector2D mLifeIconOffset;
	const Vector2D mScoreCounterOffset;
	const Vector2D mMoneyCounterOffset;

	const Vector2D mCurrentWorldOffset;

	const Vector2D mPMeterOffset;
	const Vector2D mCompletePMeterOffset;

	const Vector2D mTimerCounterOffset;

	const Vector2D mFirstEndCardOffset;
	const Vector2D mBackgroundSpriteOffset;

	TextRenderer* mFontRenderer;
};

#endif // !_HUD_DISPLAY_H_
