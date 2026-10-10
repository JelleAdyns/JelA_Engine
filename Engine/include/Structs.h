#ifndef STRUCTS_H
#define STRUCTS_H

#include "Defines.h"
#include <cassert>
#include <type_traits>
#include <numbers>



namespace jela
{
	template <typename T>
	concept cFloatArithmetic =
		std::is_arithmetic_v<T> &&
			std::is_signed_v<T> &&
				std::numeric_limits<T>::max() <= std::numeric_limits<float>::max() &&
					std::numeric_limits<T>::min() >= std::numeric_limits<float>::min();


    struct Vector2f;
	using Point2f = Vector2f;

	struct Vector3f;
	using Point3f = Vector3f;

    struct Vector2f
    {
        constexpr Vector2f() = default;
    	constexpr Vector2f(float x, float y) : x{ x }, y{ y } {}

    	constexpr Vector2f(const Point2f& startPoint, const Point2f& endPoint) :
			x{ endPoint.x - startPoint.x },
			y{ endPoint.y - startPoint.y }
    	{}

    	constexpr float& operator[](int index) { assert(index == 0 || index == 1);  return (index == 0) ? x : y; };
    	constexpr float operator[](int index) const { assert(index == 0 || index == 1);  return (index == 0) ? x : y;};
        constexpr Vector2f operator-() const{ return { -x, -y }; }
    	constexpr Vector2f operator+() const{ return { x, y }; }
    	constexpr Vector2f operator-(const Vector2f& rhs) const { return { x - rhs.x, y - rhs.y }; }
    	constexpr Vector2f operator+(const Vector2f& rhs) const { return { x + rhs.x, y + rhs.y }; }

        constexpr Vector2f& operator+=(const Vector2f& rhs) { x += rhs.x; y += rhs.y; return *this; }
		constexpr Vector2f& operator-=(const Vector2f& rhs) { x -= rhs.x; y -= rhs.y; return *this; }

		constexpr Vector2f& Scale(float xScale, float yScale) { x *= xScale; y *= yScale; return *this; }
		constexpr Vector2f& Scale(const Vector2f& scale) { return Scale(scale.x, scale.y); }
		constexpr static Vector2f Scale(const Vector2f& vecToScale, float xScale, float yScale) { return {vecToScale.x * xScale, vecToScale.y * yScale};}
		constexpr static Vector2f Scale(const Vector2f& vecToScale, const Vector2f& scale) {return Scale(vecToScale, scale.x, scale.y); }

		constexpr Vector2f operator*(cFloatArithmetic auto rhs) const { return { x * rhs, y * rhs }; }
		constexpr Vector2f operator/(cFloatArithmetic auto rhs) const
		{
			assert((std::abs(static_cast<float>(rhs)) > FLT_EPS)); return { x / rhs, y / rhs };
		}
		constexpr Vector2f& operator*=(cFloatArithmetic auto rhs) { x = x * rhs; y = y * rhs; return *this; }
		constexpr Vector2f& operator/=(cFloatArithmetic auto rhs)
		{
			assert((std::abs(static_cast<float>(rhs)) > FLT_EPS)); x = x / rhs; y = y / rhs; return *this;
		}

    	bool operator==(const Vector2f& rhs) const { return (std::abs(x - rhs.x) < FLT_EPS) && (std::abs(y - rhs.y) < FLT_EPS); }
		bool operator!=(const Vector2f& rhs) const { return !(*this == rhs); }

		constexpr static float Dot(const Vector2f& first, const Vector2f& second) { return first.x * second.x + first.y * second.y; }
		constexpr static float Cross(const Vector2f& first, const Vector2f& second) { return first.x * second.y - first.y * second.x; }
		static float AngleBetween(const Vector2f& first, const Vector2f& second, bool inDegrees = true)
    	{
    		const float angle = std::atan2(first.x * second.y - second.x * first.y, first.x * second.x + first.y * second.y);
    		if (inDegrees) return angle * 180 / std::numbers::pi_v<float>;
    		return angle;
    	}
		static Vector2f Reflect(const Vector2f& vector, const Vector2f& surfaceNormal)
    	{
    		const auto n = surfaceNormal.Normalized(); return vector - (n * 2.f * Dot(vector, n));
    	}

		tstring ToString(uint8_t decimalPrecision = 1) const { return std::format(_T("( {1:.{0}f}, {2:.{0}f} )"), decimalPrecision, x, y); }

		float Length() const { return std::sqrtf(x * x + y * y); }
		constexpr float SquaredLength() const { return x * x + y * y; }

    	Vector2f Normalized() const { const auto l = Length(); if (l < FLT_EPS) return {}; return { x / l, y / l }; }
    	Vector2f& Normalize() { const auto l = Length(); if (l < FLT_EPS) return *this; return *this /= l; }
    	constexpr Vector2f Orthogonal() const { return { -y,x }; }

