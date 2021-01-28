#include "AudioPlayer.h"

#include <iostream>

 // ----------------------------------------------------- //

Audio_Player::Audio_Player() 
	: mMainMusic(nullptr)
	, mSubAreaMusic(nullptr)
{
	SetAudioVolume(32);
}

// ----------------------------------------------------- //

Audio_Player::Audio_Player(const char* filePathForStartingMusic)
	: mMainMusic(nullptr)
	, mSubAreaMusic(nullptr)
{
	SetAudioVolume(32);

	SetMainMusicTrack(filePathForStartingMusic);
}

// ----------------------------------------------------- //

Audio_Player::~Audio_Player()
{
	RemoveMusicTracks();

	RemoveAllSFX();
}

// ----------------------------------------------------- //

void Audio_Player::OnNotify(SUBJECT_NOTIFICATION_TYPES notification, std::string data)
{

}

// ----------------------------------------------------- //

void Audio_Player::SetSubAreaMusicTrack(const char* newFilePath)
{
	// Remove the track if it already exists
	if (mSubAreaMusic)
		Mix_FreeMusic(mSubAreaMusic);

	// Load in the new track set
	mSubAreaMusic = Mix_LoadMUS(newFilePath);

	// Check it worked
	if (mSubAreaMusic == nullptr)
	{
		std::cout << "Failed to load in the music requested!" << std::endl;
		mSubAreaMusic = nullptr;
		return;
	}
}

// ----------------------------------------------------- //

void Audio_Player::SetMainMusicTrack(const char* newFilePath)
{
	// Remove the track if it already exists
	if(mMainMusic)
		Mix_FreeMusic(mMainMusic);

	// Load in the new track set
	mMainMusic = Mix_LoadMUS(newFilePath);

	// Check it worked
	if (mMainMusic == nullptr)
	{
		std::cout << "Failed to load in the music requested!" << std::endl;
		mMainMusic = nullptr;
		return;
	}
}

// ----------------------------------------------------- //

void Audio_Player::PlaySFXTrack(const char* newFilePath)
{
	// Load in the track
	mSFX.push_back(Mix_LoadWAV(newFilePath));

	// Check it exists
	if (mSFX[mSFX.size() - 1] == nullptr)
	{
		std::cout << "Failed to load an SFX track! " << std::endl;
		mSFX.pop_back();
		return;
	}

	// Set the SFX to be playing 
	Mix_PlayChannel(-1, mSFX[mSFX.size() - 1], 0);
}

// ----------------------------------------------------- //

void Audio_Player::RemoveMusicTracks()
{
	Mix_HaltMusic();

	if (mMainMusic)
		Mix_FreeMusic(mMainMusic);

	if (mSubAreaMusic)
		Mix_FreeMusic(mSubAreaMusic);

	mMainMusic    = nullptr;
	mSubAreaMusic = nullptr;
}

// ----------------------------------------------------- //

void Audio_Player::PauseMusic()
{
	Mix_PauseMusic();
}

// ----------------------------------------------------- //

void Audio_Player::ResumeMusic()
{
	Mix_ResumeMusic();
}

// ----------------------------------------------------- //

void Audio_Player::RemoveAllSFX()
{
	for (unsigned int i = 0; i < mSFX.size(); i++)
	{
		if (mSFX[i])
			Mix_FreeChunk(mSFX[i]);

		mSFX[i] = nullptr;
	}

	mSFX.clear();
}

// ----------------------------------------------------- //

void Audio_Player::PauseAllSFX()
{
	Mix_Pause(1);
	Mix_Pause(2);
}

// ----------------------------------------------------- //

void Audio_Player::ResumeAllSFX()
{
	Mix_Resume(1);
	Mix_Resume(2);
}

// ----------------------------------------------------- //

void Audio_Player::PlayMainMusic()
{
	// Only do anything if we have a track to start playing
	if (mMainMusic)
	{
		// Stop the current music
		Mix_HaltMusic();

		// Play the music track
		Mix_PlayMusic(mMainMusic, -1);

		Mix_Volume(1, MIX_MAX_VOLUME / 4);
	}
}

// ----------------------------------------------------- //

void Audio_Player::PlaySubAreaMusic() 
{
	// Only do anything if we have a track to start playing
	if (mSubAreaMusic)
	{
		// Stop the current music
		Mix_HaltMusic();

		// Play the music track
		Mix_PlayMusic(mSubAreaMusic, -1);

		Mix_Volume(1, MIX_MAX_VOLUME / 4);
	}
}

// ----------------------------------------------------- //

void Audio_Player::SetAudioVolume(int volume)
{
	Mix_Volume(1, MIX_MAX_VOLUME / 4);
	Mix_Volume(2, MIX_MAX_VOLUME / 4);
	Mix_Volume(3, MIX_MAX_VOLUME / 4);
	Mix_Volume(4, MIX_MAX_VOLUME / 4);

	Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
}

// ----------------------------------------------------- //