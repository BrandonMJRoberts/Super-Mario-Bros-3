#include "AudioPlayer.h"

#include <iostream>
#include <sstream>
#include <string>

 // ----------------------------------------------------- //

Audio_Player::Audio_Player()
	: Observer()
	, mWorldMapMusic(nullptr)
	, mMainLevelMusic(nullptr)
	, mSubAreaMusic(nullptr)
{
	openChannels = { 1,2,3,4,5,6,7,8 };

	// Default the volume to not be deafening
	SetAudioVolume(0);

	Mix_AllocateChannels(16);

	// Set the callback for when a channel finishes
	AudioCallback::SetAudioPlayer(this);
	Mix_ChannelFinished(AudioCallback::ChannelFinishedCallback);
}


// ----------------------------------------------------- //

Audio_Player::~Audio_Player()
{
	RemoveAllMusicTracks();

	RemoveAllSFX();
}

// ----------------------------------------------------- //

void Audio_Player::OnNotify(SUBJECT_NOTIFICATION_TYPES notification, std::string data)
{
	std::stringstream streamLine(data);

	switch (notification)
	{
	case SUBJECT_NOTIFICATION_TYPES::SETUP_WORLD_MAP:
		// First calculate what world we are in
		int world;
		streamLine >> world;

		switch (world)
		{
		default:
		case 1:
			SetWorldMapMusicTrack("SDL_Mario_Project/Audio/Music/World/01 - Grass Land.mp3");
		break;

		case 2:
			SetWorldMapMusicTrack("SDL_Mario_Project/Audio/Music/World/05 - Desert Hill.mp3");
		break;

		case 3:
			SetWorldMapMusicTrack("SDL_Mario_Project/Audio/Music/World/08 - Ocean Side.mp3");
		break;

		case 4:
			SetWorldMapMusicTrack("SDL_Mario_Project/Audio/Music/World/12 - Big Island.mp3");
		break;

		case 5:
			SetWorldMapMusicTrack("SDL_Mario_Project/Audio/Music/World/16 - The Sky.mp3");
		break;

		case 6:
			SetWorldMapMusicTrack("SDL_Mario_Project/Audio/Music/World/21 - Iced Land.mp3");
		break;

		case 7:
			SetWorldMapMusicTrack("SDL_Mario_Project/Audio/Music/World/26 - Pipe Maze.mp3");
		break;

		case 8:
			SetWorldMapMusicTrack("SDL_Mario_Project/Audio/Music/World/30 - Castle of Koopa.mp3");
		break;
		}

	break;

	case SUBJECT_NOTIFICATION_TYPES::PLAY_WORLD_MAP_MUSIC:
		PlayWorldMapMusic();
	break;

	case SUBJECT_NOTIFICATION_TYPES::PLAY_MAIN_LEVEL_MUSIC:
		PlayMainLevelMusic();
	break;

	case SUBJECT_NOTIFICATION_TYPES::PLAY_SUB_AREA_MUSIC:
		PlaySubAreaMusic();
	break;

	case SUBJECT_NOTIFICATION_TYPES::SETUP_MAIN_LEVEL:
		if(data == "Overworld")
			SetMainLevelMusicTrack("SDL_Mario_Project/Audio/Music/Levels/02 - Level Theme 1.mp3");
		else if(data == "Air_Ship")
			SetMainLevelMusicTrack("SDL_Mario_Project/Audio/Music/Levels/27 - Airship.mp3");
		else if(data == "Hammer_Bros")
			SetMainLevelMusicTrack("SDL_Mario_Project/Audio/Music/Levels/14 - Hammer Brothers.mp3");
		else if(data == "Mini_Fortress")
			SetMainLevelMusicTrack("SDL_Mario_Project/Audio/Music/Levels/22 - Mini-Fortress.mp3");
		else if(data == "Overworld2")
			SetMainLevelMusicTrack("SDL_Mario_Project/Audio/Music/Levels/06 - Level Theme 2.mp3");
		else if(data == "Underwater")
			SetMainLevelMusicTrack("SDL_Mario_Project/Audio/Music/Levels/09 - Underwater.mp3");
		else if(data == "Spade_Puzzle")
			SetMainLevelMusicTrack("SDL_Mario_Project/Audio/Music/Levels/11 - Spade Puzzle.mp3");
	break;

	case SUBJECT_NOTIFICATION_TYPES::SETUP_SUB_LEVEL:
		if (data == "Pipe")
			SetSubAreaMusicTrack("SDL_Mario_Project/Audio/Music/Levels/13 - Super Mario Rap.mp3");
		else if (data == "Underwater")
			SetSubAreaMusicTrack("SDL_Mario_Project/Audio/Music/Levels/09 - Underwater.mp3");
		else if (data == "Coin_Heaven")
			SetSubAreaMusicTrack("SDL_Mario_Project/Audio/Music/Levels/17 - Coin Heaven.mp3");
		else if (data == "Pick_A_Box")
			SetSubAreaMusicTrack("SDL_Mario_Project/Audio/Music/Levels/19 - Pick A Box.mp3");
		else if (data == "Boom_Boom")
			SetSubAreaMusicTrack("SDL_Mario_Project/Audio/Music/Levels/24 - Boom Boom.mp3");
		else if (data == "King_Koopa_Battle")
			SetSubAreaMusicTrack("SDL_Mario_Project/Audio/Music/Levels/32 - King Koopa Battle.mp3");
	break;

	case SUBJECT_NOTIFICATION_TYPES::PLAYER_MOVED_ON_WORLD_MAP:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Map Travel.wav");
	break;

	case SUBJECT_NOTIFICATION_TYPES::ENTERING_LEVEL:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Enter Level.wav");
	break;

	case SUBJECT_NOTIFICATION_TYPES::ADD_LIFE:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/18 - 1-Up.mp3");
	break;

	case SUBJECT_NOTIFICATION_TYPES::AIRSHIP_MOVES_ON_MAP:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Airship Moves.wav");
	break;

	case SUBJECT_NOTIFICATION_TYPES::BONUS_NO_MATCH:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Bonus Game No Match.wav");
	break;

	case SUBJECT_NOTIFICATION_TYPES::BONUS_MATCH:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Bonus Game No Match.wav");
		break;

	case SUBJECT_NOTIFICATION_TYPES::SPADE_MATCH:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Nspade Match.wav");
	break;

	case SUBJECT_NOTIFICATION_TYPES::BOSS_DEFEATED:
		if (data == "Bowser")
		{
			PlaySFXTrack("SDL_Mario_Project/Audio/SFX/33 - King Koopa Falls.mp3");
		}
		else
		{
			PlaySFXTrack("SDL_Mario_Project/Audio/SFX/25 - Boss Clear.mp3");
		}
	break;

	case SUBJECT_NOTIFICATION_TYPES::BROKEN_BLOCK:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Break Brick Block.wav");
	break;

	case SUBJECT_NOTIFICATION_TYPES::BUMPED_BLOCK:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Bump.wav");
	break;

	// Both use the same sound
	case SUBJECT_NOTIFICATION_TYPES::CANNON_FIRES_CANNON_BALL:
	case SUBJECT_NOTIFICATION_TYPES::THWOMP_HIT_GROUND:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Thwomp-CannonFire.wav");
	break;

	case SUBJECT_NOTIFICATION_TYPES::COIN_COLLECTED:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Coin.wav");
	break;

	case SUBJECT_NOTIFICATION_TYPES::COMPLETED_WORLD:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/29 - World Clear.mp3");
	break;

	case SUBJECT_NOTIFICATION_TYPES::ENTERING_PIPE:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/23 - Warp Pipe.mp3");
	break;

	case SUBJECT_NOTIFICATION_TYPES::FIRE_FIREBALL:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Fireball.wav");
	break;

	case SUBJECT_NOTIFICATION_TYPES::FROG_MARIO_WALKS:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Frog Mario Walk.wav");
	break;

	case SUBJECT_NOTIFICATION_TYPES::GAME_OVER:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/36 - Game Over.mp3");
	break;

	case SUBJECT_NOTIFICATION_TYPES::HAMMER_BROS_MOVE_AROUND_MAP:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Hammer Bros Shuffle.wav");
	break;

	case SUBJECT_NOTIFICATION_TYPES::INTO_NEW_WORLD_MAP:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Map New World.wav");
	break;

	case SUBJECT_NOTIFICATION_TYPES::JUMPED_OFF_ENEMY:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Stomp.wav");
	break;

	case SUBJECT_NOTIFICATION_TYPES::LEVEL_CLEAR:
		PauseMusic();
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/04 - Level Clear.wav");
	break;
	 
	case SUBJECT_NOTIFICATION_TYPES::LOW_TIMER:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Hurry_up.wav");
	break;

	case SUBJECT_NOTIFICATION_TYPES::OPEN_INVENTORY:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Inventory Open Close.wav");
	break;

	case SUBJECT_NOTIFICATION_TYPES::PLAYER_JUMPED:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Jump.wav");
	break;

	case SUBJECT_NOTIFICATION_TYPES::PLAYER_KICKED:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Kick.wav");
	break;

	case SUBJECT_NOTIFICATION_TYPES::PLAYER_SWIPE_OF_TAIL:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Tail.wav");
	break;

	case SUBJECT_NOTIFICATION_TYPES::PLAYER_TRANSFORM_INTO_RACOON:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Raccoon Transform.wav");
	break;

	case SUBJECT_NOTIFICATION_TYPES::SET_UNPAUSED:
	case SUBJECT_NOTIFICATION_TYPES::SET_PAUSED:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/31 - Pause.mp3");
	break;

	case SUBJECT_NOTIFICATION_TYPES::SPAWN_MUSHROOM:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Mushroom Appears.wav");
	break;

	case SUBJECT_NOTIFICATION_TYPES::TEXT_DISPLAYING:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Text.wav");
	break;

	case SUBJECT_NOTIFICATION_TYPES::VINE_GROWS:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Vine.wav");
	break;

	case SUBJECT_NOTIFICATION_TYPES::USE_WARP_WISTLE:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Whistle.wav");
	break;

	case SUBJECT_NOTIFICATION_TYPES::GAME_SELECT_COIN_SFX:
		PlaySFXTrack("SDL_Mario_Project/Audio/SFX/Coin Select.wav");
	break;

	case SUBJECT_NOTIFICATION_TYPES::PLAY_GAME_SELECT_MUSIC:
		SetMainLevelMusicTrack("SDL_Mario_Project/Audio/Music/Game Select.mp3");
		PlayMainLevelMusic();
	break;
	}
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

void Audio_Player::SetWorldMapMusicTrack(const char* newFilePath)
{
	// Remove the track if it already exists
	if (mWorldMapMusic)
		Mix_FreeMusic(mWorldMapMusic);

	// Load in the new track set
	mWorldMapMusic = Mix_LoadMUS(newFilePath);

	// Check it worked
	if (mWorldMapMusic == nullptr)
	{
		std::cout << "Failed to load in the music requested!" << std::endl;
		mWorldMapMusic = nullptr;
		return;
	}
}

// ----------------------------------------------------- //

void Audio_Player::SetMainLevelMusicTrack(const char* newFilePath)
{
	// Remove the track if it already exists
	if(mMainLevelMusic)
		Mix_FreeMusic(mMainLevelMusic);

	// Load in the new track set
	mMainLevelMusic = Mix_LoadMUS(newFilePath);

	// Check it worked
	if (mMainLevelMusic == nullptr)
	{
		std::cout << "Failed to load in the music requested!" << std::endl;
		mMainLevelMusic = nullptr;
		return;
	}
}

// ----------------------------------------------------- //

void Audio_Player::PlaySFXTrack(const char* newFilePath)
{
	// If there is a free channel to play a sound on then play it there
	if (openChannels.size() > 0)
	{
		// Load in the track
		mSFX.push_back(Mix_LoadWAV(newFilePath));

		// play the audio
		Mix_PlayChannel(openChannels[openChannels.size() - 1], mSFX[mSFX.size() - 1], 0);

		openChannels.pop_back();
	}
}

// ----------------------------------------------------- //

void Audio_Player::RemoveAllMusicTracks()
{
	Mix_HaltMusic();

	if (mMainLevelMusic)
		Mix_FreeMusic(mMainLevelMusic);

	if(mWorldMapMusic)
		Mix_FreeMusic(mWorldMapMusic);

	if (mSubAreaMusic)
		Mix_FreeMusic(mSubAreaMusic);

	mWorldMapMusic  = nullptr;
	mMainLevelMusic = nullptr;
	mSubAreaMusic   = nullptr;
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
	Mix_Pause(3);
	Mix_Pause(4);
	Mix_Pause(5);
	Mix_Pause(6);
	Mix_Pause(7);
	Mix_Pause(8);
}

// ----------------------------------------------------- //

void Audio_Player::ResumeAllSFX()
{
	Mix_Resume(1);
	Mix_Resume(2);
	Mix_Resume(3);
	Mix_Resume(4);
	Mix_Resume(5);
	Mix_Resume(6);
	Mix_Resume(7);
	Mix_Resume(8);
}

// ----------------------------------------------------- //

void Audio_Player::PlayWorldMapMusic()
{
	// Only do anything if we have a track to start playing
	if (mWorldMapMusic)
	{
		// Stop the current music
		Mix_HaltMusic();

		// Play the music track
		Mix_PlayMusic(mWorldMapMusic, -1);
	}
}

// ----------------------------------------------------- //

void Audio_Player::PlayMainLevelMusic()
{
	// Only do anything if we have a track to start playing
	if (mMainLevelMusic)
	{
		// Stop the current music
		Mix_HaltMusic();

		// Play the music track
		Mix_PlayMusic(mMainLevelMusic, -1);
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
	}
}

// ----------------------------------------------------- //

void Audio_Player::SetAudioVolume(int volume)
{
	for (unsigned int i = 0; i < openChannels.size(); i++)
	{
		Mix_Volume(openChannels[i], volume);
	}

	Mix_VolumeMusic(volume);
}

// ----------------------------------------------------- //

void Audio_Player::Update()
{
	// Loop through all SFX and check to see if they are in use
	Mix_Chunk* sfx;
	unsigned int index = 0;

	std::vector<unsigned int> sfxIndexesBeingPlayed;

	for (unsigned int i = 0; i < openChannels.size(); i++)
	{
		// Get the SFX being played on this chunk
		sfx = Mix_GetChunk(openChannels[i]);

		for (unsigned int j = 0; j < mSFX.size(); j++)
		{
			if (mSFX[j] == sfx)
			{
				sfxIndexesBeingPlayed.push_back(j);
				break;
			}
		}
	}

	// Now we have the indexes that are being played, remove all others offset
	unsigned int offset = 0;

	if (sfxIndexesBeingPlayed.size() > 0)
	{
		for (unsigned int indexBeingPlayed = sfxIndexesBeingPlayed.size(); indexBeingPlayed > 0; indexBeingPlayed--)
		{
			for (unsigned int sfxVectorIndex = 0; sfxVectorIndex < mSFX.size(); sfxVectorIndex++)
			{
				if (sfxVectorIndex == sfxIndexesBeingPlayed[indexBeingPlayed - 1])
				{
					mSFX.erase(mSFX.begin() + sfxIndexesBeingPlayed[indexBeingPlayed - 1] - offset);
					offset++;
				}
				else
				{
					continue;
				}
			}
		}
	}
}

// ----------------------------------------------------- //

void Audio_Player::SetChannelFinished(int channel)
{
	// Get the SFX that was playing on the channel
	Mix_Chunk* sfx = Mix_GetChunk(channel);
	Mix_FreeChunk(sfx);

	openChannels.push_back(channel);

	// Loop through and see if this channel is in our vector
	////for (unsigned int i = 0; i < mFilledChannels.size(); i++)
	//{
	//	if (mFilledChannels[i] == channel)
	//	{
	//		mFilledChannels.erase(mFilledChannels.begin() + i);
	//		return;
	//	}
	//}
}

// ----------------------------------------------------- //

namespace AudioCallback
{
	namespace
	{
		Audio_Player* nAudioplayer;
	}

	void SetAudioPlayer(Audio_Player* audioPlayer)
	{
		nAudioplayer = audioPlayer;
	}

	void ChannelFinishedCallback(int channelID)
	{
		if (nAudioplayer)
			nAudioplayer->SetChannelFinished(channelID);
	}
}

// ----------------------------------------------------- //