        float x{};
        float y{};

		const static Vector2f UnitX;
		const static Vector2f UnitY;
		const static Vector2f Zero;

    private:
    	constexpr static float FLT_EPS = std::numeric_limits<float>::epsilon();
    };

	inline constexpr Vector2f Vector2f::UnitX{1,0};
	inline constexpr Vector2f Vector2f::UnitY{0,1};
	inline constexpr Vector2f Vector2f::Zero{0,0};

	constexpr Vector2f operator*(cFloatArithmetic auto lhs, const Vector2f& rhs) { return rhs * lhs; }
	inline tostream& operator<< (tostream& lhs, const Vector2f& rhs) { lhs << rhs.ToString(); return lhs; }

	struct Vector3f
    {
        constexpr Vector3f() = default;
    	constexpr Vector3f(float x, float y, float z) : x{ x }, y{ y }, z{ z } {}

    	constexpr Vector3f(const Point3f& startPoint, const Point3f& endPoint) :
			x{ endPoint.x - startPoint.x },
			y{ endPoint.y - startPoint.y },
			z{ endPoint.z - startPoint.z }
    	{}


		constexpr float& operator[](int index) { assert(index >= 0 && index <= 2); if (index == 0) return x; if ( index == 1) return y; return z; }
		constexpr float operator[](int index) const { assert(index >= 0 && index <= 2); if (index == 0) return x; if ( index == 1) return y; return z; }

        constexpr Vector3f operator-() const{ return { -x, -y, -z}; }
    	constexpr Vector3f operator+() const{ return { x, y, z }; }
    	constexpr Vector3f operator-(const Vector3f& rhs) const { return { x - rhs.x, y - rhs.y, z - rhs.z }; }
    	constexpr Vector3f operator+(const Vector3f& rhs) const { return { x + rhs.x, y + rhs.y, z + rhs.z }; }

        constexpr Vector3f& operator+=(const Vector3f& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
		constexpr Vector3f& operator-=(const Vector3f& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }

		constexpr Vector3f& Scale(float xScale, float yScale, float zScale) { x *= xScale; y *= yScale; z *= zScale; return *this; }
		constexpr Vector3f& Scale(const Vector3f& scale) { return Scale(scale.x, scale.y, scale.z); }
		constexpr static Vector3f Scale(const Vector3f& vecToScale, float xScale, float yScale, float zScale) { return {vecToScale.x * xScale, vecToScale.y * yScale, vecToScale.z * zScale}; }
		constexpr static Vector3f Scale(const Vector3f& vecToScale, const Vector3f& scale) {return Scale(vecToScale, scale.x, scale.y, scale.z); }

		constexpr Vector3f operator*(cFloatArithmetic auto rhs) const { return { x * rhs, y * rhs, z * rhs }; }
		constexpr Vector3f operator/(cFloatArithmetic auto rhs) const
    	{
    		assert((std::abs(static_cast<float>(rhs)) > FLT_EPS)); return { x / rhs, y / rhs, z / rhs };
    	}
		constexpr Vector3f& operator*=(cFloatArithmetic auto rhs) { x = x * rhs; y = y * rhs; z = z * rhs; return *this; }
		constexpr Vector3f& operator/=(cFloatArithmetic auto rhs)
		{
			assert((std::abs(static_cast<float>(rhs)) > FLT_EPS)); x = x / rhs; y = y / rhs; z = z / rhs; return *this;
		}

    	bool operator==(const Vector3f& rhs) const { return (std::abs(x - rhs.x) < FLT_EPS) && (std::abs(y - rhs.y) < FLT_EPS) && (std::abs(z - rhs.z) < FLT_EPS); }
		bool operator!=(const Vector3f& rhs) const { return !(*this == rhs); }

		constexpr static float Dot(const Vector3f& first, const Vector3f& second) { return first.x * second.x + first.y * second.y + first.z * second.z; }
		constexpr static Vector3f Cross(const Vector3f& first, const Vector3f& second)
    	{
    		return
    		{
    			first.y * second.z - first.z * second.y,
    			first.z * second.x - first.x * second.z,
    			first.x * second.y - first.y * second.x
    		};
    	}
		static float AngleBetween(const Vector3f& first, const Vector3f& second, bool inDegrees = true)
    	{
    		const float firstLength = first.Length();
    		if (firstLength < FLT_EPS) return 0.f;

    		const float secondLength = second.Length();
    		if (secondLength < FLT_EPS) return 0.f;

    		const float angle = std::acos(Dot(first, second) / (firstLength * secondLength));
    		if (inDegrees) return angle * 180 / std::numbers::pi_v<float>;
    		return angle;
    	}
		static Vector3f Reflect(const Vector3f& vector, const Vector3f& surfaceNormal)
    	{
    		const auto n = surfaceNormal.Normalized();
    		return vector - (n * 2.f * Dot(vector, n));
    	}

		tstring ToString(uint8_t decimalPrecision = 1) const { return std::format(_T("( {1:.{0}f}, {2:.{0}f}, {3:.{0}f} )"), decimalPrecision, x, y, z); }

		float Length() const { return std::sqrtf(x * x + y * y + z * z); }
		constexpr float SquaredLength() const { return x * x + y * y + z * z; }

    	Vector3f Normalized() const
    	{
    		const auto l = Length();
    		if (l < FLT_EPS) return {};
    		return { x / l, y / l , z / l };
    	}
    	Vector3f& Normalize()
    	{
    		const auto l = Length();
    		if (l < FLT_EPS) return *this;
    		*this /= l;
    		return *this;
    	}

        float x{};
        float y{};
        float z{};

		const static Vector3f UnitX;
		const static Vector3f UnitY;
		const static Vector3f UnitZ;
		const static Vector3f Zero;

	private:
		constexpr static float FLT_EPS = std::numeric_limits<float>::epsilon();
    };
	inline constexpr Vector3f Vector3f::UnitX{ 1, 0, 0 };
	inline constexpr Vector3f Vector3f::UnitY{ 0, 1, 0 };
	inline constexpr Vector3f Vector3f::UnitZ{ 0, 0, 1 };
	inline constexpr Vector3f Vector3f::Zero{ 0, 0, 0 };

