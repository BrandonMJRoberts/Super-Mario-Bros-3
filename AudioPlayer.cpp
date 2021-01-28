#include "AudioPlayer.h"

#include <iostream>

 // ----------------------------------------------------- //

Audio_Player::Audio_Player() 
	: mMusic(nullptr)
{

}

// ----------------------------------------------------- //

Audio_Player::Audio_Player(const char* filePathForStartingMusic)
	: mMusic(nullptr)
{
	SetMusicTrack(filePathForStartingMusic);
}

// ----------------------------------------------------- //

Audio_Player::~Audio_Player()
{
	RemoveMusicTrack();

	RemoveAllSFX();
}

// ----------------------------------------------------- //

void Audio_Player::OnNotify(SUBJECT_NOTIFICATION_TYPES notification, std::string data)
{

}

// ----------------------------------------------------- //

void Audio_Player::SetMusicTrack(const char* newFilePath)
{
	if(mMusic)
		Mix_FreeMusic(mMusic);

	mMusic = Mix_LoadMUS(newFilePath);

	if (mMusic == nullptr)
	{
		std::cout << "Failed to load in the music requested!" << std::endl;
		mMusic = nullptr;
		return;
	}

	// Set the music to be playing on a loop
	Mix_PlayMusic(mMusic, -1);

	Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
}

// ----------------------------------------------------- //

void Audio_Player::PlaySFXTrack(const char* newFilePath)
{
	mSFX.push_back(Mix_LoadWAV(newFilePath));

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

void Audio_Player::RemoveMusicTrack()
{
	Mix_HaltMusic();

	if(mMusic)
		Mix_FreeMusic(mMusic);

	mMusic = nullptr;
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