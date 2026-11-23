#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Structs.h"
#include <functional>

namespace jela
{
	class RenderManager;

	class Geometry
	{
	public:
		Geometry(const Geometry& other);
		Geometry(Geometry&& other) noexcept;
		Geometry& operator=(const Geometry& other);
		Geometry& operator=(Geometry&& other) noexcept;

		virtual void Move(float x, float y);
		virtual void Move(const Vector2f& translation);
		virtual void ResetPosition();

		const Vector2f& GetTranslation() const;;

		class RendererDoor
		{
			friend RenderManager;
			static ID2D1PathGeometry* GetGeometry(const Geometry* geo);
		};

	protected:
		Geometry() = default;
		virtual ~Geometry();;
		ID2D1PathGeometry* GetGeometry() const;;

		HRESULT Recreate(bool releasePrevious = true);
	private:
		void swap(Geometry& other) noexcept;
		ID2D1PathGeometry* m_pGeo{};
		Vector2f m_Translation{};
	};

	class PolygonGeometry final : public Geometry
	{
	public:
		PolygonGeometry(const std::vector<Point2f>& points, bool closeSegment = true);

		PolygonGeometry(const PolygonGeometry& other) = default;
		PolygonGeometry(PolygonGeometry&& other) noexcept = default;
		PolygonGeometry& operator=(const PolygonGeometry& other) = default;
		PolygonGeometry& operator=(PolygonGeometry&& other) noexcept = default;
		~PolygonGeometry() override = default;

		void ReplacePoints(const std::vector<Point2f>& points);
		void ReplacePolygon(const std::vector<Point2f>& points, bool closeSegment = true);
		void SetCloseSegment(bool closeSegment);
		void UpdateGeometry();


		void ResetPosition() override;
		void Move(float x, float y) override { Move({ x,y }); }
		void Move(const Vector2f& translation) override;

		std::vector<Point2f> GetOriginalPoints() const;
		const std::vector<Point2f>& GetPoints() const;;
		bool IsPointInside(const Point2f& point) const;
		bool IsSegmentClosed() const;
	private:
		bool Recreate();
		void AdjustPoints(const std::function<void(Point2f&)>& func);

		bool m_NeedsToChange{};
		bool m_SegmentClosed{};
		std::vector<Point2f> m_Points{};
	};

	class ArcGeometry final : public Geometry
	{
	public:
		ArcGeometry(float centerX, float centerY, float radiusX, float radiusY, float startAngle, float angle, bool closeSegment);
		ArcGeometry(const Point2f& center, float radiusX, float radiusY, float startAngle, float angle, bool closeSegment);
		ArcGeometry(const Point2f& point1, const Point2f& point2, bool clockwise, bool closeSegment);

		ArcGeometry(const ArcGeometry& other) = default;
		ArcGeometry(ArcGeometry&& other) noexcept = default;
		ArcGeometry& operator=(const ArcGeometry& other) = default;
		ArcGeometry& operator=(ArcGeometry&& other) noexcept = default;
		~ArcGeometry() override = default;

		void SetRadius(float radiusX, float radiusY);
		void SetStartAngle(float startAngle);
		void SetAngle(float angle);
		void SetCloseSegment(bool closeSegment);
		void SetFromPoints(const Point2f& point1, const Point2f& point2, bool clockwise);
		void UpdateGeometry();

        void SetPosition(const Point2f& center) { SetPosition(center.x, center.y); }

        void SetPosition(float centerX, float centerY);

        // The center (position) of an Arc object is equal to the translation.
        Point2f GetPosition() const { return Point2f{GetTranslation().x, GetTranslation().y}; }
        const Vector2f& GetRadius() const { return m_Radius; }
        float GetStartAngle() const { return static_cast<float>(m_StartAngle); }
        float GetAngle() const { return static_cast<float>(m_Angle); }
	private:
		bool Recreate();
		Vector2f m_Radius{};
		float m_StartAngle{};
		float m_Angle{};

		bool m_SegmentClosed{};
		bool m_NeedsToChange{};
	};
}

#endif // !GEOMETRY_H
