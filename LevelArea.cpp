#include "LevelArea.h"

#include "BackgroundLayer.h"
#include "InteractionLayer.h"
#include "ObjectLayer.h"

#include "PlayableCharacter.h"
#include "AudioPlayer.h"

#include <iostream>

// --------------------------------------------------------------------------------------------------------------------------- //

LevelAreas::LevelAreas(std::string areaFilePath, bool& isStartingArea, SDL_Renderer* renderer, std::map<char, unsigned int> ConversionFromCharToIntIndexMap, Audio_Player* audioPlayer)
	: mLevelHeight(0)
	, mLevelWidth(0)
{
	// There is a strange thing happening when the areafilePath is passed in where it adds '\\' instead of '/'
	areaFilePath       = ReplaceDoubleBackslashWithFrontSlash(areaFilePath);

	// Will be implemented using three threads
	mBackgroundLayer   = new BackgroundLayer(areaFilePath   + "/Background Layer.txt",     areaFilePath + "/BackgroundSprites.png",   ConversionFromCharToIntIndexMap, renderer, Vector2D());
	mInteractableLayer = new InteractableLayer(areaFilePath + "/Interactable Layer.txt",   areaFilePath + "/InteractableSprites.png", ConversionFromCharToIntIndexMap, renderer, Vector2D());
	mObjectLayer       = new ObjectLayer(areaFilePath       + "/Object Layer.txt",         renderer, mInteractableLayer);
	
	// Create the ending section
	if (mBackgroundLayer->GetLevelEndingType() >= 0)
	{
		std::string filePath = "SDL_Mario_Project/Levels/LevelEndingSections/Section" + std::to_string(mBackgroundLayer->GetLevelEndingType());
		mEndingSection       = new BackgroundLayer(filePath + "/Background Layer.txt", filePath + "/BackgroundSprites.png", ConversionFromCharToIntIndexMap, renderer, Vector2D(mBackgroundLayer->GetLevelWidth() - 1, 0));
	}
	else
	{
		mEndingSection = nullptr;
	}

	// Need to account for the ending section not existing
	if (mEndingSection)
		mLevelWidth = mBackgroundLayer->GetLevelWidth() + mEndingSection->GetLevelWidth() - 1; // -1 to account for the overlap
	else
		mLevelWidth = mBackgroundLayer->GetLevelWidth();

	if (mBackgroundLayer)
		mLevelHeight = mBackgroundLayer->GetLevelHeight();
	else if (mEndingSection)
		mLevelHeight = mEndingSection->GetLevelHeight();

	// Now we need to calculate the name of this area
	mNameOfArea = CalculateNameOfArea(areaFilePath);

	// Now calculate what the sub area music
	if (mNameOfArea == "Overworld")
	{
		isStartingArea = true;

		// Leave the audio as it is if this is the overworld
	}
	else if (mNameOfArea == "Pipe")
	{
		audioPlayer->SetSubAreaMusicTrack("SDL_Mario_Project/Audio/Music/Levels/13 - Super Mario Rap.mp3");
	}
	else if (mNameOfArea == "Underwater")
	{
		audioPlayer->SetSubAreaMusicTrack("SDL_Mario_Project/Audio/Music/Levels/09 - Underwater.mp3");
	}
	else if (mNameOfArea == "Coin_Heaven")
	{
		audioPlayer->SetSubAreaMusicTrack("SDL_Mario_Project/Audio/Music/Levels/17 - Coin Heaven.mp3");
	}
	else if (mNameOfArea == "Pick_A_Box")
	{
		audioPlayer->SetSubAreaMusicTrack("SDL_Mario_Project/Audio/Music/Levels/19 - Pick A Box.mp3");
	}
	else if (mNameOfArea == "Boom_Boom")
	{
		audioPlayer->SetSubAreaMusicTrack("SDL_Mario_Project/Audio/Music/Levels/24 - Boom Boom.mp3");
	}
	else if (mNameOfArea == "King_Koopa_Battle")
	{
		audioPlayer->SetSubAreaMusicTrack("SDL_Mario_Project/Audio/Music/Levels/32 - King Koopa Battle.mp3");
	}
}

