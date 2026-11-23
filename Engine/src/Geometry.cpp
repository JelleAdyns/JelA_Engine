// ReSharper disable CppDFAUnusedValue
// ReSharper disable CppDFAUnreachableCode
// ReSharper disable CppDFAConstantConditions
#include "JelA/Geometry.h"
#include "JelA/Engine.h"
#include <algorithm>
#include <execution>
#include <numbers>

#include "JelA/Utils.h"

namespace jela
{
	//--------------------------------------------------------------------------------------------------------------------
	// Geometry
	Geometry::Geometry(const Geometry& other):
		m_Translation{other.m_Translation}
	{
		HResultHandler hr = Recreate();
		if (hr.Failed())
		{
			SafeRelease(&m_pGeo);
			return;
		}

		assert((m_pGeo));
		assert((other.m_pGeo));
		ID2D1GeometrySink* pSink{};
		hr = m_pGeo->Open(&pSink);
		if (hr.Succeeded()) hr = other.m_pGeo->Stream(pSink);
		if (hr.Succeeded()) hr = pSink->Close();

		SafeRelease(&pSink);
	}
	Geometry::Geometry(Geometry&& other) noexcept:
		m_Translation{std::exchange(other.m_Translation, {})},
		m_pGeo{std::exchange(other.m_pGeo, nullptr)}
	{
	}
	Geometry& Geometry::operator=(const Geometry& other)
	{
		Geometry{other}.swap(*this);
		return *this;
	}
	Geometry& Geometry::operator=(Geometry&& other) noexcept
	{
		Geometry{std::move(other)}.swap(*this);
		return *this;
	}
	void Geometry::Move(float x, float y){ Move({ x,y }); }
	void Geometry::Move(const Vector2f& translation) { m_Translation += translation; }
	void Geometry::ResetPosition() { m_Translation = {}; }

	const Vector2f& Geometry::GetTranslation() const { return m_Translation; }

	ID2D1PathGeometry* Geometry::RendererDoor::GetGeometry(const Geometry* geo){ return geo->GetGeometry(); }

	Geometry::~Geometry() { SafeRelease(&m_pGeo); }
	ID2D1PathGeometry* Geometry::GetGeometry() const { return m_pGeo; }

	HRESULT Geometry::Recreate(bool releasePrevious)
	{
		if (releasePrevious) SafeRelease(&m_pGeo);
		return ENGINE.RenderMngr()->Get2DFactory().get()->CreatePathGeometry(&m_pGeo);
	}
	void Geometry::swap(Geometry& other) noexcept
	{
		std::swap(m_Translation, other.m_Translation);
		std::swap(m_pGeo, other.m_pGeo);
	}
	//--------------------------------------------------------------------------------------------------------------------



	//--------------------------------------------------------------------------------------------------------------------
	// Polygon
	PolygonGeometry::PolygonGeometry(const std::vector<Point2f>& points, bool closeSegment) :
		Geometry{}
	{
		AdjustPoints([&](Point2f& point) { point += GetTranslation(); });
		ReplacePolygon(points, closeSegment);
		m_NeedsToChange = true;
		UpdateGeometry();
	}

	void PolygonGeometry::ReplacePoints(const std::vector<Point2f>& points)
	{
		if (m_Points.size() != points.size()) m_NeedsToChange = true;
		else
		{
			if (const auto ogPoints = GetOriginalPoints();
				std::ranges::mismatch(ogPoints, points).in1 != ogPoints.cend())
				m_NeedsToChange = true;
		}
		m_Points = points;
	}
	void PolygonGeometry::ReplacePolygon(const std::vector<Point2f>& points, bool closeSegment)
	{
		ReplacePoints(points);
		SetCloseSegment(closeSegment);
	}
	void PolygonGeometry::SetCloseSegment(bool closeSegment)
	{
		if (m_SegmentClosed != closeSegment) m_NeedsToChange = true;
		m_SegmentClosed = closeSegment;
	}
	void PolygonGeometry::UpdateGeometry()
	{
		if (!m_NeedsToChange) return;
		if (!Recreate())
		{
			OutputDebugString(std::format(_T("Creation of Polygon failed!\n")).c_str());
		}
	}
	bool PolygonGeometry::Recreate()
	{
		OutputDebugString(_T("Recreating the Polygon...\n"));
		if (m_Points.empty()) return false;

		HRESULT hr = S_OK;
		hr = Geometry::Recreate();

		if (!SUCCEEDED(hr)) return false;

		const auto pGeo = GetGeometry();
		if (!pGeo) return false;

		ID2D1GeometrySink* pSink{};
		hr = pGeo->Open(&pSink);

		if (SUCCEEDED(hr))
		{
			std::vector<D2D1_POINT_2F> D2points(m_Points.size());

			for (size_t i = 0; i < m_Points.size(); i++)
			{
				auto pointY = m_Points[i].y;
				if constexpr (USE_MATHEMATICAL_COORDINATESYSTEM) pointY = -pointY;
				D2points[i] = D2D1::Point2F(m_Points[i].x, pointY);
			}

			pSink->BeginFigure(D2points.front(), D2D1_FIGURE_BEGIN_FILLED);
			pSink->AddLines(D2points.data(), static_cast<UINT32>(D2points.size()));
			pSink->EndFigure(m_SegmentClosed ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);

			hr = pSink->Close();
		}
		SafeRelease(&pSink);

		m_NeedsToChange = false;
		return hr == S_OK;
	}

