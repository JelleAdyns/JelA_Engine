#include "Geometry.h"
#include "Engine.h"
#include <algorithm>
#include <execution>
#include <numbers>

namespace jela
{
	//--------------------------------------------------------------------------------------------------------------------
	// Geometry
	HRESULT Geometry::Recreate()
	{
		SafeRelease(&m_pGeo);
		return ENGINE.GetFactory()->CreatePathGeometry(&m_pGeo);
	}
	//--------------------------------------------------------------------------------------------------------------------



	//--------------------------------------------------------------------------------------------------------------------
	// Polygon
	Polygon::Polygon(const std::vector<Point2Int>& points, bool closeSegment) :
		Geometry{},
		m_Points{points}
	{
		AdjustPoints([&](Point2Int& point) { point += GetTranslation(); });
		Recreate(points, closeSegment);
	}
	bool Polygon::Recreate(const std::vector<Point2Int>& points, bool closeSegment)
	{
		HRESULT hr = Geometry::Recreate();

		ID2D1GeometrySink* pSink{};

		if (SUCCEEDED(hr))
		{
			hr = GetGeometry()->Open(&pSink);
		}
		if (SUCCEEDED(hr))
		{

			if (!m_Points.empty())
			{
				std::vector<D2D1_POINT_2F> D2points(points.size());

				for (size_t i = 0; i < points.size(); i++)
				{
#ifdef MATHEMATICAL_COORDINATESYSTEM

					D2points[i] = D2D1::Point2F(static_cast<FLOAT>(points[i].x), static_cast<FLOAT>(ENGINE.GetWindowRect().height - points[i].y));
#else
					D2points[i] = D2D1::Point2F(static_cast<FLOAT>(points[i].x), static_cast<FLOAT>(points[i].y));

#endif // MATHEMATICAL_COORDINATESYSTEM
				}

				pSink->BeginFigure(D2points.front(), D2D1_FIGURE_BEGIN_FILLED);
				pSink->AddLines(D2points.data(), (UINT32)D2points.size());
				pSink->EndFigure(closeSegment ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);
			}

			hr = pSink->Close();
		}

		SafeRelease(&pSink);

		return hr == S_OK;
	}

	void Polygon::ResetPosition()
	{
		AdjustPoints([&](Point2Int& point) { point -= GetTranslation(); });
		Geometry::ResetPosition();
	}
	void Polygon::Move(const Vector2f& translation)
	{
		Geometry::Move(translation);
		AdjustPoints([&](Point2Int& point) { point += translation; });
	}

	std::vector<Point2Int> Polygon::GetOriginalPoints() const
	{
		std::vector<Point2Int>transformedPoints = m_Points;
		std::for_each(std::execution::par, transformedPoints.begin(), transformedPoints.end(),
			[&](Point2Int& point) { point -= GetTranslation(); });
		return transformedPoints;
	}

