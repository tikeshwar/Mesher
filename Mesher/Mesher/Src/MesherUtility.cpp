#include "MesherUtility.h"

namespace MesherUtility {

IntersectionType		intersect					(	const Vector3D & p1, const Vector3D & p2, 
														const Vector3D & q1, const Vector3D & q2, 
														double & t1, double & t2, 
														const Tolerance & inTol)
{
	auto dirP = p2 - p1;
	auto dirQ = q2 - q1;

	auto crossPQ = dirP^dirQ;
	auto crossPQMag = crossPQ.magnitude();
	if(inTol.isZero(crossPQMag))
	{
		auto q1p1 = q1 - p1;
		auto skewDistance = (crossPQ/crossPQMag)*q1p1;
		if(inTol.isZero(skewDistance))
			return Overlapping;
		else
			return Parallel;
	}
	else
	{
		auto q1p1 = q1 - p1;
		auto skewDistance = (crossPQ/crossPQMag)*q1p1;
		if(inTol.isZero(skewDistance))
		{
			auto cross = q1p1^dirQ;
			if(inTol.isPositive (cross*crossPQ))
				t1 = cross.magnitude()/crossPQ.magnitude();
			else
				t1 = -cross.magnitude()/crossPQ.magnitude();
			
			cross = -q1p1^dirP;
			if(inTol.isPositive (cross*(-crossPQ)))
				t2 = cross.magnitude()/crossPQ.magnitude();
			else
				t2 = -cross.magnitude()/crossPQ.magnitude();
			
			if((inTol.isGreater(t1,0) && inTol.isLess(t1,1.0)) &&
				(inTol.isGreater(t2,0) && inTol.isLess(t2,1.0)))
				return Intersecting;
			else if((inTol.isGreater(t1,1.0) || inTol.isLess(t1,0)) && 
				(inTol.isGreater(t2,1.0) || inTol.isLess(t2,0)))
				return NonIntersecting;
			else if((inTol.isEqaul(t1,1.0) || inTol.isEqaul(t1,0)) && 
				(inTol.isEqaul(t2,1.0) || inTol.isEqaul(t2,0)))
				return OnSegmentEnds;
		}
		else
			return Skew;
	}
	return MesherUtility::Unknown;
}

extern inline
bool					isCollinear					(	const Vector3D & inPoint1,
														const Vector3D & inPoint2,
														const Vector3D & inPoint3,
														const Tolerance & inTol)
{
	return inTol.isZero(SIGNED_AREA(inPoint1, inPoint2, inPoint3));
}

extern inline
bool					isCounterClockwise			(	const Vector3D & inPoint1,  
														const Vector3D & inPoint2, 
														const Vector3D & inPoint3,
														const Tolerance & inTol)
{
	return	inTol.isGreater((inPoint3.y() - inPoint1.y())*(inPoint2.x() - inPoint1.x()),
			(inPoint2.y() - inPoint1.y())*(inPoint3.x() - inPoint1.x())); 
}

bool					isCounterClockwise			(	const list<Vertex*> & inPolygon,
														const Tolerance & inTol)
{
	list<Vertex*> polygon(inPolygon);
	if(inPolygon.front() != inPolygon.back())
		polygon.push_back(polygon.front());
	
	double sum = 0;
	for(auto it = polygon.begin(); it != --polygon.end(); )
	{
		auto p1 = (*it)->point();
		auto p2 = (*(++it))->point();
		sum += (p2.x() - p1.x())*(p2.y() + p1.y());
	}

	return inTol.isNegative(sum);
}

extern inline
bool					isSegmentsIntersecting		(   const Vector3D & inPoint1, 
														const Vector3D & inPoint2, 
														const Vector3D & inPoint3,
														const Vector3D & inPoint4,
														const Tolerance & inTol)
{
	return	isCounterClockwise(inPoint1, inPoint3, inPoint4, inTol) != 
			isCounterClockwise(inPoint2, inPoint3, inPoint4, inTol) &&
			isCounterClockwise(inPoint1, inPoint2, inPoint3, inTol) != 
			isCounterClockwise(inPoint1, inPoint2, inPoint4, inTol);
}

double					findShortestEdgeLen			(	const Triangle* inTriangle)
{
	double l1 = inTriangle->edge(0)->lengthSqr();
	double l2 = inTriangle->edge(1)->lengthSqr();
	double l3 = inTriangle->edge(2)->lengthSqr();

	double min = min(l1, min(l2, l3));
	return sqrt(min);
}

double					findSmallestAngle			(	const Triangle* inTriangle)
{
	double l1 = inTriangle->edge(0)->lengthSqr();
	double l2 = inTriangle->edge(1)->lengthSqr();
	double l3 = inTriangle->edge(2)->lengthSqr();

	double max = max(l1,max(l2, l3));
	return sqrt(max);
}

double					findMinimumAngle			(	const Triangle * inTriangle)
{
	return inTriangle->smallestAngle();
}

double					findLargestEdgeLen			(	const Triangle* inTriangle)
{
	double l1 = inTriangle->edge(0)->lengthSqr();
	double l2 = inTriangle->edge(1)->lengthSqr();
	double l3 = inTriangle->edge(2)->lengthSqr();

	double max = max(l1,max(l2, l3));
	return sqrt(max);
}

Edge*					findLargestEdge				(	const Triangle* inTriangle)
{
	map<double, Edge*> edgeMap;
	edgeMap.insert(map<double, Edge*>::value_type( inTriangle->edge(0)->lengthSqr(),inTriangle->edge(0)));
	edgeMap.insert(map<double, Edge*>::value_type( inTriangle->edge(1)->lengthSqr(),inTriangle->edge(1)));
	edgeMap.insert(map<double, Edge*>::value_type( inTriangle->edge(2)->lengthSqr(),inTriangle->edge(2)));

	return (--edgeMap.end())->second;
}

double					minAngleAroundVertex		(   const Vertex* inVertex)
{
	if(inVertex == nullptr)
		return 1e10;

	double minAngle = 1e10;
	list<Triangle*> triangleList;
	list<Edge*> edgeList;
	inVertex->neighbours(triangleList, edgeList);

	for(auto tritr = triangleList.begin(); tritr != triangleList.end(); tritr++)
	{
		double angle = (*tritr)->angleAt(inVertex);
		if(angle < minAngle)
			minAngle = angle;
	}
	return minAngle;
}

}