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
#else
	RectInt::RectInt(int left, int top, int width, int height):
		left{ left },
		top{ top },
		width{ width },
		height{ height }
	{}
#endif // MATHEMATICAL_COORDINATESYSTEM


EllipseInt::EllipseInt(int xCenter, int yCenter, int xRadius, int yRadius) :
	centerX{ xCenter },
	centerY{ yCenter },
	radiusX{ xRadius },
	radiusY{ yRadius }
{}