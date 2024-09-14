#ifndef STRUCTS_H
#define STRUCTS_H

#include "Defines.h"
#include <cassert>

struct Point2Int
{
	Point2Int() = default;
	explicit Point2Int(int x, int y);
	int x;
	int y;

	bool operator==(const Point2Int& rhs);
	bool operator!=(const Point2Int& rhs);
};


#ifdef MATHEMATICAL_COORDINATESYSTEM
struct RectInt
{
	RectInt() = default;
	explicit RectInt(int left, int bottom, int width, int height);
	explicit RectInt(const Point2Int& leftBottom, int width, int height);

	int left;
	int bottom;
	int width;
	int height;
};
#else
struct RectInt
{
public:
	RectInt() = default;
	explicit RectInt(int left, int top, int width, int height);
	explicit RectInt(const Point2Int& leftTop, int width, int height);

	int left;
	int top;
	int width;
	int height;
};
#endif // MATHEMATICAL_COORDINATESYSTEM

struct EllipseInt
{
	EllipseInt() = default;
	explicit EllipseInt(int xCenter, int yCenter, int xRadius, int yRadius);
	explicit EllipseInt(const Point2Int& center, int xRadius, int yRadius);

	Point2Int center;
	int radiusX;
	int radiusY;
};

struct CircleInt
{
	CircleInt() = default;
	explicit CircleInt(int xCenter, int yCenter, int radius);
	explicit CircleInt(const Point2Int& center, int radius);

	Point2Int center;
	int rad;
};


struct Vector2f
{
	Vector2f() = default;
	Vector2f(float x, float y);
	Vector2f(const Point2Int& endPoint);
	Vector2f(const Point2Int& startPoint, const Point2Int& endPoint);

	Vector2f operator-() const;
	Vector2f operator+() const;
	Vector2f operator-(const Vector2f& rhs) const;
	Vector2f operator+(const Vector2f& rhs) const;
	Vector2f operator*(auto rhs) const;
	Vector2f operator/(auto rhs) const;
	Vector2f& operator*=(auto rhs);
	Vector2f& operator/=(auto rhs);
	Vector2f& operator+=(const Vector2f& rhs);
	Vector2f& operator-=(const Vector2f& rhs);

	bool operator==(const Vector2f& rhs) const;
	bool operator!=(const Vector2f& rhs) const;

	static float Dot(const Vector2f& first, const Vector2f& second);
	static float Cross(const Vector2f& first, const Vector2f& second);
	static float AngleBetween(const Vector2f& first, const Vector2f& second);
	static Vector2f Reflect(const Vector2f& vector, const Vector2f& surfaceNormal);

	tstring	ToString() const;

	float Length() const;
	float SquaredLength() const;

	Vector2f Normalized() const;
	Vector2f& Normalize();
	Vector2f Orthogonal() const;


	float x;
	float y;
};

Vector2f operator*(float lhs, Vector2f rhs);
tostream& operator<< (tostream& lhs, const Vector2f& rhs);

Point2Int& operator+=(Point2Int& lhs, const Vector2f& rhs);
Point2Int operator+(const Point2Int& lhs, const Vector2f& rhs);
Point2Int& operator-=(Point2Int& lhs, const Vector2f& rhs);
Point2Int operator-(const Point2Int& lhs, const Vector2f& rhs);

Vector2f operator-(const Point2Int& lhs, const Point2Int& rhs);

#endif // !STRUCTS_H

