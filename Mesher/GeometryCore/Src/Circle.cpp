#include "stdafx.h"
#include "Circle.h"

Circle::Circle(Vector3D inCenter, double inRadius)
	:mCenter(inCenter), mRadius(inRadius)
{
}
Circle::~Circle()
{
}
const Vector3D & Circle::center				( )const
{
	return mCenter;
}
double Circle::radius						( )const
{
	return mRadius;
}
bool Circle::isInside						( const Vector3D & inPoint,
											  const Tolerance inTol)const
{
	return inTol.isLess((inPoint - mCenter).magnitudeSqr(),mRadius*mRadius);
}