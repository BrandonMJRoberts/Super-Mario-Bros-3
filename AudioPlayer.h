#ifndef _AUDIO_PLAYER_H_
#define _AUDIO_PLAYER_H_

#include "Observer.h"

#include <SDL.h>
#include <SDL_mixer.h>

#include <vector>


class Audio_Player : public Observer
{
public:
	Audio_Player();
	Audio_Player(const char* filePathForStartingMusic);
	~Audio_Player();

	void OnNotify(SUBJECT_NOTIFICATION_TYPES notification, std::string data) override;

	void SetMusicTrack(const char* newFilePath);

private:
	void RemoveMusicTrack();

	void PlaySFXTrack(const char* newFilePath);

	void PauseMusic();
	void ResumeMusic();

	void RemoveAllSFX();
	void PauseAllSFX();
	void ResumeAllSFX();

	Mix_Music*              mMusic;

	// Vector to hold the SFX
	std::vector<Mix_Chunk*> mSFX;
};

#endif