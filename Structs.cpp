#include "Structs.h"

Point2Int::Point2Int(int x, int y):
	x{x},
	y{y}
{}

#ifdef MATHEMATICAL_COORDINATESYSTEM
	RectInt::RectInt(int left, int bottom, int width, int height):
		left{ left },
		bottom{ bottom },
		width{ width },
		height{ height }
	{}
	RectInt::RectInt(const Point2Int& leftBottom, int width, int height) :
		left{ leftBottom.x },
		bottom{ leftBottom.y },
		width{ width },
		height{ height }
	{}
#else
	RectInt::RectInt(int left, int top, int width, int height):
		left{ left },
		top{ top },
		width{ width },
		height{ height }
	{}
	RectInt::RectInt(const Point2Int& leftTop, int width, int height) :
		left{ leftTop.x },
		top{ leftTop.y },
		width{ width },
		height{ height }
	{}
#endif // MATHEMATICAL_COORDINATESYSTEM


EllipseInt::EllipseInt(int xCenter, int yCenter, int xRadius, int yRadius) :
	center{ xCenter,yCenter },
	radiusX{ xRadius },
	radiusY{ yRadius }
{}

EllipseInt::EllipseInt(const Point2Int& center, int xRadius, int yRadius) :
	center{center},
	radiusX{xRadius},
	radiusY{yRadius}
{}


CircleInt::CircleInt(int xCenter, int yCenter, int radius) :
	center{ xCenter, yCenter },
	rad{ radius }
{
}

CircleInt::CircleInt(const Point2Int& center, int radius) :
	center{ center},
	rad{ radius }
{
}


