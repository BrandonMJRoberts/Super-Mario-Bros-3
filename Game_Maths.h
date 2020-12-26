#ifndef _GAME_MATHS_H_
#define _GAME_MATHS_H_

#include <math.h>

struct Vector2D
{
	Vector2D operator*(const double other)
	{
		return Vector2D(this->x * other, this->y * other);
	}

	Vector2D operator/(const double other)
	{
		if (other != 0)
			return Vector2D(this->x / other, this->y / other);
		else
			return *this;
	}

	Vector2D operator+(const Vector2D other)
	{
		return Vector2D(this->x + other.x, this->y + other.y);
	}

	Vector2D operator+=(const Vector2D other)
	{
		return Vector2D(this->x + other.x, this->y + other.y);
	}

	Vector2D operator-(const Vector2D other)
	{
		return Vector2D(this->x - other.x, this->y - other.y);
	}

	bool operator==(const Vector2D other)
	{
		if (this->x == other.x && this->y == other.y)
			return true;

		return false;
	}

	Vector2D()
	{
		x = 0.0f;
		y = 0.0f;
	}

	Vector2D(double startX, double startY)
	{
		x = startX;
		y = startY;
	}

	void normalise()
	{
		// First calculate the length of the vector
		double length = sqrt((x * x) + (y * y));

		// Now divide both axis by the length
		if (length > 0)
		{
			x /= length;
			y /= length;
		}
	}

	double length()
	{
		return sqrt((x * x) + (y * y));
	}

	double lengthSquared()
	{
		return (x * x) + (y * y);
	}

	double x;
	double y;
};

#endif