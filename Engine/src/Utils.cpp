#include "Utils.h"

namespace jela::utils
{

    // Following functions originate from Koen Samyn, professor Game Development at Howest

    // Private Implementations
    struct QuadraticInformation
    {
        float a{};
        float b{};
        float c{};
        float Discriminant{};
    };
    bool LineIntersectsEllipse(const Point2f& point1, const Point2f& point2, const Ellipsef& e, QuadraticInformation& info)
    {
        //https://math.stackexchange.com/questions/2534644/points-of-intersection-between-line-and-ellipse
        const jela::Vector2f slope{ point2 - point1 };
        const float slopeCoefficient{ slope.y / slope.x }; // Represents 'm' in 'y = mx + a'

        const float lineElevation{ (point1.y - e.center.y) - slopeCoefficient * (point1.x - e.center.x) }; // Represents 'a' in 'y = mx + a'
        const float yRadSquared{ e.radiusY * e.radiusY };
        const float xRadSquared{ e.radiusX * e.radiusX };

        // ax^2 + bx + c = 0
        const float a{ yRadSquared + xRadSquared * slopeCoefficient * slopeCoefficient };
        const float b{ 2 * xRadSquared * slopeCoefficient * lineElevation };
        const float c{ xRadSquared * (lineElevation * lineElevation) - xRadSquared * yRadSquared };

        // D = b^2 - 4*a*c
        const float D{ b * b - 4 * a * c };
        info = { a, b, c, D };
        return D >= 0; // infinite line overlaps
    }
    void CalculateIntersections(float slopeCoefficient, float lineElevation, const QuadraticInformation& info, std::pair<Point2f, Point2f>& intersections)
    {
        // (-b +- sqrt(D)) / 2*a
        float x1{}, x2{};

        if (std::abs(info.a) > FLT_EPSILON)
        {
            const float sqrtD{ sqrtf(info.Discriminant) };
            const float denom{ 1 / (2 * info.a) };

            x1 = (-info.b - sqrtD) * denom;
            x2 = (-info.b + sqrtD) * denom;
        }
        // y = mx + a
        const float y1 = slopeCoefficient * x1 + lineElevation;
        const float y2 = slopeCoefficient * x2 + lineElevation;

        intersections.first.x += x1;
        intersections.first.y += y1;
        intersections.second.x += x2;
        intersections.second.y += y2;

        if (std::abs(info.Discriminant) < FLT_EPSILON)
        {
            intersections.second.x = FLT_MAX;
            intersections.second.y = FLT_MAX;
        }
    }
    Intersections IntersectionPointsLieOnLine(const Point2f& point1, const Point2f& point2, std::pair<Point2f, Point2f>& intersections)
    {
        const bool firstPointLiesOnLine{ IsPointOnLineSegment(intersections.first, point1, point2, 0.01f) };
        const bool secondPointLiesOnLine{ IsPointOnLineSegment(intersections.second, point1, point2, 0.01f) };
        if (!firstPointLiesOnLine) intersections.first = jela::Point2f{ FLT_MAX, FLT_MAX};
        if (!secondPointLiesOnLine) intersections.second = jela::Point2f{ FLT_MAX, FLT_MAX };

        if (firstPointLiesOnLine && secondPointLiesOnLine) return Intersections::Double;
        if (firstPointLiesOnLine || secondPointLiesOnLine) return Intersections::One;
        return Intersections::None;
    }

    // Public Implementations
    float Distance(float x1, float y1, float x2, float y2)
    {
        const float b = x2 - x1;
        const float c = y2 - y1;
        return sqrtf(b * b + c * c);
    }

    float Distance(const Point2f & p1, const Point2f & p2)
    {
        return Distance(p1.x, p1.y, p2.x, p2.y);
    }

    bool IsPointInRect(const Point2f & p, const Rectf & r)
    {
        return p.x >= r.left and
            p.x <= (r.left + r.width) and
#ifdef MATHEMATICAL_COORDINATESYSTEM
            p.y >= r.bottom and
            p.y <= (r.bottom + r.height);
#else
            p.y >= r.top and
            p.y <= (r.top + r.height);
#endif // MATHEMATICAL_COORDINATESYSTEM

    }

    bool IsPointInCircle(const Point2f & p, const Circlef & c)
    {
        const float x = c.center.x - p.x;
        const float y = c.center.y - p.y;
        return x * x + y * y <= c.rad * c.rad;
    }

