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

	void SetMainMusicTrack(const char* newFilePath);
	void SetSubAreaMusicTrack(const char* newFilePath);

	void PlayMainMusic();
	void PlaySubAreaMusic();

private:
	void RemoveMusicTracks();

	void PlaySFXTrack(const char* newFilePath);

	void PauseMusic();
	void ResumeMusic();

	void RemoveAllSFX();
	void PauseAllSFX();
	void ResumeAllSFX();

	void SetAudioVolume(int volume);

	Mix_Music*              mMainMusic;
	Mix_Music*              mSubAreaMusic;

	// Vector to hold the SFX
	std::vector<Mix_Chunk*> mSFX;
};

#endif