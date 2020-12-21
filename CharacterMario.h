#ifndef _CHARACTER_MARIO_H
#define _CHARACTER_MARIO_H

#include "Character.h"

class CharacterMario final : public Character
{
public:
	CharacterMario() = delete;
	CharacterMario(SDL_Renderer* renderer, std::string imagePath, Vector2D startingPosition);
	~CharacterMario();

	void Update(float deltaTime, SDL_Event e) override;
	void Render() override;


};
#endif // !_CHARACTER_MARIO_H
