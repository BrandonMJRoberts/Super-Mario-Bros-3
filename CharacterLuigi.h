#ifndef _CHARACTER_LUIGI_H_
#define _CHARACTER_LUIGI_H_

#include "Character.h"

class CharacterLuigi final : public Character
{
public:
	CharacterLuigi() = delete;
	CharacterLuigi(SDL_Renderer* renderer, std::string imagePath, Vector2D startingPosition);
	~CharacterLuigi();

	void Update(float deltaTime, SDL_Event e) override;
	void Render() override;

};

#endif // !_CHARACTER_LUIGI_H_
