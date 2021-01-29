#ifndef _GAME_SCREEN_WORLD_MAP_SMB3_H_
#define _GAME_SCREEN_WORLD_MAP_SMB3_H_

#include "GameScreen_SMB3.h"
#include <map>

#include "BackgroundLayer_WorldMap.h"
#include "NodeMap_WorldMap.h"
#include "BorderWorldMap.h"

#include "BaseWorldMapCharacter.h"

class GameScreen_WorldMap_SMB3 final : public GameScreen_SMB3
{
public:
	GameScreen_WorldMap_SMB3() = delete;
	GameScreen_WorldMap_SMB3(SDL_Renderer* renderer, Audio_Player* audioPlayerRef);
	~GameScreen_WorldMap_SMB3();

	void Render() override;
	ReturnDataFromGameScreen Update(const float deltaTime, SDL_Event e) override;

private:
	void SetupConversionTable();

	BackgroundLayer_WorldMap* mBackground;
	NodeMap_WorldMap*         mNodeMap;
	WorldMapBorder*           mBorder;
	BaseWorldMapCharacter*    mPlayer;

	std::map<char, unsigned int> mConversionTable;

	//Audio_Player*             mAudioPlayer;

	unsigned int              mWorldID;
};

#endif