// --------------------------------------------------------------------------------------------------------------------------- //

LevelAreas::~LevelAreas()
{
	if (mBackgroundLayer)
		delete mBackgroundLayer;

	if (mInteractableLayer)
		delete mInteractableLayer;

	if (mObjectLayer)
		delete mObjectLayer;

	if (mEndingSection)
	{
		delete mEndingSection;
		mEndingSection = nullptr;
	}

	mBackgroundLayer   = nullptr;
	mInteractableLayer = nullptr;
	mObjectLayer       = nullptr;
}

// --------------------------------------------------------------------------------------------------------------------------- //

std::string LevelAreas::CalculateNameOfArea(std::string areaFilePath)
{
	// Loop through from the end of the string up until we hit a forward slash
	// That is the name of this area
	if (areaFilePath.length() != 0)
	{
		for (unsigned int i = areaFilePath.length() - 1; i > 0; i--)
		{
			if (areaFilePath[i] == '/')
			{
				return (areaFilePath.substr(i + 1, areaFilePath.length() - i));
			}
		}
	}

	return "";
}

// --------------------------------------------------------------------------------------------------------------------------- //

void LevelAreas::Render(Vector2D gridReferencePoint)
{
	// Render in this order: Background, Ending, interaction, object
	if (mBackgroundLayer)
		mBackgroundLayer->Render(gridReferencePoint);

	if (mEndingSection)
		mEndingSection->Render(gridReferencePoint);

	if (mInteractableLayer)
		mInteractableLayer->Render(gridReferencePoint);

	if (mObjectLayer)
		mObjectLayer->Render(gridReferencePoint);
}

// --------------------------------------------------------------------------------------------------------------------------- //

Area_Transition_Data LevelAreas::Update(const float deltaTime, SDL_Event e, PlayableCharacter* player)
{
	// First update the interaction layer and then the object layer
	if (mInteractableLayer)
		mInteractableLayer->Update(deltaTime);

	if (mObjectLayer && player)
		mObjectLayer->Update(deltaTime, e, player->GetRealGridPosition());

	switch (e.type)
	{
		case SDL_KEYUP:
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_m:
				return Area_Transition_Data{ 1, 0 };
			break;

			case SDLK_n:
				return Area_Transition_Data{ 0, 0 };
			}
		}
	}

	return Area_Transition_Data{ -1, 0 };
}

// --------------------------------------------------------------------------------------------------------------------------- //

std::string LevelAreas::ReplaceDoubleBackslashWithFrontSlash(std::string filePath)
{
	std::string returnString = "";

	for (unsigned int i = 0; i < filePath.size(); i++)
	{
		if (filePath[i] == '\\')
		{
			return filePath.substr(0, i) + '/' + filePath.substr(i + 1, filePath.length() - i);
		}
	}

	return returnString;
}

// --------------------------------------------------------------------------------------------------------------------------- //

Vector2D LevelAreas::GetInitialSpawnPoint() const
{
	if (mObjectLayer)
	{
		return mObjectLayer->GetInitialSpawnPoint();
	}
	
	return Vector2D();
}

// --------------------------------------------------------------------------------------------------------------------------- //

const Vector2D LevelAreas::GetSpawnPointPosition(unsigned int ID)
{
	if (mObjectLayer)
	{
		return mObjectLayer->GetSpawnPoint(ID);
	}

	return Vector2D();
}

// --------------------------------------------------------------------------------------------------------------------------- //

void LevelAreas::PlayMusicForArea(Audio_Player* audioPlayer)
{
	if (mNameOfArea == "Overworld")
		audioPlayer->PlayMainMusic();
	else
		audioPlayer->PlaySubAreaMusic();
}

// --------------------------------------------------------------------------------------------------------------------------- //