	void PolygonGeometry::ResetPosition()
	{
		AdjustPoints([&](Point2f& point) { point -= GetTranslation(); });
		Geometry::ResetPosition();
	}
	void PolygonGeometry::Move(const Vector2f& translation)
	{
		Geometry::Move(translation);
		AdjustPoints([&](Point2f& point) { point += translation; });
	}

	std::vector<Point2f> PolygonGeometry::GetOriginalPoints() const
	{
		std::vector<Point2f>transformedPoints = m_Points;
		std::for_each(std::execution::par, transformedPoints.begin(), transformedPoints.end(),
			[&](Point2f& point) { point -= GetTranslation(); });
		return transformedPoints;
	}
	const std::vector<Point2f>& PolygonGeometry::GetPoints() const { return m_Points; }

	bool PolygonGeometry::IsPointInside(const Point2f& point) const
	{
		if (m_Points.size() < 2) return false;

		// 1. First do a simple test with axis aligned bounding box around the polygon
		float xMin{ m_Points[0].x };
		float xMax{ m_Points[0].x };
		float yMin{ m_Points[0].y };
		float yMax{ m_Points[0].y };
		for (size_t idx{ 1 }; idx < m_Points.size(); ++idx)
		{
			if (xMin > m_Points[idx].x) xMin = m_Points[idx].x;

			if (xMax < m_Points[idx].x) xMax = m_Points[idx].x;

			if (yMin > m_Points[idx].y) yMin = m_Points[idx].y;

			if (yMax < m_Points[idx].y) yMax = m_Points[idx].y;
		}

		if (point.x < xMin || point.x > xMax || point.y < yMin || point.y > yMax) return false;

        // 2. Draw a virtual ray from anywhere outside the polygon to the point
        //    and count how often it hits any side of the polygon.
		//    If the number of hits is even, it's outside of the polygon, if it's odd, it's inside.
		int numberOfIntersectionPoints{ 0 };
		const Point2f p2{ xMax + 10, yMax + 20 }; // random point outside the box

		// Count the number of intersection points
		float lambda1{}, lambda2{};
		for (size_t i{ 0 }; i < m_Points.size(); ++i)
		{
			if (utils::IntersectLineSegments(m_Points[i], m_Points[(i + 1) % m_Points.size()], point, p2, lambda1, lambda2))
			{
				++numberOfIntersectionPoints;
			}
		}
		return numberOfIntersectionPoints % 2 != 0;

	}
	bool PolygonGeometry::IsSegmentClosed() const { return m_SegmentClosed; }

	void PolygonGeometry::AdjustPoints(const std::function<void(Point2f&)>& func)
	{
		std::for_each(std::execution::par, m_Points.begin(), m_Points.end(), func);
	}
	//--------------------------------------------------------------------------------------------------------------------


	//--------------------------------------------------------------------------------------------------------------------
	// Arc
	ArcGeometry::ArcGeometry(float centerX, float centerY, float radiusX, float radiusY, float startAngle, float angle, bool closeSegment) :
		Geometry{},
		m_Radius{radiusX, radiusY},
		m_SegmentClosed{closeSegment}
	{
		SetPosition(centerX,centerY);
		SetStartAngle(startAngle);
		SetAngle(angle);

		m_NeedsToChange = true;
		UpdateGeometry();
	}
	ArcGeometry::ArcGeometry(const Point2f& center, float radiusX, float radiusY, float startAngle, float angle, bool closeSegment) :
		ArcGeometry{ center.x, center.y, radiusX, radiusY, startAngle, angle, closeSegment }
	{}
	ArcGeometry::ArcGeometry(const Point2f& point1, const Point2f& point2, bool clockwise, bool closeSegment) :
		Geometry{},
		m_SegmentClosed{closeSegment}
	{
		SetFromPoints(point1, point2, clockwise);

		m_NeedsToChange = true;
		UpdateGeometry();
	}

