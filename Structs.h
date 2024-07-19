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

template <typename ValueType>
requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
struct Vector2
{
	Vector2() = default;
	Vector2(ValueType x, ValueType y) :
		x{ x },
		y{ y }
	{}
	Vector2(const Point2Int& endPoint) :
		x{ static_cast<ValueType>(endPoint.x) },
		y{ static_cast<ValueType>(endPoint.y) }
	{}
	Vector2(const Point2Int& startPoint, const Point2Int& endPoint) :
		x{ static_cast<ValueType>(endPoint.x - startPoint.x) },
		y{ static_cast<ValueType>(endPoint.y - startPoint.y) }
	{}

	Vector2 operator-() const;
	Vector2 operator+() const;
	Vector2 operator-(const Vector2& rhs) const;
	Vector2 operator+(const Vector2& rhs) const;
	Vector2 operator*(auto rhs) const;
	Vector2 operator/(auto rhs) const;
	Vector2& operator*=(auto rhs);
	Vector2& operator/=(auto rhs);
	Vector2& operator+=(const Vector2& rhs);
	Vector2& operator-=(const Vector2& rhs);

	bool operator==(const Vector2<ValueType>& rhs) const;
	bool operator!=(const Vector2<ValueType>& rhs) const;

	static float Dot(const Vector2& first, const Vector2& second);
	static float Cross(const Vector2& first, const Vector2& second);
	static float AngleBetween(const Vector2& first, const Vector2& second);
	static Vector2 Reflect(const Vector2& vector, const Vector2& surfaceNormal);

	tstring	ToString() const;

	float Length() const;
	float SquaredLength() const;

	Vector2 Normalized() const;
	Vector2& Normalize();
	Vector2 Orthogonal() const;


	ValueType x;
	ValueType y;
};


template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
inline float Vector2<ValueType>::Dot(const Vector2& first, const Vector2& second)
{
	return first.x * second.x + first.y * second.y;
}

template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
inline float Vector2<ValueType>::Cross(const Vector2& first, const Vector2& second)
{
	return first.x * second.y - first.y * second.x;
}

template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
inline float Vector2<ValueType>::AngleBetween(const Vector2& first, const Vector2& second)
{
	return atan2(first.x * second.y - second.x * first.y, first.x * second.x + first.y * second.y);
}

template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
inline Vector2<ValueType> Vector2<ValueType>::Reflect(const Vector2& vector, const Vector2& surfaceNormal)
{
	return vector - (2.f * Dot(vector, surfaceNormal) * surfaceNormal);
}


template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
inline tstring Vector2<ValueType>::ToString() const
{
	return _T("( ") + to_tstring(x) + _T(", ") + to_tstring(y) + _T(" )");
}

template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
inline float Vector2<ValueType>::Length() const
{
	return sqrtf(static_cast<float>(x * x + y * y));
}

template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
inline float Vector2<ValueType>::SquaredLength() const
{
	return static_cast<float>(x * x + y * y);
}

template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
inline Vector2<ValueType> Vector2<ValueType>::Normalized() const
{
	auto l = Length();
	return { x / l, y / l };
}

template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
inline Vector2<ValueType>& Vector2<ValueType>::Normalize()
{
	auto l = Length();
	x /= l;
	y /= l;
	return *this;
}

template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
inline Vector2<ValueType> Vector2<ValueType>::Orthogonal() const
{
	return { -y,x };
}

////////////////
// Operators
////////////////

// members
template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
inline Vector2<ValueType> Vector2<ValueType>::operator-() const
{
	return { -x, -y };
}

template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
inline Vector2<ValueType> Vector2<ValueType>::operator+() const
{
	return { x, y };
}

template<typename ValueType>
inline Vector2<ValueType> Vector2<ValueType>::operator-(const Vector2& rhs) const
{
	return { x - rhs.x, y - rhs.y };
}

template<typename ValueType>
inline Vector2<ValueType> Vector2<ValueType>::operator+(const Vector2& rhs) const
{
	return { x + rhs.x, y + rhs.y };
}

template<typename ValueType>
inline Vector2<ValueType> Vector2<ValueType>::operator*(auto rhs) const
{
	return { x * rhs, y * rhs };
}

template<typename ValueType>
inline Vector2<ValueType> Vector2<ValueType>::operator/(auto rhs) const
{
	assert((rhs != 0));

	return { x / rhs, y / rhs };
}

template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
inline Vector2<ValueType>& Vector2<ValueType>::operator*=(auto rhs)
{
	x *= rhs;
	y *= rhs;
	return *this;
}

template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
inline Vector2<ValueType>& Vector2<ValueType>::operator/=(auto rhs)
{
	assert((rhs != 0));

	x /= rhs;
	y /= rhs;
	return *this;
}

template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
inline Vector2<ValueType>& Vector2<ValueType>::operator+=(const Vector2& rhs)
{
	x += rhs.x;
	y += rhs.y;
	return *this;
}

template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
inline Vector2<ValueType>& Vector2<ValueType>::operator-=(const Vector2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	return *this;
}

template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
bool Vector2<ValueType>::operator==(const Vector2<ValueType>& rhs) const
{
	return (abs(x - rhs.x) < 0.0001f) && (abs(y - rhs.y) < 0.0001f);
}

template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
bool Vector2<ValueType>::operator!=(const Vector2<ValueType>& rhs) const
{
	return !(*this == rhs);
}

// non-members
template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
Vector2<ValueType> operator*(float lhs, Vector2<ValueType> rhs)
{
	return rhs * lhs;
}

template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
tostream& operator<< (tostream& lhs, const Vector2<ValueType>& rhs)
{
	lhs << rhs.ToString();
	return lhs;
}

template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
Point2Int& operator+=(Point2Int& lhs, const Vector2<ValueType>& rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	return lhs;
}
template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
Point2Int operator+(const Point2Int& lhs, const Vector2<ValueType>& rhs)
{
	return Point2Int{ lhs.x + rhs.x, lhs.y +lhs.y };
}
template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
Point2Int& operator-=(Point2Int& lhs, const Vector2<ValueType>& rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	return lhs;
}
template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
Point2Int operator-(const Point2Int& lhs, const Vector2<ValueType>& rhs)
{
	return Point2Int{ static_cast<int>(lhs.x - rhs.x), static_cast<int>(lhs.y - lhs.y) };
}

template<typename ValueType>
	requires std::is_arithmetic_v<ValueType> and std::is_fundamental_v<ValueType>
Vector2<ValueType> operator-(const Point2Int& lhs, const Point2Int& rhs)
{
	return { static_cast<ValueType>(lhs.x - rhs.x),static_cast<ValueType>(lhs.y - rhs.y) }
}

#endif // !STRUCTS_H

