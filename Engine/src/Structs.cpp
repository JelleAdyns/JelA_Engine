#include "Structs.h"
#include <numbers>

namespace jela
{

	Point2f::Point2f(float x, float y) :
		x{ x },
		y{ y }
	{}
	bool Point2f::operator==(const Point2f& rhs) const
	{
		return (abs(x - rhs.x) < FLT_EPSILON) && (abs(y - rhs.y) < FLT_EPSILON);
	}
	bool Point2f::operator!=(const Point2f& rhs) const
	{
		return !(*this == rhs);
	}


#ifdef MATHEMATICAL_COORDINATESYSTEM
	Rectf::Rectf(float left, float bottom, float width, float height) :
		left{ left },
		bottom{ bottom },
		width{ width },
		height{ height }
	{}
	Rectf::Rectf(const Point2f& leftBottom, float width, float height) :
		left{ leftBottom.x },
		bottom{ leftBottom.y },
		width{ width },
		height{ height }
	{}
#else
	Rectf::Rectf(float left, float top, float width, float height) :
		left{ left },
		top{ top },
		width{ width },
		height{ height }
	{}
	Rectf::Rectf(const Point2f& leftTop, float width, float height) :
		left{ leftTop.x },
		top{ leftTop.y },
		width{ width },
		height{ height }
	{}
#endif // MATHEMATICAL_COORDINATESYSTEM


	Ellipsef::Ellipsef(float xCenter, float yCenter, float xRadius, float yRadius) :
		center{ xCenter,yCenter },
		radiusX{ xRadius },
		radiusY{ yRadius }
	{}

	Ellipsef::Ellipsef(const Point2f& center, float xRadius, float yRadius) :
		center{ center },
		radiusX{ xRadius },
		radiusY{ yRadius }
	{}


	Circlef::Circlef(float xCenter, float yCenter, float radius) :
		center{ xCenter, yCenter },
		rad{ radius }
	{
	}

	Circlef::Circlef(const Point2f& center, float radius) :
		center{ center },
		rad{ radius }
	{
	}

	Vector2f::Vector2f(float x, float y) :
		x{ x },
		y{ y }
	{}
	Vector2f::Vector2f(const Point2f& endPoint) :
		x{ endPoint.x },
		y{ endPoint.y }
	{}
	Vector2f::Vector2f(const Point2f& startPoint, const Point2f& endPoint) :
		x{ endPoint.x - startPoint.x },
		y{ endPoint.y - startPoint.y }
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
		return atan2(first.x * second.y - second.x * first.y, first.x * second.x + first.y * second.y) * 180 / std::numbers::pi_v<float>;
	}
	Vector2f Vector2f::Reflect(const Vector2f& vector, const Vector2f& surfaceNormal)
	{
		return vector - (2.f * Dot(vector, surfaceNormal.Normalized()) * surfaceNormal.Normalized());
	}

	tstring Vector2f::ToString(uint8_t decimalPrecision) const
	{
		return _T("( ") + std::format(_T("{:.{}f}"), x, decimalPrecision) +
			_T(", ") + std::format(_T("{:.{}f}"), y, decimalPrecision) + _T(" )");
	}
	float Vector2f::Length() const
	{
		return sqrtf(x * x + y * y);
	}
	float Vector2f::SquaredLength() const
	{
		return x * x + y * y;
	}
	Vector2f Vector2f::Normalized() const
	{
		auto l = Length();
		if (l < FLT_EPSILON) return {};
		return { x / l, y / l };
	}
	Vector2f& Vector2f::Normalize()
	{
		auto l = Length();
		if (l < FLT_EPSILON) return *this;
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
		return (abs(x - rhs.x) < FLT_EPSILON) && (abs(y - rhs.y) < FLT_EPSILON);
	}
	bool Vector2f::operator!=(const Vector2f& rhs) const
	{
		return !(*this == rhs);
	}

	// non-member

	tostream& operator<< (tostream& lhs, const Vector2f& rhs)
	{
		lhs << rhs.ToString();
		return lhs;
	}
	Point2f& operator+=(Point2f& lhs, const Vector2f& rhs)
	{
		lhs.x += rhs.x;
		lhs.y += rhs.y;
		return lhs;
	}
	Point2f operator+(const Point2f& lhs, const Vector2f& rhs)
	{
		return Point2f{ lhs.x + rhs.x, lhs.y + rhs.y };
	}
	Point2f& operator-=(Point2f& lhs, const Vector2f& rhs)
	{
		lhs.x -= rhs.x;
		lhs.y -= rhs.y;
		return lhs;
	}
	Point2f operator-(const Point2f& lhs, const Vector2f& rhs)
	{
		return Point2f{ lhs.x - rhs.x, lhs.y - rhs.y };
	}
	Vector2f operator-(const Point2f& lhs, const Point2f& rhs)
	{
		return { lhs.x - rhs.x, lhs.y - rhs.y };
	}

}