    bool IsPointInEllipse(const Point2f & p, const Ellipsef & e)
    {
        const float xDist = p.x - e.center.x;
        const float yDist = p.y - e.center.y;
        const float xRadSqrd = e.radiusX * e.radiusX;
        const float yRadSqrd = e.radiusY * e.radiusY;

        const float lhs = xDist * xDist * yRadSqrd + yDist * yDist * xRadSqrd;
        const float rhs = xRadSqrd * yRadSqrd;

        return lhs <= rhs;
    }
    bool IsOverlapping(const Point2f & point1, const Point2f & point2, const Circlef & c)
    {
        return DistPointLineSegment(c.center, point1, point2) <= c.rad;
    }

    bool IsOverlapping(const Point2f & point1, const Point2f & point2, const Ellipsef & e)
    {
        if(IsPointInEllipse(point1, e) || IsPointInEllipse(point2, e)) return true;

        std::pair<Point2f, Point2f> points{};
        const Intersections intersects = IntersectEllipseLineSegment(e, point1, point2, points);
        return intersects == Intersections::Double || intersects == Intersections::One;
    }

    bool IsOverlapping(const Point2f & point1, const Point2f & point2, const Rectf & r)
    {
        if (IsPointInRect(point1, r) || IsPointInRect(point2, r)) return true;

        std::pair<Point2f, Point2f> p{};
        const Intersections intersects = IntersectRectLine(r, point1, point2, p);
        return intersects == Intersections::Double || intersects == Intersections::One;
    }

    bool IsOverlapping(const Rectf & r1, const Rectf & r2)
    {

#ifdef MATHEMATICAL_COORDINATESYSTEM
        if ((r1.left + r1.width) < r2.left || (r2.left + r2.width) < r1.left ||
            r1.bottom > (r2.bottom + r2.height) || r2.bottom > (r1.bottom + r1.height))
        {
            return false;
        }
#else
        if ((r1.left + r1.width) < r2.left || (r2.left + r2.width) < r1.left ||
            (r1.top + r1.height) < r2.top || (r2.top + r2.height) < r1.top)
        {
            return false;
        }
#endif // MATHEMATICAL_COORDINATESYSTEM

        return true;
    }

    bool IsOverlapping(const Rectf & r, const Circlef & c)
    {
        if (IsPointInRect(c.center, r)) return true;

        const auto isOverlapping = [c] (const Point2f& p1, const Point2f& p2)
        {
            return (c.center - ClosestPointOnLine(c.center, p1, p2)).SquaredLength() <= c.rad * c.rad;
        };
        const float right = r.left + r.width;

#ifdef MATHEMATICAL_COORDINATESYSTEM
        const float top = r.bottom + r.height;
        if (isOverlapping( Point2f{ r.left, r.bottom }, Point2f{ r.left, top })) return true;

        if (isOverlapping( Point2f{ r.left, r.bottom }, Point2f{ right, r.bottom })) return true;

        if (isOverlapping( Point2f{ r.left, top }, Point2f{ right, top })) return true;

        if (isOverlapping( Point2f{ right, top }, Point2f{ right, r.bottom })) return true;
#else
        const float bottom = r.top + r.height;
        if (isOverlapping( Point2f{ r.left, r.top }, Point2f{ r.left, bottom })) return true;

        if (isOverlapping( Point2f{ r.left, r.top }, Point2f{ right, r.top })) return true;

        if (isOverlapping( Point2f{ r.left, bottom }, Point2f{ right, bottom })) return true;

        if (isOverlapping( Point2f{ right, bottom }, Point2f{ right, r.top })) return true;

#endif // MATHEMATICAL_COORDINATESYSTEM

        return false;
    }

    bool IsOverlapping(const Circlef & c1, const Circlef & c2)
    {
        return (c2.center - c1.center).SquaredLength() < (c1.rad + c2.rad) * (c1.rad + c2.rad);
    }


    Point2f ClosestPointOnLine(const Point2f & point, const Point2f & linePointA, const Point2f & linePointB)
    {
        const Vector2f aToB{ linePointA, linePointB };
        const Vector2f aToPoint{ linePointA, point };
        float pointProjectionOnLine{ Vector2f::Dot(aToB, aToPoint) };

        // If pointProjectionOnLine is negative, then the closest point is A
        if (pointProjectionOnLine < 0) return linePointA;

        const Vector2f abNorm{ aToB.Normalized() };
        pointProjectionOnLine = Vector2f::Dot(abNorm, aToPoint);

        // If pointProjectionOnLine is > than dist(linePointA,linePointB) then the closest point is B
        if (const float squaredDistAB{ aToB.SquaredLength() };
            pointProjectionOnLine * pointProjectionOnLine > squaredDistAB) return linePointB;

        // Closest point is between A and B, calc intersection point
        const Point2f intersection{ linePointA + pointProjectionOnLine * abNorm };
        return intersection;
    }

