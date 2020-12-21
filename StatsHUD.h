#ifndef _STATS_HUB_H_
#define _STATS_HUB_H_

#include "Texture2D.h"
#include "Commons_SMB3.h"

class Stats_HUD final
{
public:
	Stats_HUD();
	~Stats_HUD();

private:
	Texture2D* mBackgroundSprite;

	Texture2D* mPMeterArrowSprite;
	Texture2D* mCompletePMeterSprite;

	Texture2D* mCurrentCounterWorldSprite;


	Vector2D mLivesCounterOffset;
	Vector2D mScoreCounterOffset;
	Vector2D mMoneyCounterOffset;
	Vector2D mCurrentWorldOffset;
	Vector2D mPMeterOffset;
	Vector2D mTimerCounterOffset;
};

#endif // !_STATS_HUB_H_
