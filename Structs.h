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

	int centerX;
	int centerY;
	int radiusX;
	int radiusY;
};

#endif // !STRUCTS_H

