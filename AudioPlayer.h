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
	~Audio_Player();

	void OnNotify(SUBJECT_NOTIFICATION_TYPES notification, std::string data) override;

private:
	void SetWorldMapMusicTrack(const char* newFilePath);
	void SetMainLevelMusicTrack(const char* newFilePath);
	void SetSubAreaMusicTrack(const char* newFilePath);

	void PlayWorldMapMusic();
	void PlayMainLevelMusic();
	void PlaySubAreaMusic();

	void RemoveAllMusicTracks();

	void PlaySFXTrack(const char* newFilePath);

	void PauseMusic();
	void ResumeMusic();

	void RemoveAllSFX();
	void PauseAllSFX();
	void ResumeAllSFX();

	void SetAudioVolume(int volume);

	// Three types of music required for the game, as there will only be one audio player in the game
	Mix_Music*              mWorldMapMusic;
	Mix_Music*              mMainLevelMusic;
	Mix_Music*              mSubAreaMusic;

	// Vector to hold the SFX
	std::vector<Mix_Chunk*> mSFX;
};

#endif