	constexpr Vector3f operator*(cFloatArithmetic auto lhs, const Vector3f& rhs) { return rhs * lhs; }
	inline tostream& operator<< (tostream& lhs, const Vector3f& rhs) { lhs << rhs.ToString(); return lhs; }


	struct Matrix3X3
	{
		constexpr Matrix3X3() = default;
		constexpr Matrix3X3(const Vector2f& xAxis, const Vector2f& yAxis, const Vector2f& t):
			data{ {xAxis.x, xAxis.y, 0}, {yAxis.x, yAxis.y, 0}, {t.x, t.y, 1} }
		{}
		constexpr Matrix3X3(const Vector3f& xAxis, const Vector3f& yAxis, const Vector3f& t):
			data{ xAxis, yAxis, t}
		{}

		Vector3f& operator[](int index) { assert(index >= 0 && index <= 2); return data[index]; }
		Vector3f operator[](int index) const { assert(index >= 0 && index <= 2); return data[index]; };

		Vector2f GetXAxis() const { return {data[0].x, data[0].y};}
		Vector2f GetYAxis() const { return {data[1].x, data[1].y};}
		Vector2f GetTranslation() const { return {data[2].x, data[2].y};}

		static constexpr Matrix3X3 Translation(float x, float y) { return Translation({x,y}); }
		static constexpr Matrix3X3 Translation(const Vector2f& translation) {return {Vector2f::UnitX, Vector2f::UnitY, translation}; }
		static Matrix3X3 Rotation(float angle)
		{
			const float c = std::cos(angle);
			const float s = std::sin(angle);
			return {{c ,s}, {-s, c}, Vector2f::Zero};
		}
		static constexpr Matrix3X3 Scale(float xScale, float yScale) { return {Vector2f::UnitX * xScale, Vector2f::UnitY * yScale, Vector2f::Zero}; }
		static constexpr Matrix3X3 Scale(const Vector2f& scale) { return Scale(scale.x, scale.y); }

		static constexpr Matrix3X3 Transpose(const Matrix3X3& m) { return m.CalculateTranspose(); }

		constexpr Vector2f TransformVector(float x, float y) const { return { data[0].x * x + data[1].x * y, data[0].y * x + data[1].y * y, }; }
		constexpr Vector2f TransformVector(const Vector2f& v) const { return TransformVector(v.x, v.y); }
		constexpr Vector2f TransformPoint(float x, float y) const { return { data[0].x * x + data[1].x * y + data[2].x, data[0].y * x + data[1].y * y + data[2].y }; }
		constexpr Vector2f TransformPoint(const Vector2f& v) const { return TransformPoint(v.x, v.y); }

		constexpr Matrix3X3 Transpose() { return *this = CalculateTranspose(); }

