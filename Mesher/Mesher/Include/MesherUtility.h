#pragma once

#include "System.h"
using namespace GeometryCore;

namespace MesherUtility 
{

	// intersection API
	enum IntersectionType
	{
		Intersecting,
		NonIntersecting,
		OnSegmentEnds,
		Parallel,
		Overlapping,
		Skew,
		Unknown
	};

	IntersectionType		 intersect					(	const Vector3D & p1, const Vector3D & p2, 
															const Vector3D & q1, const Vector3D & q2, 
															double & t1, double & t2, 
															const Tolerance & inTol = Tolerance());

	inline
	bool					isCollinear					(	const Vector3D & inPoint1,
															const Vector3D & inPoint2,
															const Vector3D & inPoint3,
															const Tolerance & inTol = Tolerance());

	inline
	bool					isCounterClockwise			(	const Vector3D & inPoint1,  
															const Vector3D & inPoint2, 
															const Vector3D & inPoint3,
															const Tolerance & inTol = Tolerance());

	bool					isCounterClockwise			(	const list<Vertex*> & inPolygon,
															const Tolerance & inTol = Tolerance());

	inline
	bool					isSegmentsIntersecting		(   const Vector3D & inPoint1,  
															const Vector3D & inPoint2, 
															const Vector3D & inPoint3,
															const Vector3D & inPoint4,
															const Tolerance & inTol = Tolerance());

	double					findShortestEdgeLen			(	const Triangle* inTriangle);

	double					findSmallestAngle			(	const Triangle* inTriangle);

	double					findMinimumAngle			(	const Triangle * inTriangle);

	double					findLargestEdgeLen			(	const Triangle* inTriangle);

	Edge*					findLargestEdge				(	const Triangle* inTriangle);

	double					minAngleAroundVertex		(   const Vertex* inVertex);

}