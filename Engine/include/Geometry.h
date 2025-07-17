#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Structs.h"
#include <functional>

namespace jela
{
	class Geometry
	{
	public:

		virtual void Move(float x, float y) { Move({ x,y }); }
		virtual void Move(const Vector2f& translation) { m_Translation += translation; }

		const Vector2f& GetTranslation() const { return m_Translation; };
		ID2D1PathGeometry* const GetGeometry() const { return m_pGeo; };

	protected:
		Geometry() = default;
		virtual ~Geometry() { SafeRelease(&m_pGeo); };
		virtual void ResetPosition() { m_Translation = {}; }

		HRESULT Recreate();

	private:
		Vector2f m_Translation{};
		ID2D1PathGeometry* m_pGeo{};
	};

	class Polygon final : public Geometry
	{
	public:
		Polygon(const std::vector<Point2Int>& points, bool closeSegment = true);
		virtual ~Polygon() = default;

		bool Recreate(const std::vector<Point2Int>& points, bool closeSegment = true);

		virtual void ResetPosition() override;
		virtual void Move(float x, float y) override { Move({ x,y }); }
		virtual void Move(const Vector2f& translation) override;

		std::vector<Point2Int> GetOriginalPoints() const;
		const std::vector<Point2Int>& GetPoints() const{ return m_Points; };
		bool IsPointInside(const Point2Int& point) const;
	private:
		void AdjustPoints(const std::function<void(Point2Int&)>& func);

		std::vector<Point2Int> m_Points{};
	};

	class Arc final : public Geometry
	{
	public:
		Arc(int centerX, int centerY, float radiusX, float radiusY, float startAngle, float angle, bool closeSegment);
		Arc(const Point2Int& center, float radiusX, float radiusY, float startAngle, float angle, bool closeSegment);
		Arc(const Point2Int& point1, const Point2Int& point2, bool clockwise, bool closeSegment);

		virtual ~Arc() = default;

		bool Recreate(float radiusX, float radiusY, float startAngle, float angle, bool closeSegment);
		bool RecreateByRadius(float radiusX, float radiusY, bool closeSegment);
		bool RecreateByAngle(float startAngle, float angle, bool closeSegment);
		bool Recreate(const Point2Int& point1, const Point2Int& point2, bool clockwise, bool closeSegment);
		
		void SetPosition(const Point2Int& center) { SetPosition(center.x, center.y); }
		void SetPosition(int centerX, int centerY);
		
		// The center (position) of an Arc object is equal to the translation.
		Point2Int GetPosition() const { return Point2Int{ static_cast<int>(GetTranslation().x),static_cast<int>(GetTranslation().y) }; }
		const Vector2f& GetRadius() const { return m_Radius; }
		float GetStartAngle() const { return m_StartAngle; }
		float GetAngle() const { return m_Angle; }
	private:
		Vector2f m_Radius;
		float m_StartAngle;
		float m_Angle;
	};
}

#endif // !GEOMETRY_H
