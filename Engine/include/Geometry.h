#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Structs.h"
#include <functional>

namespace jela
{
	class Geometry
	{
	public:
		Geometry(const Geometry& other);
		Geometry(Geometry&& other) noexcept;
		Geometry& operator=(const Geometry& other);
		Geometry& operator=(Geometry&& other) noexcept;

		virtual void Move(float x, float y) { Move({ x,y }); }
		virtual void Move(const Vector2f& translation) { m_Translation += translation; }

		const Vector2f& GetTranslation() const { return m_Translation; };
		ID2D1PathGeometry* GetGeometry() const { return m_pGeo; };

	protected:
		Geometry() = default;
		virtual ~Geometry() { SafeRelease(&m_pGeo); };
		virtual void ResetPosition() { m_Translation = {}; }

		HRESULT Recreate(bool releasePrevious = true);

	private:
		void swap(Geometry& other) noexcept;
		Vector2f m_Translation{};
		ID2D1PathGeometry* m_pGeo{};
	};

	class Polygon final : public Geometry
	{
	public:
		Polygon(const std::vector<Point2f>& points, bool closeSegment = true);

		Polygon(const Polygon& other) = default;
		Polygon(Polygon&& other) noexcept = default;
		Polygon& operator=(const Polygon& other) = default;
		Polygon& operator=(Polygon&& other) noexcept = default;
		~Polygon() override = default;

		bool Recreate(const std::vector<Point2f>& points, bool closeSegment = true);

		void ResetPosition() override;
		void Move(float x, float y) override { Move({ x,y }); }
		void Move(const Vector2f& translation) override;

		std::vector<Point2f> GetOriginalPoints() const;
		const std::vector<Point2f>& GetPoints() const{ return m_Points; };
		bool IsPointInside(const Point2f& point) const;
	private:
		void AdjustPoints(const std::function<void(Point2f&)>& func);

		bool m_SegmentClosed{};
		std::vector<Point2f> m_Points{};
	};

	class Arc final : public Geometry
	{
	public:
		Arc(float centerX, float centerY, float radiusX, float radiusY, float startAngle, float angle, bool closeSegment);
		Arc(const Point2f& center, float radiusX, float radiusY, float startAngle, float angle, bool closeSegment);
		Arc(const Point2f& point1, const Point2f& point2, bool clockwise, bool closeSegment);

		Arc(const Arc& other) = default;
		Arc(Arc&& other) noexcept = default;
		Arc& operator=(const Arc& other) = default;
		Arc& operator=(Arc&& other) noexcept = default;
		~Arc() override = default;

		bool Recreate(float radiusX, float radiusY, float startAngle, float angle, bool closeSegment);
		bool RecreateByRadius(float radiusX, float radiusY, bool closeSegment);
		bool RecreateByAngle(float startAngle, float angle, bool closeSegment);
		bool Recreate(const Point2f& point1, const Point2f& point2, bool clockwise, bool closeSegment);

        void SetPosition(const Point2f& center) { SetPosition(center.x, center.y); }

        void SetPosition(float centerX, float centerY);

        // The center (position) of an Arc object is equal to the translation.
        Point2f GetPosition() const { return Point2f{GetTranslation().x, GetTranslation().y}; }
        const Vector2f& GetRadius() const { return m_Radius; }
        float GetStartAngle() const { return m_StartAngle; }
        float GetAngle() const { return m_Angle; }
	private:
		Vector2f m_Radius{};
		float m_StartAngle{};
		float m_Angle{};
	};
}

#endif // !GEOMETRY_H