	bool Polygon::IsPointInside(const Point2Int& point) const
	{
		if (m_Points.size() < 2) return false;

		// 1. First do a simple test with axis aligned bounding box around the polygon
		int xMin{ m_Points[0].x };
		int xMax{ m_Points[0].x };
		int yMin{ m_Points[0].y };
		int yMax{ m_Points[0].y };
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
		Point2Int p2{ xMax + 10, yMax + 20 }; // random point outside the box

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

	void Polygon::AdjustPoints(const std::function<void(Point2Int&)>& func)
	{
		std::for_each(std::execution::par, m_Points.begin(), m_Points.end(), func);
	}
	//--------------------------------------------------------------------------------------------------------------------
	

	//--------------------------------------------------------------------------------------------------------------------
	// Arc
	Arc::Arc(int centerX, int centerY, float radiusX, float radiusY, float startAngle, float angle, bool closeSegment) :
		Geometry{},
		m_Radius{radiusX,radiusY},
		m_StartAngle{startAngle},
		m_Angle{angle}
	{
		SetPosition(centerX,centerY);
		Recreate(radiusX, radiusY, startAngle, angle, closeSegment);
	}
	Arc::Arc(const Point2Int& center, float radiusX, float radiusY, float startAngle, float angle, bool closeSegment) :
		Arc{ center.x, center.y, radiusX, radiusY, startAngle, angle, closeSegment }
	{}
	Arc::Arc(const Point2Int& point1, const Point2Int& point2, bool clockwise, bool closeSegment) :
		Geometry{}
	{
		Recreate(point1, point2, clockwise, closeSegment);
	}

	bool Arc::Recreate(float radiusX, float radiusY, float startAngle, float angle, bool closeSegment)
	{
		assert(radiusX >= 0 && radiusY >= 0);

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
		while (startAngle >= 360.f) startAngle -= 360;
		while (startAngle <= -360.f) startAngle += 360;

		Geometry::Recreate();

		ID2D1GeometrySink* pSink;

		HRESULT hr = GetGeometry()->Open(&pSink);

		if (SUCCEEDED(hr))
		{
			auto startRad = (startAngle + (angle < 0.f ? angle : 0)) * std::numbers::pi_v<float> / 180;
			auto endRad = (startAngle + (angle > 0.f ? angle : 0)) * std::numbers::pi_v<float> / 180;

#ifdef MATHEMATICAL_COORDINATESYSTEM

			auto beginPoint = D2D1::Point2F(radiusX * std::cosf(startRad), ENGINE.GetWindowRect().height - (radiusY * std::sinf(startRad)));
			auto endPoint = D2D1::Point2F(radiusX * std::cosf(endRad), ENGINE.GetWindowRect().height - (radiusY * std::sinf(endRad)));
#else
			auto beginPoint = D2D1::Point2F(radiusX * std::cosf(startRad), - radiusY * std::sinf(startRad));
			auto endPoint = D2D1::Point2F(radiusX * std::cosf(endRad), - radiusY * std::sinf(endRad));

#endif // MATHEMATICAL_COORDINATESYSTEM

			D2D1_ARC_SEGMENT arcSegment{
				endPoint,
				D2D1::SizeF(radiusX, radiusY),
				0,
				D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
				std::abs(angle) < 180.f ? D2D1_ARC_SIZE_SMALL : D2D1_ARC_SIZE_LARGE
			};

			pSink->BeginFigure(beginPoint, D2D1_FIGURE_BEGIN_FILLED);
			pSink->AddArc(arcSegment);

#ifdef MATHEMATICAL_COORDINATESYSTEM

			if (closeSegment) pSink->AddLine(D2D1::Point2F(0, static_cast<FLOAT>(ENGINE.GetWindowRect().height)));
#else
			if (closeSegment) pSink->AddLine(D2D1::Point2F(0, 0));

#endif // MATHEMATICAL_COORDINATESYSTEM

			pSink->EndFigure(closeSegment ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);
			hr = pSink->Close();
		}

		SafeRelease(&pSink);

		return hr == S_OK;
	}
	bool Arc::RecreateByRadius(float radiusX, float radiusY, bool closeSegment)
	{
		return Recreate(radiusX, radiusY, m_StartAngle, m_Angle, closeSegment);
	}
	bool Arc::RecreateByAngle(float startAngle, float angle, bool closeSegment)
	{
		return Recreate(m_Radius.x, m_Radius.y, startAngle, angle, closeSegment);
	}
	bool Arc::Recreate(const Point2Int& point1, const Point2Int& point2, bool clockwise, bool closeSegment)
	{
		m_Radius = point1 - point2;
		m_Radius.x = std::abs(m_Radius.x);
		m_Radius.y = std::abs(m_Radius.y);

		m_Angle = clockwise ? -90.f : 90.f;

		const auto& swapCenterPoint = [&](bool xComparison)							// CP1  _____P2
		{																			//	   /   
			if (xComparison) m_StartAngle += 90;									//	  /	    
			if (xComparison == clockwise) SetPosition(point1.x, point2.y);			//   |
		};																			//	 |
																					// P1		 CP2
		SetPosition(point2.x, point1.y);

#ifdef MATHEMATICAL_COORDINATESYSTEM
		if (point1.y >= point2.y)
#else
		if (point1.y <= point2.y)
#endif
		{
			m_StartAngle = 0;
			swapCenterPoint(point1.x <= point2.x);
		}
		else
		{
			m_StartAngle = 180;
			swapCenterPoint(point1.x > point2.x);
		}
		
		if (!clockwise) m_StartAngle += 90;

		return Recreate(m_Radius.x, m_Radius.y, m_StartAngle, m_Angle, closeSegment);

	}

	void Arc::SetPosition(int centerX, int centerY)
	{
		ResetPosition();
		Move(static_cast<float>(centerX), static_cast<float>(centerY));
	}

	//--------------------------------------------------------------------------------------------------------------------

}