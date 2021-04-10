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
	// Taking the reference point of (0,0) at bottom left
	// And that going up the screen decreases the y axis value

	if (   rect1.x + rect1.width < rect2.x 
		|| rect1.x > rect2.x + rect2.width)
		return false;

	if(    rect1.y < rect2.y - rect2.height 
		|| rect1.y - rect1.height > rect2.y)
		return false;

	return true;
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