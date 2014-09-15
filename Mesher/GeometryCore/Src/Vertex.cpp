#include "StdAfx.h"
#include "Vertex.h"

using namespace GeometryCore;

Vertex::Vertex(Vector3D inPoint)
{
	mPoint = inPoint;
}
Vertex::~Vertex(void)
{
}
inline const Vector3D &	Vertex::point					( )const 
{
	return mPoint;
}
inline void				Vertex::set						( const Vector3D & inPoint)
{
	mPoint.set(inPoint.x(), inPoint.y(), inPoint.z());
}


