#include "Structs.h"

Point2Int::Point2Int(int x, int y):
	x{x},
	y{y}
{}
bool Point2Int::operator==(const Point2Int& rhs)
{
	return x == rhs.x and y == rhs.y;
}
bool Point2Int::operator!=(const Point2Int& rhs)
{
	return x != rhs.x or y != rhs.y;
}


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

Vector2f::Vector2f(float x, float y) :
	x{ x },
	y{ y }
{}
Vector2f::Vector2f(const Point2Int& endPoint) :
	x{ static_cast<float>(endPoint.x) },
	y{ static_cast<float>(endPoint.y) }
{}
Vector2f::Vector2f(const Point2Int& startPoint, const Point2Int& endPoint) :
	x{ static_cast<float>(endPoint.x - startPoint.x) },
	y{ static_cast<float>(endPoint.y - startPoint.y) }
{}


float Vector2f::Dot(const Vector2f& first, const Vector2f& second)
{
	return first.x * second.x + first.y * second.y;
}
float Vector2f::Cross(const Vector2f& first, const Vector2f& second)
{
	return first.x * second.y - first.y * second.x;
}
float Vector2f::AngleBetween(const Vector2f& first, const Vector2f& second)
{
	return atan2(first.x * second.y - second.x * first.y, first.x * second.x + first.y * second.y);
}
Vector2f Vector2f::Reflect(const Vector2f& vector, const Vector2f& surfaceNormal)
{
	return vector - (2.f * Dot(vector, surfaceNormal) * surfaceNormal);
}

tstring Vector2f::ToString() const
{
	return _T("( ") + to_tstring(x) + _T(", ") + to_tstring(y) + _T(" )");
}
float Vector2f::Length() const
{
	return sqrtf(static_cast<float>(x * x + y * y));
}
float Vector2f::SquaredLength() const
{
	return static_cast<float>(x * x + y * y);
}
Vector2f Vector2f::Normalized() const
{
	auto l = Length();
	if (l < 0.0001f) return {};
	return { x / l, y / l };
}
Vector2f& Vector2f::Normalize()
{
	auto l = Length();
	if (l < 0.0001f) return *this;
	*this /= l;
	return *this;
}
Vector2f Vector2f::Orthogonal() const
{
	return { -y,x };
}

////////////////
// Operators
////////////////

// member
Vector2f Vector2f::operator-() const
{
	return { -x, -y };
}
Vector2f Vector2f::operator+() const
{
	return { x, y };
}

Vector2f Vector2f::operator-(const Vector2f& rhs) const
{
	return { x - rhs.x, y - rhs.y };
}

Vector2f Vector2f::operator+(const Vector2f& rhs) const
{
	return { x + rhs.x, y + rhs.y };
}

Vector2f Vector2f::operator*(auto rhs) const
{
	return { x * rhs, y * rhs };
}

Vector2f Vector2f::operator/(auto rhs) const
{
	assert((rhs != 0));

	return { x / rhs, y / rhs };
}
Vector2f& Vector2f::operator*=(auto rhs)
{
	x *= rhs;
	y *= rhs;
	return *this;
}
Vector2f& Vector2f::operator/=(auto rhs)
{
	assert((rhs != 0));

	x /= rhs;
	y /= rhs;
	return *this;
}
Vector2f& Vector2f::operator+=(const Vector2f& rhs)
{
	x += rhs.x;
	y += rhs.y;
	return *this;
}
Vector2f& Vector2f::operator-=(const Vector2f& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	return *this;
}
bool Vector2f::operator==(const Vector2f& rhs) const
{
	return (abs(x - rhs.x) < 0.0001f) && (abs(y - rhs.y) < 0.0001f);
}
bool Vector2f::operator!=(const Vector2f& rhs) const
{
	return not (*this == rhs);
}

// non-member
Vector2f operator*(float lhs, Vector2f rhs)
{
	return rhs * lhs;
}
tostream& operator<< (tostream& lhs, const Vector2f& rhs)
{
	lhs << rhs.ToString();
	return lhs;
}
Point2Int& operator+=(Point2Int& lhs, const Vector2f& rhs)
{
	lhs.x += static_cast<int>(rhs.x);
	lhs.y += static_cast<int>(rhs.y);
	return lhs;
}
Point2Int operator+(const Point2Int& lhs, const Vector2f& rhs)
{
	return Point2Int{ static_cast<int>(lhs.x + rhs.x), static_cast<int>(lhs.y + rhs.y) };
}
Point2Int& operator-=(Point2Int& lhs, const Vector2f& rhs)
{
	lhs.x -= static_cast<int>(rhs.x);
	lhs.y -= static_cast<int>(rhs.y);
	return lhs;
}
Point2Int operator-(const Point2Int& lhs, const Vector2f& rhs)
{
	return Point2Int{ static_cast<int>(lhs.x - rhs.x), static_cast<int>(lhs.y - rhs.y) };
}
Vector2f operator-(const Point2Int& lhs, const Point2Int& rhs)
{
	return { static_cast<float>(lhs.x - rhs.x),static_cast<float>(lhs.y - rhs.y) };
}