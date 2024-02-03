#ifndef STRUCTS_H
#define STRUCTS_H

#include "Defines.h"

struct Point2Int
{
	Point2Int() = default;
	explicit Point2Int(int x, int y);
	int x;
	int y;
};


#ifdef MATHEMATICAL_COORDINATESYSTEM
struct RectInt
{
public:
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

#endif // !STRUCTS_H

