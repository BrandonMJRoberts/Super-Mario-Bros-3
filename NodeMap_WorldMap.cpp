#include "NodeMap_WorldMap.h"

#include <fstream>
#include <sstream>

#include "Texture2D.h"
#include "Constants_SMB3.h"

// ----------------------------------------------------------------- //

NodeMap_WorldMap::NodeMap_WorldMap(std::string filePath)
{
	mWidth  = 0;
	mHeight = 0;

	LoadInDataFromFile(filePath);
}

// ----------------------------------------------------------------- //

NodeMap_WorldMap::~NodeMap_WorldMap()
{
	for (unsigned int i = 0; i < mHeight; i++)
	{
		delete[] mNodeMapData[i];
	}
	delete[] mNodeMapData;
	mNodeMapData = nullptr;

	mLevelLoadingFilePaths.clear();
}


// ----------------------------------------------------------------- //

void NodeMap_WorldMap::LoadInDataFromFile(std::string filePath)
{
	// First open the data files
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		std::cout << "Failed to open the node map file for the world map." << std::endl;
		return;
	}

	char*             cLine = new char[100];
	std::string       sLine;
	std::stringstream ssLine;

	unsigned int      failsafeCounter = 0;
	unsigned int      amountOfLevels = 0;

	unsigned int      currentRow = 0;

	while (file.getline(cLine, 100))
	{
		if (failsafeCounter++ >= FAILSAFE_MAX_COUNT_FILE_LOADING_LOOPS)
		{
			std::cout << "Error with the file being loaded in: node map for the world map." << std::endl;
			return;
		}

		sLine = std::string(cLine);

		if (sLine == "" || sLine == "END" || sLine[0] == '#')
			continue;

		ssLine = std::stringstream(sLine);

		// First we need to load in the amount of enterable levels in this map
		if (amountOfLevels == 0)
		{
			ssLine >> amountOfLevels;
			continue;
		}

		// If we havent gotten in all of the level file paths yet:
		if (mLevelLoadingFilePaths.size() < amountOfLevels)
		{
			char index;
		    std::string filePathForLevel;
			ssLine >> index >> filePathForLevel;
			mLevelLoadingFilePaths[index] = filePathForLevel;
			continue;
		}

		// Now we get the width and height
		if (mWidth == 0)
		{
			ssLine >> mWidth;
			continue;
		}

		if (mHeight == 0)
		{
			ssLine >> mHeight;
			continue;
		}

		// Now we need to load in the data after allocating the required memory 
		if (!mNodeMapData)
		{
			// Setup the rows
			mNodeMapData = new char* [mHeight];

			// Populate the rows with columns
			for (unsigned int i = 0; i < mHeight; i++)
			{
				mNodeMapData[i] = new char[mWidth - 1];
			}
		}

		for (unsigned int i = 0; i < sLine.size(); i++)
		{
			mNodeMapData[currentRow][i] = sLine[i];
		}

		currentRow++;
	}
}

// ----------------------------------------------------------------- //

void NodeMap_WorldMap::ReplaceDataPoint(unsigned int row, unsigned int col, char newVal)
{
	if (mNodeMapData)
	{
		if (col >= mWidth || row >= mHeight || col < 0 || row < 0)
		{
			return;
		}

		mNodeMapData[row][col] = newVal;
	}
}

// ----------------------------------------------------------------- //

char NodeMap_WorldMap::GetSpecificDataPoint(unsigned int row, unsigned int col)
{
	if (!mNodeMapData || col >= mWidth || row >= mHeight || col < 0 || row < 0)
		return '-';

	return mNodeMapData[row][col];
}

// ----------------------------------------------------------------- //

Vector2D NodeMap_WorldMap::GetSpawnPoint()
{
	for (unsigned int row = 0; row < mHeight; row++)
	{
		for (unsigned int col = 0; col < mWidth; col++)
		{
			if (mNodeMapData[row][col] == 'S')
				return Vector2D(col, row);
		}
	}

	return Vector2D();
}

// ----------------------------------------------------------------- //