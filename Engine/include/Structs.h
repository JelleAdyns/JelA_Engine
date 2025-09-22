#ifndef STRUCTS_H
#define STRUCTS_H

#include "Defines.h"
#include <cassert>
#include <type_traits>



namespace jela
{
	template <typename T>
	concept cArithmetic = std::is_arithmetic_v<T>;

	struct Point2f
	{
		Point2f() = default;
		explicit Point2f(float x, float y);
		float x;
		float y;

		bool operator==(const Point2f& rhs) const;
		bool operator!=(const Point2f& rhs) const;
	};


#ifdef MATHEMATICAL_COORDINATESYSTEM
	struct Rectf
	{
		Rectf() = default;
		explicit Rectf(float left, float bottom, float width, float height);
		explicit Rectf(const Point2f& leftBottom, float width, float height);

		float left;
		float bottom;
		float width;
		float height;
	};
#else
	struct Rectf
	{
	public:
		Rectf() = default;
		explicit Rectf(float left, float top, float width, float height);
		explicit Rectf(const Point2f& leftTop, float width, float height);

		float left;
		float top;
		float width;
		float height;
	};
#endif // MATHEMATICAL_COORDINATESYSTEM

	struct Ellipsef
	{
		Ellipsef() = default;
		explicit Ellipsef(float xCenter, float yCenter, float xRadius, float yRadius);
		explicit Ellipsef(const Point2f& center, float xRadius, float yRadius);

		Point2f center;
		float radiusX;
		float radiusY;
	};

	struct Circlef
	{
		Circlef() = default;
		explicit Circlef(float xCenter, float yCenter, float radius);
		explicit Circlef(const Point2f& center, float radius);

		Point2f center;
		float rad;
	};


    struct Vector2f
    {
        Vector2f() = default;
        Vector2f(float x, float y);
        Vector2f(const Point2f& endPoint);
        Vector2f(const Point2f& startPoint, const Point2f& endPoint);

        Vector2f operator-() const;
		Vector2f operator+() const;
		Vector2f operator-(const Vector2f& rhs) const;
		Vector2f operator+(const Vector2f& rhs) const;

        Vector2f& operator+=(const Vector2f& rhs);
		Vector2f& operator-=(const Vector2f& rhs);

		Vector2f operator*(cArithmetic auto rhs) const
		{
			return { static_cast<float>(x * rhs), static_cast<float>(y * rhs) };
		}
		Vector2f operator/(cArithmetic auto rhs) const
		{
			assert((std::abs(rhs) > FLT_EPSILON));
			return { static_cast<float>(x / rhs), static_cast<float>(y / rhs) };
		}
		Vector2f& operator*=(cArithmetic auto rhs)
		{
			x = static_cast<float>(x * rhs);
			y = static_cast<float>(y * rhs);
			return *this;
		}
		Vector2f& operator/=(cArithmetic auto rhs)
		{
			assert((std::abs(rhs) > FLT_EPSILON));
			x = static_cast<float>(x / rhs);
			y = static_cast<float>(y / rhs);
			return *this;
		}

		bool operator==(const Vector2f& rhs) const;
		bool operator!=(const Vector2f& rhs) const;

		static float Dot(const Vector2f& first, const Vector2f& second);
		static float Cross(const Vector2f& first, const Vector2f& second);
		static float AngleBetween(const Vector2f& first, const Vector2f& second);
		static Vector2f Reflect(const Vector2f& vector, const Vector2f& surfaceNormal);

		tstring	ToString(uint8_t decimalPrecision = 1) const;

		float Length() const;
		float SquaredLength() const;

		Vector2f Normalized() const;
		Vector2f& Normalize();
		Vector2f Orthogonal() const;


		float x;
		float y;
	};

	Vector2f operator*(cArithmetic auto lhs, Vector2f rhs)
	{
		return rhs * lhs;
	}

	tostream& operator<< (tostream& lhs, const Vector2f& rhs);

	Point2f& operator+=(Point2f& lhs, const Vector2f& rhs);
	Point2f operator+(const Point2f& lhs, const Vector2f& rhs);
	Point2f& operator-=(Point2f& lhs, const Vector2f& rhs);
	Point2f operator-(const Point2f& lhs, const Vector2f& rhs);

	Vector2f operator-(const Point2f& lhs, const Point2f& rhs);
}

#endif // !STRUCTS_H

