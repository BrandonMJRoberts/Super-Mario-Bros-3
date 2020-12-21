#ifndef _GAME_MATHS_H_
#define _GAME_MATHS_H_

struct Vector2D
{
	Vector2D operator*(const int other)
	{
		return Vector2D(this->x * other, this->y * other);
	}

	Vector2D operator/(const int other)
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

	Vector2D operator-(const Vector2D other)
	{
		return Vector2D(this->x - other.x, this->y - other.y);
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

	double x;
	double y;
};

#endif