    float DistPointLineSegment(const Point2f & point, const Point2f & linePointA, const Point2f & linePointB)
    {
        return (point - ClosestPointOnLine(point, linePointA, linePointB)).Length();
    }

    bool IsPointOnLineSegment(const Point2f & point, const Point2f & linePointA, const Point2f & linePointB, float epsilon)
    {
        const Vector2f aToPoint{ linePointA, point };
        const Vector2f bToPoint{ linePointB, point };

        // If not on same line, return false
        if (std::abs(Vector2f::Cross(aToPoint, bToPoint)) > epsilon) return false;

        // Both vectors must point in opposite directions if p is between a and b
        if (Vector2f::Dot(aToPoint, bToPoint) > 0) return false;

        return true;
    }

    Intersections IntersectEllipse(const Ellipsef& e, const Vector2f& line, const Point2f& origin, std::pair<Point2f, Point2f>& intersections)
    {
        if (std::abs(line.x) < FLT_EPSILON) // Vertical line
        {
            if (std::abs(origin.x - (e.center.x - e.radiusX)) < FLT_EPSILON || std::abs(origin.x - (e.center.x + e.radiusX)) < FLT_EPSILON)
            {
                intersections.first = jela::Point2f{ origin.x, e.center.y };
                intersections.second = jela::Point2f{ FLT_MAX, FLT_MAX };
                return Intersections::One;
            }
            if (origin.x > e.center.x - e.radiusX && origin.x < e.center.x + e.radiusX)
            {
                const float sqrtRoot = sqrtf(1 - ((origin.x - e.center.x) * (origin.x - e.center.x)) / (e.radiusX * e.radiusX));
                intersections.first = jela::Point2f{ origin.x, e.center.y - e.radiusY * sqrtRoot };
                intersections.second = jela::Point2f{ origin.x, e.center.y + e.radiusY * sqrtRoot };
                return Intersections::Double;
            }

            return Intersections::None;
        }

        QuadraticInformation info{};
        if (!LineIntersectsEllipse(origin, origin + line, e, info)) return Intersections::None;

        const float slopeCoefficient{ line.y / line.x }; // Represents 'm' in 'y = mx + a'
        const float lineElevation{ (origin.y - e.center.y) - slopeCoefficient * (origin.x - e.center.x) }; // Represents 'a' in 'y = mx + a'

        intersections.first.x = e.center.x;
        intersections.second.x = e.center.x;
        intersections.first.y = e.center.y;
        intersections.second.y = e.center.y;

        CalculateIntersections(slopeCoefficient, lineElevation, info, intersections);

        if (std::abs(info.Discriminant) < FLT_EPSILON)
            return Intersections::One;

        return Intersections::Double;
    }
    Intersections IntersectEllipseLineSegment(const Ellipsef& e, const Point2f& point1, const Point2f& point2, std::pair<Point2f, Point2f>& intersections)
    {
        if (IntersectEllipse(e, { point1, point2 }, point1, intersections) == Intersections::None)
            return Intersections::None;

        return IntersectionPointsLieOnLine(point1, point2, intersections);
    }
    Intersections IntersectCircle(const Circlef& circle, const Vector2f& line, const Point2f& origin, std::pair<Point2f, Point2f>& intersections)
    {
        if (std::abs(line.x) < FLT_EPSILON) // Vertical line
        {
            if (std::abs(origin.x - (circle.center.x - circle.rad)) < FLT_EPSILON || std::abs(origin.x - (circle.center.x + circle.rad)) < FLT_EPSILON)
            {
                intersections.first = jela::Point2f{ origin.x, circle.center.y};
                intersections.second = jela::Point2f{ FLT_MAX, FLT_MAX };
                return Intersections::One;
            }
            if (origin.x >= circle.center.x - circle.rad && origin.x <= circle.center.x + circle.rad)
            {
                const float sqrtRoot = sqrtf(circle.rad * circle.rad - (origin.x - circle.center.x) * (origin.x - circle.center.x));
                intersections.first = jela::Point2f{ origin.x, circle.center.y - sqrtRoot };
                intersections.second = jela::Point2f{ origin.x, circle.center.y + sqrtRoot };

                return Intersections::Double;
            }

            return Intersections::None;
        }

        const float slopeCoefficient{ line.y / line.x }; // Represents 'm' in 'y = mx + a'
        const float lineElevation{ (origin.y - circle.center.y) - slopeCoefficient * (origin.x - circle.center.x) }; // Represents 'a' in 'y = mx + a'

        // ax^2 + bx + c = 0
        const float a{ 1 + slopeCoefficient * slopeCoefficient };
        const float b{ 2 * slopeCoefficient * lineElevation };
        const float c{ lineElevation * lineElevation - circle.rad * circle.rad };

        // D = b^2 - 4*a*c
        float D{ b * b - 4 * a * c };

        if (D < -FLT_EPSILON) return Intersections::None;
        if (D < 0.0f) D = 0;

        intersections.first.x = circle.center.x;
        intersections.second.x = circle.center.x;
        intersections.first.y = circle.center.y;
        intersections.second.y = circle.center.y;

        CalculateIntersections(slopeCoefficient, lineElevation, { a,b,c,D }, intersections);

        if (std::abs(D) < FLT_EPSILON)
            return Intersections::One;

        return Intersections::Double;
    }
    Intersections IntersectCircleLineSegment(const Circlef& circle, const Point2f& point1, const Point2f& point2, std::pair<Point2f, Point2f>& intersections)
    {
        if (IntersectCircle(circle, { point1, point2 }, point1, intersections) == Intersections::None)
            return Intersections::None;

        return IntersectionPointsLieOnLine(point1, point2, intersections);
    }
    bool IntersectLines(const Vector2f & l1, const Point2f & origin1, const Vector2f & l2, const Point2f & origin2)
    {
        if (std::abs(Vector2f::Cross(l1, l2)) <= FLT_EPSILON) // if parallel
        {
            const Vector2f OriginToOrigin{ origin1, origin2 };
            // if there's an offset, return false
            if (std::abs(Vector2f::Cross(OriginToOrigin, l2)) > FLT_EPSILON) return false;
        }
        return true;
    }
    bool IntersectLineSegments(const Point2f & p1, const Point2f & p2, const Point2f & q1, const Point2f & q2, float& line1Interpolation, float& line2Interpolation)
    {
        if (!IntersectLines(p1, p2, q1, q2)) return false;

        bool intersecting{ false };
        const Vector2f firstLine{ p1, p2 };
        const Vector2f secondLine{ q1, q2 };

        if (const float crossArea = Vector2f::Cross(firstLine, secondLine);
            std::abs(crossArea) <= FLT_EPSILON) // if parallel
        {
            line1Interpolation = 0;
            line2Interpolation = 0;
            if (IsPointOnLineSegment(p1, q1, q2) ||
                IsPointOnLineSegment(p2, q1, q2))
            {
                intersecting = true;
            }
        }
        else
        {
            const Vector2f p1q1{ p1, q1 };
            const float num1 = Vector2f::Cross(p1q1, secondLine);
            const float num2 = Vector2f::Cross(p1q1, firstLine);

            line1Interpolation = num1 / crossArea;
            line2Interpolation = num2 / crossArea;

            if (line1Interpolation > 0 && line1Interpolation <= 1 && line2Interpolation > 0 && line2Interpolation <= 1)
                intersecting = true;

        }

        return intersecting;
    }

    Intersections IntersectRectLine(const Rectf & r, const Point2f & p1, const Point2f & p2, std::pair<Point2f, Point2f>&intersections)
    {
        const float xDenom{ p2.x - p1.x };
        const float x1{ (r.left - p1.x) / xDenom };
        const float x2{ (r.left + r.width - p1.x) / xDenom };

        float yDenom{ p2.y - p1.y };
#ifdef MATHEMATICAL_COORDINATESYSTEM
        const float y1{ (r.bottom - p1.y) / yDenom };
        const float y2{ (r.bottom + r.height - p1.y) / yDenom };
#else
        const float y1{ (r.top - p1.y) / yDenom };
        const float y2{ (r.top + r.height - p1.y) / yDenom };
#endif // !MATHEMATICAL_COORDINATESYSTEM

        const float tMin{ std::max(std::min(x1,x2), std::min(y1,y2)) };
        const float tMax{ std::min(std::max(x1,x2), std::max(y1,y2)) };

        if (tMin > tMax) return Intersections::None;

        const Vector2f lineDirection{ p1, p2 };
        intersections.first = p1 + lineDirection * tMin;
        intersections.second = p1 + lineDirection * tMax;

        return IntersectionPointsLieOnLine(p1, p2, intersections);
    }

}
