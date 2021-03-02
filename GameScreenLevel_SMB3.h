#ifndef _GAME_SCREEN_LEVEL_H_
#define _GAME_SCREEN_LEVEL_H_

#include "BaseObject.h"
#include "GameScreen_SMB3.h"

#include <map>
#include <vector>

struct SDL_Renderer;
class  Texture2D;
class  LevelAreas;
class  PlayableCharacter;
class  Observer;
//class  Transition;
class  VisualTransitionHandler;

// ----------------------------------------------------------------------------------------------------------- //

class GameScreenLevel_SMB3 final : public GameScreen_SMB3
{
public:
	GameScreenLevel_SMB3() = delete;
	GameScreenLevel_SMB3(SDL_Renderer* renderer, const char* levelFilePath, const bool playingAsMario, Observer& HUD_Observer, LEVEL_TYPE levelType, Audio_Player* audioPlayerRef, Observer* hudObserver);
	~GameScreenLevel_SMB3();

	void Render() override;
	ReturnDataFromGameScreen Update(const float deltaTime, SDL_Event e) override;

private:
	void TransitionToNewArea(Area_Transition_Data data, bool pipeTransition);

	void InitialiseLookUpTable();
	void HandleInput(const float deltaTime, SDL_Event e);

	std::map<char, unsigned int> mConversionFromCharToIntIndexMap;

	// List of all the areas found in this level
	std::vector<LevelAreas*> mAreas;

	// Id of which level area the player is currently in (will default to the area named 'overworld')
	unsigned int             mCurrentLevelAreaID;

	const SDL_Renderer*      mRenderer;

	PlayableCharacter*       mPlayer;

	//Transition*              mFadeInOutTransition;

	VisualTransitionHandler* mTransitionHandler;
};

// ----------------------------------------------------------------------------------------------------------- //

#endif
