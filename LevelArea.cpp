#include "LevelArea.h"

#include "BackgroundLayer.h"
#include "InteractionLayer.h"
#include "ObjectLayer.h"

#include "PlayableCharacter.h"
#include "Observer.h"

#include <iostream>

// --------------------------------------------------------------------------------------------------------------------------- //

LevelAreas::LevelAreas(std::string areaFilePath, bool& isStartingArea, SDL_Renderer* renderer, std::map<char, unsigned int> ConversionFromCharToIntIndexMap, std::vector<Observer*> observers)
	: mLevelHeight(0)
	, mLevelWidth(0)
{
	// There is a strange thing happening when the areafilePath is passed in where it adds '\\' instead of '/'
	areaFilePath       = ReplaceDoubleBackslashWithFrontSlash(areaFilePath);

	// Will be implemented using three threads
	mBackgroundLayer   = new BackgroundLayer(areaFilePath   + "/Background Layer.txt",     areaFilePath + "/BackgroundSprites.png",   ConversionFromCharToIntIndexMap, renderer, Vector2D());
	mInteractableLayer = new InteractableLayer(areaFilePath + "/Interactable Layer.txt",   areaFilePath + "/InteractableSprites.png", ConversionFromCharToIntIndexMap, renderer, Vector2D());
	mObjectLayer       = new ObjectLayer(areaFilePath       + "/Object Layer.txt",         renderer, mInteractableLayer, observers);
	
	// Create the ending section
	if (mBackgroundLayer->GetLevelEndingType() >= 0)
	{
		std::string filePath = "SDL_Mario_Project/Levels/LevelEndingSections/Section" + std::to_string(mBackgroundLayer->GetLevelEndingType());
		mEndingSection       = new BackgroundLayer(filePath + "/Background Layer.txt", filePath + "/BackgroundSprites.png", ConversionFromCharToIntIndexMap, renderer, Vector2D(mBackgroundLayer->GetLevelWidth() - 2, 0));
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

	if (mNameOfArea == "Overworld")
		isStartingArea = true;
	else
		isStartingArea = false;
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

void LevelAreas::RenderBackground(Vector2D gridReferencePoint)
{
	// Render in this order: Background, Ending, interaction, object
	if (mBackgroundLayer)
		mBackgroundLayer->Render(gridReferencePoint);

	if (mEndingSection)
		mEndingSection->Render(gridReferencePoint);
}

// --------------------------------------------------------------------------------------------------------------------------- //

void LevelAreas::RenderGround(Vector2D gridReferencePoint)
{
	if (mInteractableLayer)
		mInteractableLayer->Render(gridReferencePoint);
}

// --------------------------------------------------------------------------------------------------------------------------- //

void LevelAreas::RenderObjects(Vector2D gridReferencePoint)
{
	if (mObjectLayer)
		mObjectLayer->Render(gridReferencePoint);
}

// --------------------------------------------------------------------------------------------------------------------------- //

void LevelAreas::RenderAll(Vector2D gridReferencePoint)
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
	{
		if (mObjectLayer->GetLevelEndCollected())
		{
			player->SetLevelOver();
		}

		if (player->GetIsAlive())
		{
			if (mObjectLayer->Update(deltaTime, e, player->GetRealGridPosition()))
			{
				return Area_Transition_Data{ 0, 0, true, true };
			}
		}
	}

	switch (e.type)
	{
		case SDL_KEYUP:
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_m:
				return Area_Transition_Data{ 1, 0, false, false  };
			break;

			case SDLK_n:
				return Area_Transition_Data{ 0, 0, false, false };
			}
		}
	}

	return Area_Transition_Data{ -1, 0, false, false };
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