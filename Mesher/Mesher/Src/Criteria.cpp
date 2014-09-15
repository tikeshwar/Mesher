#include "Criteria.h"
#include <cmath>

Criteria::Criteria()
{
	mMinEdgeLength = -1;
	mMaxEdgeLength = -1;
	mMinArea = -1;
	mMaxArea = -1;
	mRatio = -1;
}
Criteria::~Criteria()
{
}
void Criteria::setMinEdgeLength(double inMinEdgeLength)
{
	mMinEdgeLength = inMinEdgeLength;
}
void Criteria::setMaxEdgeLength(double inMaxEdgeLength)
{
	mMaxEdgeLength = inMaxEdgeLength;
}
void Criteria::setMinArea(double inMinArea)
{
	mMinArea = inMinArea;
}
void Criteria::setMaxArea(double inMaxArea)
{
	mMaxArea = inMaxArea;
}
void Criteria::setRatio(double inRatio)
{
	mRatio = inRatio;
}
double Criteria::minEdgeLength()const
{
	return mMinEdgeLength;
}
double Criteria::maxEdgeLength()const
{
	return mMaxEdgeLength;
}
double Criteria::minArea()const
{
	return mMinArea;
}
double Criteria::maxArea()const
{
	return mMaxArea;
}
double Criteria::ratio()const
{
	return mRatio;
}
double Criteria::minAngle()const
{
	return asin(1.0/(2*mRatio));
}