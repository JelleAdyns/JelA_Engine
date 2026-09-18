#ifndef STRUCTS_H
#define STRUCTS_H

#include "Defines.h"
#include <cassert>
#include <type_traits>
#include <numbers>



namespace jela
{
	template <typename T>
	concept cArithmetic = std::is_arithmetic_v<T>;


    struct Vector2f;
	using Point2f = Vector2f;

    struct Vector2f
    {
        constexpr Vector2f() = default;
    	constexpr Vector2f(float x, float y) : x{ x }, y{ y } {}

    	constexpr Vector2f(const Point2f& startPoint, const Point2f& endPoint) :
			x{ endPoint.x - startPoint.x },
			y{ endPoint.y - startPoint.y }
    	{}

        constexpr Vector2f operator-() const{ return { -x, -y }; }
    	constexpr Vector2f operator+() const{ return { x, y }; }
    	constexpr Vector2f operator-(const Vector2f& rhs) const { return { x - rhs.x, y - rhs.y }; }
    	constexpr Vector2f operator+(const Vector2f& rhs) const { return { x + rhs.x, y + rhs.y }; }

        constexpr Vector2f& operator+=(const Vector2f& rhs) { x += rhs.x; y += rhs.y; return *this; }
		constexpr Vector2f& operator-=(const Vector2f& rhs) { x -= rhs.x; y -= rhs.y; return *this; }

		constexpr Vector2f operator*(const Vector2f& rhs) const { return { x * rhs.x, y * rhs.y }; }
		constexpr Vector2f operator/(const Vector2f& rhs) const
    	{
    		return {
    			(rhs.x < FLT_EPSILON ? 0.f : x / rhs.x),
				(rhs.y < FLT_EPSILON ? 0.f : y / rhs.y)
			};
    	}

		constexpr Vector2f operator*(cArithmetic auto rhs) const { return { static_cast<float>(x * rhs), static_cast<float>(y * rhs) }; }
		constexpr Vector2f operator/(cArithmetic auto rhs) const
		{
			assert((std::abs(rhs) > FLT_EPSILON));
			return { static_cast<float>(x / rhs), static_cast<float>(y / rhs) };
		}
		constexpr Vector2f& operator*=(cArithmetic auto rhs)
		{
			x = static_cast<float>(x * rhs);
			y = static_cast<float>(y * rhs);
			return *this;
		}
		constexpr Vector2f& operator/=(cArithmetic auto rhs)
		{
			assert((std::abs(rhs) > FLT_EPSILON));
			x = static_cast<float>(x / rhs);
			y = static_cast<float>(y / rhs);
			return *this;
		}

		constexpr bool operator==(const Vector2f& rhs) const { return (abs(x - rhs.x) < FLT_EPSILON) && (abs(y - rhs.y) < FLT_EPSILON); }
		constexpr bool operator!=(const Vector2f& rhs) const { return !(*this == rhs); }

		constexpr static float Dot(const Vector2f& first, const Vector2f& second) { return first.x * second.x + first.y * second.y; }
		constexpr static float Cross(const Vector2f& first, const Vector2f& second) { return first.x * second.y - first.y * second.x; }
		constexpr static float AngleBetween(const Vector2f& first, const Vector2f& second)
    	{
    		return atan2(first.x * second.y - second.x * first.y, first.x * second.x + first.y * second.y) * 180 / std::numbers::pi_v<float>;
    	}
		constexpr static Vector2f Reflect(const Vector2f& vector, const Vector2f& surfaceNormal)
    	{
    		const auto n = surfaceNormal.Normalized();
    		return vector - (n * 2.f * Dot(vector, n));
    	}

		constexpr tstring ToString(uint8_t decimalPrecision = 1) const { return std::format(_T("( {1:.{0}f}, {2:.{0}f} )"), decimalPrecision, x, y); }

		constexpr float Length() const { return sqrtf(x * x + y * y); }
		constexpr float SquaredLength() const { return x * x + y * y; }

