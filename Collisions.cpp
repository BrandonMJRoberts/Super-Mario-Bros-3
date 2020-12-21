#include "Collisions.h"

#include "Character.h"

Collisions* Collisions::mInstance = nullptr;

// --------------------------------------------------------- //

Collisions::Collisions()
{

}

// --------------------------------------------------------- //

Collisions::~Collisions()
{
	mInstance = nullptr;
}

// --------------------------------------------------------- //

Collisions* Collisions::Instance()
{
	if (mInstance == nullptr)
		mInstance = new Collisions;

	return mInstance;
}

// --------------------------------------------------------- //

bool Collisions::Box(Rect2D rect1, Rect2D rect2)
{
	if (rect1.x + (rect1.width / 2.0f) > rect2.x &&
		rect1.x + (rect1.width / 2.0f) < rect2.x + rect2.width &&
		rect1.y + (rect1.height / 2.0f) > rect2.y &&
		rect1.y + (rect1.height / 2.0f) < rect2.y + rect2.height)
	{
		return true;
	}

	return false;
}

// --------------------------------------------------------- //

bool Collisions::Circle(Character* character1, Character* character2)
{
	// First calculate the distance between the two characters
	Vector2D vectorBetween = Vector2D( character1->GetPosition().x - character2->GetPosition().x, character1->GetPosition().y - character2->GetPosition().y);

	double distanceSquared = (vectorBetween.x * vectorBetween.x) + (vectorBetween.y + vectorBetween.y);

	double combinedDistance = character1->GetCollisionRadius() + character2->GetCollisionRadius();
	combinedDistance *= combinedDistance;

	// Return if the distance is less than the radius required
	return distanceSquared < combinedDistance;
}

// --------------------------------------------------------- //