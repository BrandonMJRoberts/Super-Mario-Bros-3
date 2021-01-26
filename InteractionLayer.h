#ifndef _INTERACTION_LAYER_H_
#define _INTERACTION_LAYER_H_

#include <vector>
#include <string>
#include <map>

#include "Commons_SMB3.h"

class Texture2D;
struct SDL_Renderer;

class InteractableLayer final
{
public:
	InteractableLayer() = delete;
	InteractableLayer(std::string filePathToDataFile, std::string filePathToSpriteSheet, std::map<char, unsigned int> lookupConversion, SDL_Renderer* renderer, Vector2D offsetFromTopLeft);
	~InteractableLayer();

	void Render(const Vector2D gridReferencePoint);                      // For rendering the colliable objects in the level
	void Update(const float deltaTime); // Update for updating animations, such as breaking a brick block

	bool GetIsCollision(unsigned int row, unsigned int col) { return (mInteractionLayerDataStore[row][col] != 150); }


private:
	bool LoadInDataFromFile(std::string filePath, std::map<char, unsigned int> lookupConversion);
	bool CheckAllDataLoaded();

	std::vector<std::string> mListOfObjectUsedInInteractionLayer; // Built using the first part of the data file - only used for the construction of the array of data

	unsigned int** mInteractionLayerDataStore; // Stores the ID for the objects used in the level area - directly correlates to the map store above

	Texture2D*   mSpriteSheet;

	unsigned int mLevelWidth;
	unsigned int mLevelHeight;
	unsigned int mAmountOfSpritesInDataFile;
	unsigned int mAmountOfSpritesOnSpriteSheetWidth;
	unsigned int mAmountOfSpritesOnSpriteSheetHeight;

	Vector2D     mOffsetFromTopLeft;
};

#endif // !_INTERACTION_LAYER_H_