	void ArcGeometry::SetRadius(float radiusX, float radiusY)
	{
		assert(radiusX >= 0 && radiusY >= 0);

		if (m_Radius.x != radiusX || m_Radius.y != radiusY)
			m_NeedsToChange = true;

		m_Radius.x = radiusX;
		m_Radius.y = radiusY;
	}
	void ArcGeometry::SetStartAngle(float startAngle)
	{
		while (startAngle >= 360.f) startAngle -= 360;
		while (startAngle <= -360.f) startAngle += 360;

		if (m_StartAngle != startAngle) m_NeedsToChange = true;

		m_StartAngle = startAngle;
	}
	void ArcGeometry::SetAngle(float angle)
	{
		if (angle >= 360.f)
		{
			angle = 359.9f;
			OutputDebugString(_T("Angle is larger or equal to 360. Use Ellipse instead.\n"));
		}
		if (angle <= -360.f)
		{
			angle = -359.9f;
			OutputDebugString(_T("Angle is smaller or equal to -360. Use Ellipse instead.\n"));
		}

		if (m_Angle != angle) m_NeedsToChange = true;

		m_Angle = angle;
	}
	void ArcGeometry::SetCloseSegment(bool closeSegment)
	{
		if (m_SegmentClosed != closeSegment) m_NeedsToChange = true;
		m_SegmentClosed = closeSegment;
	}
	void ArcGeometry::SetFromPoints(const Point2f& point1, const Point2f& point2, bool clockwise)
	{
		if constexpr (USE_MATHEMATICAL_COORDINATESYSTEM == false) clockwise = !clockwise;

		auto rad = point1 - point2;
		rad.x = std::abs(rad.x);
		rad.y = std::abs(rad.y);
		SetRadius(rad.x, rad.y);

		SetAngle(clockwise ? -90.f : 90.f);

		float startAngle{};
		const auto& swapCenterPoint = [&](bool xComparison)
		{																		// CP1  _____P2
			if (xComparison == clockwise) SetPosition(point1.x, point2.y);		//	   /
			else						  SetPosition(point2.x, point1.y);		//	  /
																				//   |
			if (xComparison) startAngle += 90;									//	 |
			if (!clockwise) startAngle += 90;									// P1		 CP2
		};

		if (point1.y >= point2.y)
		{
			startAngle = 0;
			swapCenterPoint(point1.x <= point2.x);
		}
		else
		{
			startAngle = 180;
			swapCenterPoint(point1.x > point2.x);
		}

		SetStartAngle(startAngle);
	}
	void ArcGeometry::UpdateGeometry()
	{
		if (!m_NeedsToChange) return;

		if (!Recreate())
		{
			OutputDebugString(std::format(_T("Creation of Arc failed! Tried to create an arc with:\n"
									"offset: {}\nradius: {}\nstart angle: {}\nangle: {}\nclose segments: {}\n"),
				GetTranslation().ToTString(), m_Radius.ToTString(),m_StartAngle,m_Angle, m_SegmentClosed).c_str());
		}
	}
	bool ArcGeometry::Recreate()
	{
		OutputDebugString(_T("Recreating the Arc...\n"));
		HRESULT hr = Geometry::Recreate();

		if (!SUCCEEDED(hr)) return false;

		const auto pGeo = GetGeometry();
		if (!pGeo) return false;

		ID2D1GeometrySink* pSink;
		hr = pGeo->Open(&pSink);

		if (SUCCEEDED(hr))
		{
			const auto startRad = (m_StartAngle + std::min(m_Angle, 0.f)) * std::numbers::pi_v<float> / 180;
			const auto endRad = (m_StartAngle + std::max(m_Angle, 0.f)) * std::numbers::pi_v<float> / 180;

			const auto startX = m_Radius.x * std::cosf(startRad);
			auto startY = m_Radius.y * std::sinf(startRad);
			if constexpr (USE_MATHEMATICAL_COORDINATESYSTEM) startY = -startY;

			const auto endX = m_Radius.x * std::cosf(endRad);
			auto endY = m_Radius.y * std::sinf(endRad);
			if constexpr (USE_MATHEMATICAL_COORDINATESYSTEM) endY = -endY;

			const D2D1_ARC_SEGMENT arcSegment
			{
				.point = D2D1::Point2F(endX, endY),
				.size = D2D1::SizeF(m_Radius.x, m_Radius.y),
				.rotationAngle = 0,
				.sweepDirection = USE_MATHEMATICAL_COORDINATESYSTEM ? D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE : D2D1_SWEEP_DIRECTION_CLOCKWISE,
				.arcSize = std::abs(m_Angle) < 180 ? D2D1_ARC_SIZE_SMALL : D2D1_ARC_SIZE_LARGE
			};

			pSink->BeginFigure(D2D1::Point2F(startX, startY), D2D1_FIGURE_BEGIN_FILLED);
			pSink->AddArc(arcSegment);

			if (m_SegmentClosed) pSink->AddLine(D2D1::Point2F());

			pSink->EndFigure(m_SegmentClosed ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);
			hr = pSink->Close();
		}

		SafeRelease(&pSink);

		m_NeedsToChange = false;
		return hr == S_OK;
	}

	void ArcGeometry::SetPosition(float centerX, float centerY)
	{
		ResetPosition();
		Move(centerX, centerY);
	}

	//--------------------------------------------------------------------------------------------------------------------

}