		constexpr Matrix3X3& operator*=(cFloatArithmetic auto s)
		{
			for (auto& row : data) row *= s;
			return *this;
		}
		constexpr Matrix3X3 operator*(cFloatArithmetic auto s) const
		{
			Matrix3X3 result{*this};
			for (auto& row : result.data) row *= s;
			return result;
		}
		constexpr Matrix3X3& operator-=(const Matrix3X3& m)
		{
			for (auto row = 0; row < ROWS; ++row) data[row] -= m[row];
			return *this;
		}
		constexpr Matrix3X3 operator-(const Matrix3X3& m) const
		{
			Matrix3X3 result{*this};
			for (auto row = 0; row < ROWS; ++row) result[row] -= m[row];
			return result;
		}
		constexpr Matrix3X3& operator+=(const Matrix3X3& m)
		{
			for (auto row = 0; row < ROWS; ++row) data[row] += m[row];
			return *this;
		}
		constexpr Matrix3X3 operator+(const Matrix3X3& m) const
		{
			Matrix3X3 result{*this};
			for (auto row = 0; row < ROWS; ++row) result[row] += m[row];
			return result;
		}
		constexpr Matrix3X3& operator*=(const Matrix3X3& m)
		{
			Matrix3X3 copy{*this};
			Matrix3X3 transpose{m.CalculateTranspose()};
			for (auto row = 0; row < ROWS; ++row)
				for (auto col = 0; col < COLUMS; ++col)
					data[row][col] = Vector3f::Dot(copy[row], transpose[col]);

			return *this;
		}
		constexpr Matrix3X3 operator*(const Matrix3X3& m) const
		{
			Matrix3X3 result{};
			Matrix3X3 transpose{m.CalculateTranspose()};
			for (auto row = 0; row < ROWS; ++row)
				for (auto col = 0; col < COLUMS; ++col)
					result[row][col] = Vector3f::Dot(data[row], transpose[col]);

			return result;
		}

		static constexpr uint8_t ROWS = 3;
		static constexpr uint8_t COLUMS = 3;

	private:

		constexpr Matrix3X3 CalculateTranspose() const
		{
			Matrix3X3 result{};

			for (auto row = 0; row < ROWS; ++row)
				for (auto col = 0; col < COLUMS; ++col)
					result[row][col] = data[col][row];

			return result;
		}

		Vector3f data[3]
		{
			{1,0,0},
			{0,1,0},
			{0,0,1}
		};
	};
	constexpr Matrix3X3 operator*(cFloatArithmetic auto lhs, const Matrix3X3& rhs) { return rhs * lhs; }


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
	struct RoundedRectf : public Rectf
	{
		constexpr RoundedRectf() = default;
		constexpr RoundedRectf(float left, float bottom, float width, float height, float radiusX, float radiusY) :
			Rectf{left, bottom, width, height},
			xRadius{radiusX},
			yRadius{radiusY}
		{}
		constexpr RoundedRectf(const Point2f& bottomLeft, float width, float height, float radiusX, float radiusY) :
			Rectf{bottomLeft, width, height},
			xRadius{radiusX},
			yRadius{radiusY}
		{}
		constexpr RoundedRectf(const Point2f& bottomLeft, const Point2f& topRight, float radiusX, float radiusY) :
			Rectf{bottomLeft, topRight},
			xRadius{radiusX},
			yRadius{radiusY}
		{}
		constexpr RoundedRectf(float left, float bottom, float width, float height, const Vector2f& radius) :
			RoundedRectf{left, bottom, width, height, radius.x, radius.y}
		{}
		constexpr RoundedRectf(const Point2f& bottomLeft, float width, float height, const Vector2f& radius) :
			RoundedRectf{bottomLeft, width, height, radius.x, radius.y}
		{}
		constexpr RoundedRectf(const Point2f& bottomLeft, const Point2f& topRight, const Vector2f& radius) :
			RoundedRectf{bottomLeft, topRight, radius.x, radius.y}
		{}

		float xRadius{};
		float yRadius{};
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
	struct RoundedRectf : public Rectf
	{
		constexpr RoundedRectf() = default;
		constexpr RoundedRectf(float left, float top, float width, float height, float radiusX, float radiusY) :
			Rectf{left, top, width, height},
			xRadius{radiusX},
			yRadius{radiusY}
		{}
		constexpr RoundedRectf(const Point2f& topLeft, float width, float height, float radiusX, float radiusY) :
			Rectf{topLeft, width, height},
			xRadius{radiusX},
			yRadius{radiusY}
		{}
		constexpr RoundedRectf(const Point2f& topLeft, const Point2f& bottomRight, float radiusX, float radiusY) :
			Rectf{topLeft, bottomRight},
			xRadius{radiusX},
			yRadius{radiusY}
		{}
		constexpr RoundedRectf(float left, float top, float width, float height, const Vector2f& radius) :
			RoundedRectf{left, top, width, height, radius.x, radius.y}
		{}
		constexpr RoundedRectf(const Point2f& topLeft, float width, float height, const Vector2f& radius) :
			RoundedRectf{topLeft, width, height, radius.x, radius.y}
		{}
		constexpr RoundedRectf(const Point2f& topLeft, const Point2f& bottomRight, const Vector2f& radius) :
			RoundedRectf{topLeft, bottomRight, radius.x, radius.y}
		{}

		float xRadius{};
		float yRadius{};
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

