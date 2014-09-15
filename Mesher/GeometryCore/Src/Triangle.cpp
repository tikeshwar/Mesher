#include "StdAfx.h"
#include "Edge.h"
#include "Triangle.h"

using namespace GeometryCore;

void Triangle::findCircumCircle					(	const Vector3D & p1,
													const Vector3D & p2,
													const Vector3D & p3,
													Vector3D & outCenter, 
													double & outRadius)
{
	Vector3D p1p3 = p1 - p3;
	Vector3D p2p3 = p2 - p3;
	Vector3D p1p3xp2p3 = p1p3^p2p3;

	outCenter = p3 + (((p1p3*p1p3)*p2p3 - (p2p3*p2p3)*p1p3)^p1p3xp2p3)/(2*p1p3xp2p3*p1p3xp2p3);
	outRadius = (outCenter - p1).magnitude();
}
Triangle::Triangle(Vertex* v1, Vertex* v2, Vertex* v3, Edge* e1, Edge* e2, Edge* e3)
{
	mVertices[0] = v1;
	mVertices[1] = v2;
	mVertices[2] = v3;

	mEdges[0] = e1;
	mEdges[1] = e2;
	mEdges[2] = e3;

	Vector3D center;
	double radius = 0;
	findCircumCircle(mVertices[0]->point(), mVertices[1]->point(), mVertices[2]->point(), center, radius);
	mCircle = new Circle(center, radius);


	// finding the aspect ratio
	auto shortestEdge = smallestEdge();
	auto oppVertex = opposite(shortestEdge);
	auto vec1 = oppVertex->point() - shortestEdge->vertex(0)->point();
	auto vec2 = oppVertex->point() - shortestEdge->vertex(1)->point();
	vec1.normalize();
	vec2.normalize();
	double sineOfMinAngle = (vec1^vec2).magnitude();
	mRatio = 1/(2*sineOfMinAngle);
}
Triangle::~Triangle()
{
	if(mCircle)
		delete mCircle;
}

