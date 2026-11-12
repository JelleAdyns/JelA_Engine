#ifndef UTILS_H
#define UTILS_H

#include "Structs.h"

namespace jela::utils
{
    // Following functions originate from Koen Samyn, professor Game Development at Howest

    enum class Intersections
    {
        Double,
        One,
        None
    };

    float Distance(float x1, float y1, float x2, float y2);
    float Distance(const Point2f& p1, const Point2f& p2);

    bool IsPointInRect(const Point2f& p, const Rectf& r);
    bool IsPointInCircle(const Point2f& p, const Circlef& c);
    bool IsPointInEllipse(const Point2f& p, const Ellipsef& e);

    bool IsOverlapping(const Point2f& point1, const Point2f& point2, const Circlef& c);
    bool IsOverlapping(const Point2f& point1, const Point2f& point2, const Ellipsef& e);
    bool IsOverlapping(const Point2f& point1, const Point2f& point2, const Rectf& r);
    bool IsOverlapping(const Rectf& r1, const Rectf& r2);
    bool IsOverlapping(const Rectf& r, const Circlef& c);
    bool IsOverlapping(const Circlef& c1, const Circlef& c2);

    Point2f ClosestPointOnLine(const Point2f& point, const Point2f& linePointA, const Point2f& linePointB);
    float DistPointLineSegment(const Point2f& point, const Point2f& linePointA, const Point2f& linePointB);
    bool IsPointOnLineSegment(const Point2f& point, const Point2f& linePointA, const Point2f& linePointB, float epsilon = FLT_EPSILON);
    Intersections IntersectEllipse(const Ellipsef& e, const Vector2f& line, const Point2f& origin, std::pair<Point2f, Point2f>& intersections);
    Intersections IntersectEllipseLineSegment(const Ellipsef& e, const Point2f& point1, const Point2f& point2, std::pair<Point2f, Point2f>& intersections);
    Intersections IntersectCircle(const Circlef& circle, const Vector2f& line, const Point2f& origin, std::pair<Point2f, Point2f>& intersections);
    Intersections IntersectCircleLineSegment(const Circlef& circle, const Point2f& point1, const Point2f& point2, std::pair<Point2f, Point2f>& intersections);
    bool IntersectLines(const Vector2f& l1, const Point2f& origin1, const Vector2f& l2, const Point2f& origin2);
    bool IntersectLineSegments(const Point2f& p1, const Point2f& p2, const Point2f& q1, const Point2f& q2, float& line1Interpolation, float& line2Interpolation);
    Intersections IntersectRectLine(const Rectf& r, const Point2f& p1, const Point2f& p2, std::pair<Point2f, Point2f>& intersections);

    template <std::swappable E>
    static void SwapEraseOnVector(std::vector<E>& v, size_t idx)
    {
        if (idx >= v.size()) throw std::out_of_range{"Index out of range"};

        v[idx].swap(v.back());
        v.pop_back();
    }
}

#endif //UTILS_H