    	constexpr Vector2f Normalized() const
    	{
    		const auto l = Length();
    		if (l < FLT_EPSILON) return {};
    		return { x / l, y / l };
    	}
    	constexpr Vector2f& Normalize()
    	{
    		auto l = Length();
    		if (l < FLT_EPSILON) return *this;
    		*this /= l;
    		return *this;
    	}
    	constexpr Vector2f Orthogonal() const { return { -y,x }; }

        float x{};
        float y{};
    };

	constexpr Vector2f operator*(cArithmetic auto lhs, Vector2f rhs) { return rhs * lhs; }
	constexpr tostream& operator<< (tostream& lhs, const Vector2f& rhs) { lhs << rhs.ToString(); return lhs; }


#ifdef MATHEMATICAL_COORDINATESYSTEM
	struct Rectf
	{
		constexpr Rectf() = default;
		constexpr Rectf(float left, float bottom, float width, float height) :
			left{ left },
			bottom{ bottom },
			width{ width },
			height{ height }
		{}

		constexpr Rectf(const Point2f& bottomLeft, float width, float height) :
			left{bottomLeft.x},
			bottom{bottomLeft.y},
			width{ width },
			height{ height }
		{}

		constexpr Rectf(const Point2f& bottomLeft, const Point2f& topRight) :
			left{bottomLeft.x},
			bottom{bottomLeft.y},
			width{topRight.x - bottomLeft.x},
			height{topRight.y - bottomLeft.y}
		{
			assert((topRight.x >= bottomLeft.x && topRight.y >= bottomLeft.y));
		}

		constexpr float Right() const { return left + width; }
		constexpr float Top() const { return bottom + height; }
		constexpr Point2f BottomLeft() const { return Point2f{left, bottom}; }
		constexpr Point2f BottomRight() const { return Point2f{Right(), bottom}; }
		constexpr Point2f TopLeft() const { return Point2f{left, Top()}; }
		constexpr Point2f TopRight() const { return Point2f{Right(), Top()}; }

		float left{};
		float bottom{};
		float width{};
		float height{};
	};
#else
	struct Rectf
	{
	public:
		constexpr Rectf() = default;
		constexpr Rectf(float left, float top, float width, float height) :
			left{ left },
			top{ top },
			width{ width },
			height{ height }
		{}
		constexpr Rectf(const Point2f& topLeft, float width, float height) :
			left{topLeft.x},
			top{topLeft.y},
			width{width },
			height{ height }
		{}
		constexpr Rectf(const Point2f& topLeft, const Point2f& bottomRight) :
			left{topLeft.x},
			top{topLeft.y},
			width{bottomRight.x - topLeft.x},
			height{bottomRight.y - topLeft.y}
		{
			assert((bottomRight.x >= topLeft.x && bottomRight.y >= topLeft.y));
		}

		constexpr float Right() const { return left + width; }
		constexpr float Bottom() const { return top + height; }
		constexpr Point2f BottomLeft() const { return Point2f{left, Bottom()}; }
		constexpr Point2f BottomRight() const { return Point2f{Right(), Bottom()}; }
		constexpr Point2f TopLeft() const { return Point2f{left, top}; }
		constexpr Point2f TopRight() const { return Point2f{Right(), top}; }

		float left{};
		float top{};
		float width{};
		float height{};
	};
#endif // MATHEMATICAL_COORDINATESYSTEM

	struct Ellipsef
	{
		constexpr Ellipsef() = default;
		constexpr Ellipsef(float xCenter, float yCenter, float xRadius, float yRadius) :
			center{ xCenter,yCenter },
			radiusX{ xRadius },
			radiusY{ yRadius }
		{}

		constexpr Ellipsef(const Point2f& center, float xRadius, float yRadius) :
			center{ center },
			radiusX{ xRadius },
			radiusY{ yRadius }
		{}

		Point2f center{};
		float radiusX{};
		float radiusY{};
	};

	struct Circlef
	{
		constexpr Circlef() = default;
		constexpr Circlef(float xCenter, float yCenter, float radius) :
			center{ xCenter, yCenter },
			rad{ radius }
		{}

		constexpr Circlef(const Point2f& center, float radius) :
			center{ center },
			rad{ radius }
		{}

		Point2f center{};
		float rad{};
	};
}

#endif // !STRUCTS_H

