#ifndef _NODEMAP_WORLD_MAP_H_
#define _NODEMAP_WORLD_MAP_H_

#include "Commons_SMB3.h"

#include <vector>
#include <string>
#include <map>

class NodeMap_WorldMap final
{
public:
	NodeMap_WorldMap(std::string filePath);
	~NodeMap_WorldMap();

	void        ReplaceDataPoint(unsigned int row, unsigned int col, char newVal);

	char        GetSpecificDataPoint(unsigned int row, unsigned int col);
	char**      GetDataStore() { return mNodeMapData; }

	std::string GetLevelFilePath(char request) { return mLevelLoadingFilePaths[request]; }
	Vector2D    GetSpawnPoint();

private:
	void        LoadInDataFromFile(std::string filePath);

	std::map<char, std::string> mLevelLoadingFilePaths;

	unsigned int                mWidth;
	unsigned int                mHeight;

	char**                      mNodeMapData;
};

// --------------------------------------------------------------------------- //

#endif
