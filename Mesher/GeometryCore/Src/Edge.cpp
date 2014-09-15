#include "StdAfx.h"
#include <Vertex.h>
#include <Edge.h>
#include <Typedefs.h>

using namespace GeometryCore;

Edge::Edge( Vertex* v1, Vertex* v2)
{
	mVertices[0] = v1;
	mVertices[1] = v2;

 	mTriangles[0] = mTriangles[1] = nullptr;
}
Edge::~Edge()
{
	if(mTriangles[0] != nullptr || mTriangles[1] != nullptr)
		return;
}
//inline Vertex*				Edge::vertex				( size_t index)
//{
//	return mVertices[index];
//}
inline Vertex*				Edge::vertex				( size_t index)
{
	return mVertices[index];
}
inline const Vertex*		Edge::vertex				( size_t index)const
{
	return mVertices[index];
}
inline Vertex*		Edge::otherVertex					( const Vertex* inVertex)
{
	if(inVertex == mVertices[0])
		return mVertices[1];
	return mVertices[0];
}
inline const Vertex*		Edge::otherVertex			( const Vertex* inVertex)const
{
	if(inVertex == mVertices[0])
		return mVertices[1];
	return mVertices[0];
}
inline Triangle*			Edge::triangle				( size_t index) 
{
	return mTriangles[index];
}
inline const Triangle*		Edge::triangle				( size_t index)const 
{
	return mTriangles[index];
}
inline Triangle*			Edge::otherTriangle			( const Triangle* inTriangle )
{
	if(mTriangles[0] == inTriangle)
		return mTriangles[1];
	else if(mTriangles[1] == inTriangle)
		return mTriangles[0];
	return nullptr;
}
inline const Triangle*		Edge::otherTriangle			( const Triangle* inTriangle )const
{
	if(mTriangles[0] == inTriangle)
		return mTriangles[1];
	else if(mTriangles[1] == inTriangle)
		return mTriangles[0];
	return nullptr;
}
bool						Edge::addTriangle			( Triangle* inTriangle)
{
	if(mTriangles[0] == nullptr)
	{
		mTriangles[0] = inTriangle;
		return true;
	}
	else if(mTriangles[1] == nullptr)
	{
		mTriangles[1] = inTriangle;
		return true;
	}
	return false;
}
bool						Edge::removeTriangle		( Triangle* inTriangle)
{
	if(mTriangles[0] == inTriangle)
	{
		mTriangles[0] = nullptr;
		return true;
	}
	else if(mTriangles[1] == inTriangle)
	{
		mTriangles[1] = nullptr;
		return true;
	}
	return false;	
}
inline bool					Edge::isEndPoint			( const Vertex* inVertex)const
{
	return (mVertices[0] == inVertex ||	mVertices[1] == inVertex);
}
bool						Edge::isNeighbour			( const Triangle* inTriangle)const
{
	return (mTriangles[0] == inTriangle || mTriangles[1] == inTriangle);
}
inline double				Edge::length			    ( )const
{
	return (mVertices[0]->point() - mVertices[1]->point()).magnitude();
}
inline double				Edge::lengthSqr			    ( )const
{
	return (mVertices[0]->point() - mVertices[1]->point()).magnitudeSqr();
}
inline Vector3D				Edge::midPoint				( )const
{
	return (mVertices[0]->point() + mVertices[1]->point())/2.0;
}
