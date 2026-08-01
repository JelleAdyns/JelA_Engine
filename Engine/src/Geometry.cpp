// ReSharper disable CppDFAUnusedValue
// ReSharper disable CppDFAUnreachableCode
// ReSharper disable CppDFAConstantConditions
#include "Geometry.h"
#include "Engine.h"
#include <algorithm>
#include <execution>
#include <numbers>

#include "Utils.h"

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
	HRESULT Geometry::Recreate(bool releasePrevious)
	{
		if (releasePrevious) SafeRelease(&m_pGeo);
		return ENGINE.Get2DFactory().get()->CreatePathGeometry(&m_pGeo);
	}
	void Geometry::swap(Geometry& other) noexcept
	{
		std::swap(m_Translation, other.m_Translation);
		std::swap(m_pGeo, other.m_pGeo);
	}
	//--------------------------------------------------------------------------------------------------------------------



	//--------------------------------------------------------------------------------------------------------------------
	// Polygon
	Polygon::Polygon(const std::vector<Point2f>& points, bool closeSegment) :
		Geometry{},
		m_Points{points}
	{
		AdjustPoints([&](Point2f& point) { point += GetTranslation(); });
		if (!Recreate(points, closeSegment))
			OutputDebugString(std::format(_T("Creation of Polygon failed!\n")).c_str());
	}

	bool Polygon::Recreate(const std::vector<Point2f>& points, bool closeSegment)
	{
		HRESULT hr = S_OK;

		m_Points = points;
		if (m_Points.empty()) return false;

		hr = Geometry::Recreate();


		if (!SUCCEEDED(hr)) return false;

		const auto pGeo = GetGeometry();
		if (!pGeo) return false;

		ID2D1GeometrySink* pSink{};
		hr = pGeo->Open(&pSink);

		if (SUCCEEDED(hr))
		{
			std::vector<D2D1_POINT_2F> D2points(m_Points.size());

			const auto windowHeight = ENGINE.GetGameSize().y;
			for (size_t i = 0; i < m_Points.size(); i++)
			{
				auto pointY = m_Points[i].y;
				if (USE_MATHEMATICAL_COORDINATESYSTEM) pointY = windowHeight - pointY;
				D2points[i] = D2D1::Point2F(m_Points[i].x, pointY);
			}

			pSink->BeginFigure(D2points.front(), D2D1_FIGURE_BEGIN_FILLED);
			pSink->AddLines(D2points.data(), static_cast<UINT32>(D2points.size()));
			pSink->EndFigure(closeSegment ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);

			hr = pSink->Close();
		}
        SafeRelease(&pSink);

		return hr == S_OK;
	}

	void Polygon::ResetPosition()
	{
		AdjustPoints([&](Point2f& point) { point -= GetTranslation(); });
		Geometry::ResetPosition();
	}
	void Polygon::Move(const Vector2f& translation)
	{
		Geometry::Move(translation);
		AdjustPoints([&](Point2f& point) { point += translation; });
	}

	std::vector<Point2f> Polygon::GetOriginalPoints() const
	{
		std::vector<Point2f>transformedPoints = m_Points;
		std::for_each(std::execution::par, transformedPoints.begin(), transformedPoints.end(),
			[&](Point2f& point) { point -= GetTranslation(); });
		return transformedPoints;
	}

	bool Polygon::IsPointInside(const Point2f& point) const
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

	void Polygon::AdjustPoints(const std::function<void(Point2f&)>& func)
	{
		std::for_each(std::execution::par, m_Points.begin(), m_Points.end(), func);
	}
	//--------------------------------------------------------------------------------------------------------------------


	//--------------------------------------------------------------------------------------------------------------------
	// Arc
	Arc::Arc(float centerX, float centerY, float radiusX, float radiusY, float startAngle, float angle, bool closeSegment) :
		Geometry{},
		m_Radius{radiusX,radiusY},
		m_StartAngle{startAngle},
		m_Angle{angle}
	{
		SetPosition(centerX,centerY);
		if (!Recreate(radiusX, radiusY, startAngle, angle, closeSegment))
		{
			OutputDebugString(std::format(_T("Creation of Arc failed! Tried to create an arc with:\n"
									"center: ({}, {})\nradius: (x:{}, y:{})\nstart angle: {}\nangle: {}\nclose segments: {} "),
				centerX,centerY,radiusX,radiusY,startAngle,angle, closeSegment).c_str());
		}
	}
	Arc::Arc(const Point2f& center, float radiusX, float radiusY, float startAngle, float angle, bool closeSegment) :
		Arc{ center.x, center.y, radiusX, radiusY, startAngle, angle, closeSegment }
	{}
	Arc::Arc(const Point2f& point1, const Point2f& point2, bool clockwise, bool closeSegment) :
		Geometry{}
	{
		if (!Recreate(point1, point2, clockwise, closeSegment))
		{
			OutputDebugString(std::format(_T("Creation of Arc failed! Tried to create an arc with:\n"
									"point 1: ({}, {})\npoint 2: ({}, {})\nclockwise: {}\nclose segments: {} "),
				point1.x,point1.y,point2.x,point2.y,clockwise,closeSegment).c_str());
		}
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

		HRESULT hr = Geometry::Recreate();

		if (!SUCCEEDED(hr)) return false;

		const auto pGeo = GetGeometry();
		if (!pGeo) return false;

		ID2D1GeometrySink* pSink;
		hr = pGeo->Open(&pSink);

		if (SUCCEEDED(hr))
		{
			const auto startRad = (startAngle + (angle < 0.f ? angle : 0)) * std::numbers::pi_v<float> / 180;
			const auto endRad = (startAngle + (angle > 0.f ? angle : 0)) * std::numbers::pi_v<float> / 180;

			const auto windowHeight = ENGINE.GetGameSize().y;

			const auto startX = radiusX * std::cosf(startRad);
			auto startY = radiusY * std::sinf(startRad);
			if (USE_MATHEMATICAL_COORDINATESYSTEM) startY = windowHeight - startY;

			const auto endX = radiusX * std::cosf(endRad);
			auto endY = radiusY * std::sinf(endRad);
			if (USE_MATHEMATICAL_COORDINATESYSTEM) endY = windowHeight - endY;

			const D2D1_ARC_SEGMENT arcSegment{
				D2D1::Point2F(endX, endY),
				D2D1::SizeF(radiusX, radiusY),
				0,
				D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
				std::abs(angle) < 180.f ? D2D1_ARC_SIZE_SMALL : D2D1_ARC_SIZE_LARGE
			};

			pSink->BeginFigure(D2D1::Point2F(startX, startY), D2D1_FIGURE_BEGIN_FILLED);
			pSink->AddArc(arcSegment);

			if (closeSegment) pSink->AddLine(D2D1::Point2F(0, USE_MATHEMATICAL_COORDINATESYSTEM ? windowHeight : 0));

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
	bool Arc::Recreate(const Point2f& point1, const Point2f& point2, bool clockwise, bool closeSegment)
	{
		m_Radius = point1 - point2;
		m_Radius.x = std::abs(m_Radius.x);
		m_Radius.y = std::abs(m_Radius.y);

		m_Angle = clockwise ? -90.f : 90.f;

        const auto& swapCenterPoint = [&](bool xComparison)					// CP1  _____P2
        {																			//	   /
            if (xComparison) m_StartAngle += 90;									//	  /
			if (xComparison == clockwise) SetPosition(point1.x, point2.y);			//   |
		};																			//	 |
																					// P1		 CP2
		SetPosition(point2.x, point1.y);

		if (USE_MATHEMATICAL_COORDINATESYSTEM ?
			(point1.y >= point2.y) :
			(point1.y <= point2.y))
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

	void Arc::SetPosition(float centerX, float centerY)
	{
		ResetPosition();
		Move(centerX, centerY);
	}

	//--------------------------------------------------------------------------------------------------------------------

}