inline Edge*		Triangle::edge			( size_t index)
{
	return mEdges[index];
}
inline const Edge*	Triangle::edge			( size_t index)const
{
	return mEdges[index];
}
inline Vertex*		Triangle::vertex		( size_t index)
{
	return mVertices[index];
}
inline const Vertex*Triangle::vertex		( size_t index)const
{
	return mVertices[index];
}
inline bool			Triangle::isVertex		( const Vertex* inVertex)const
{
	return (mVertices[0] == inVertex || mVertices[1] == inVertex || mVertices[2] == inVertex);
}
inline bool			Triangle::isEdge		( const Edge* inEdge)const
{
	return (mEdges[0] == inEdge || mEdges[1] == inEdge || mEdges[2] == inEdge);
}
void				Triangle::neighbours	( const Triangle* neighbors[3])const
{
	neighbors[0] = mEdges[0]->otherTriangle(this);
	neighbors[1] = mEdges[1]->otherTriangle(this);
	neighbors[2] = mEdges[2]->otherTriangle(this);
}
double				Triangle::angleAt		( const Vertex* inVertex)const
{
	const Edge* edge = opposite(inVertex);
	Vector3D v1 = inVertex->point() - edge->vertex(0)->point();
	Vector3D v2 = inVertex->point() - edge->vertex(1)->point();
	v1.normalize();
	v2.normalize();
	return acos(v1*v2);
}
Edge*				Triangle::opposite		( const Vertex* inVertex)
{
	if(mVertices[0] == inVertex)return mEdges[0];
	if(mVertices[1] == inVertex)return mEdges[1];
	if(mVertices[2] == inVertex)return mEdges[2];
	return nullptr;
}
const Edge*			Triangle::opposite		( const Vertex* inVertex)const
{
	if(mVertices[0] == inVertex)return mEdges[0];
	if(mVertices[1] == inVertex)return mEdges[1];
	if(mVertices[2] == inVertex)return mEdges[2];
	return nullptr;
}
Vertex*				Triangle::opposite		( const Edge* inEdge)
{
	if(mEdges[0] == inEdge)return mVertices[0];
	if(mEdges[1] == inEdge)return mVertices[1];
	if(mEdges[2] == inEdge)return mVertices[2];
	return nullptr;
}
const Vertex*		Triangle::opposite		( const Edge* inEdge)const
{
	if(mEdges[0] == inEdge)return mVertices[0];
	if(mEdges[1] == inEdge)return mVertices[1];
	if(mEdges[2] == inEdge)return mVertices[2];
	return nullptr;
}
inline const Circle&Triangle::circumCircle	( )
{
	return *mCircle;
}
inline Vector3D		Triangle::centroid		( )const
{
	return (mVertices[0]->point() + mVertices[1]->point() + mVertices[2]->point())/3.0;
}
inline double		Triangle::area			( )const
{
	return 0.5*((mVertices[0]->point() - mVertices[1]->point())^(mVertices[1]->point() - mVertices[2]->point())).magnitude();
}
inline double  		Triangle::aspectRatio   ( )const
{
	return mRatio;
}
const Edge*			Triangle::smallestEdge	( )const
{
	typedef map<double, const Edge*> EdgeMap;
	EdgeMap edgeMap;
	edgeMap.insert(EdgeMap::value_type(edge(0)->lengthSqr(),edge(0)));
	edgeMap.insert(EdgeMap::value_type(edge(1)->lengthSqr(),edge(1)));
	edgeMap.insert(EdgeMap::value_type(edge(2)->lengthSqr(),edge(2)));

	return edgeMap.begin()->second;
}
const Edge*			Triangle::largestEdge	( )const
{
	typedef map<double, const Edge*> EdgeMap;
	EdgeMap edgeMap;
	edgeMap.insert(EdgeMap::value_type(edge(0)->lengthSqr(),edge(0)));
	edgeMap.insert(EdgeMap::value_type(edge(1)->lengthSqr(),edge(1)));
	edgeMap.insert(EdgeMap::value_type(edge(2)->lengthSqr(),edge(2)));

	return (--edgeMap.end())->second;
}
double				Triangle::smallestAngle	( )const
{
	double a0 = angleAt(mVertices[0]);
	double a1 = angleAt(mVertices[1]);
	double a2 = angleAt(mVertices[2]);
	return min(a0, min(a1, a2));
}
double				Triangle::largestAngle	( )const
{
	double a0 = angleAt(mVertices[0]);
	double a1 = angleAt(mVertices[1]);
	double a2 = angleAt(mVertices[2]);
	return max(a0, max(a1, a2));
}
Triangle::Position	Triangle::isInside		( const Vector3D & inPoint,
											  const Tolerance & inTol)const
{
	//GeometryCore::Vector3D pt1 = inTriangle->vertex(0)->point();
	//GeometryCore::Vector3D pt2 = inTriangle->vertex(1)->point();
	//GeometryCore::Vector3D pt3 = inTriangle->vertex(2)->point();
	//double a = area(pt1, pt2, pt3);
	//double a1 = area(pt1, pt2, inPoint);
	//double a2 = area(pt2, pt3, inPoint);
	//double a3 = area(pt3, pt1, inPoint);
	//double suma = a1+a2+a3;

	//if( fabs(a - suma) < TOL)
	//	return true;
	//return false;

	const GeometryCore::Vector3D & A = mVertices[0]->point();
	const GeometryCore::Vector3D & B = mVertices[1]->point();
	const GeometryCore::Vector3D & C = mVertices[2]->point();
	const GeometryCore::Vector3D & P = inPoint;	
	// Compute vectors        
	const GeometryCore::Vector3D & v0 = C - A;
	const GeometryCore::Vector3D & v1 = B - A;
	const GeometryCore::Vector3D & v2 = P - A;

	// Compute dot products
	double dot00 = (v0*v0);
	double dot01 = (v0*v1);
	double dot02 = (v0*v2);
	double dot11 = (v1*v1);
	double dot12 = (v1*v2);

	// Compute barycentric coordinates
	double invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	double v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	if((inTol.isEqaul(u,0) || inTol.isGreater(u,0)) &&
	   (inTol.isEqaul(v,0) || inTol.isGreater(v,0)) &&
	   (inTol.isEqaul(u+v,1.0) || inTol.isLess(u+v,1.0)))
	{
		if(inTol.isEqaul(u,1.0) || inTol.isEqaul(v,1.0))
			return OnVertex;
		else if(inTol.isEqaul(u + v,1.0))
			return OnEdge;
		else if(inTol.isLess(u + v,1.0))
			return Inside;
	}
	return Outside;
}