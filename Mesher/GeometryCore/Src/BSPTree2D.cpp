#include "StdAfx.h"
#include "BSPTree2D.h"
#include <assert.h>

using namespace GeometryCore;
static Tolerance mTol;
BSPTree2D* BSPTree2D::create( const PointList& inPointList, std::string& outState, double inTol /*= 1.0e-13*/, size_t inMaxNodeCount /*= 7000*/, size_t numAxialDivsn /*= 0*/ )
{
	Segment::List seglist;
	PointList::const_iterator itrP1 = inPointList.begin();
	PointList::const_iterator itrP2 = inPointList.begin();
	for(itrP2++;itrP2 != inPointList.end();itrP1++,itrP2++)
		seglist.push_back(Segment(*itrP1,*itrP2));
	outState.clear();
	Node* baseNode = new Node(seglist,outState,inTol,inMaxNodeCount,numAxialDivsn);
	if(!outState.empty())
	{
		delete baseNode;
		return nullptr;
	}
	return new BSPTree2D(baseNode,inTol);
}

BSPTree2D* BSPTree2D::create( const PointArray& inPointArray, std::string& outState, double inTol /*= 1.0e-13*/, size_t inMaxNodeCount /*= 7000*/, size_t numAxialDivsn /*= 0*/ )
{
	Segment::List seglist;
	for(auto itPoint = inPointArray.begin(); itPoint != --inPointArray.end();)
	{
		auto & p1 = *itPoint, & p2 = *(++itPoint);
		seglist.push_back(Segment(p1, p2));
	}
	outState.clear();
	Node* baseNode = new Node(seglist,outState,inTol,inMaxNodeCount,numAxialDivsn);
	if(!outState.empty())
	{
		delete baseNode;
		return nullptr;
	}
	return new BSPTree2D(baseNode,inTol);
}
BSPTree2D::Pos		BSPTree2D::pointPosition	( const Vector3D&					inPoint)const
{
	if (!mBaseNode)
		return UNKNOWN_POS;
	return mBaseNode->pointPosition(inPoint,mTolerance);
}
bool BSPTree2D::Segment::findInterSection	( Segment&		inLine, 
												  Vector3D&	outP,
												  double		inTol)
{
	//double a1 = SIGNED_AREA(mP1, mP2, inLine.mP2);		// Compute winding of abd (+ or -)
	//double a2 = SIGNED_AREA(mP1, mP2, inLine.mP1);		// To intersect, must have sign opposite of a1
	//
	//if(fabs(a1)<inTol || fabs(a2)<inTol)
	//	return false;
	//
	//if(a1 * a2 < 0.0)
	//{
	//	double a3 = SIGNED_AREA(inLine.mP1, inLine.mP2, mP1);	// Compute winding of cda (+ or -)
	//	double a4 = SIGNED_AREA(inLine.mP1, inLine.mP2, mP2);	// a3 + a2 - a1;
	//	
	//	if(a3 * a4 < 0.0)
	//	{
	//		double t = a3 / (a3 - a4);
	//		outP = mP1 + t * (mP2 - mP1);
	//		return true;
	//		if(&outT1)
	//			outT1 = a1 / (a1 - a2);
	//		if(&outT2)
	//			outT2 = a3 / (a3 - a4);
	//		return CGTrue;
	//	}
	//}

	//return CGFalse;

	double t;
	double a1 = SIGNED_AREA(mP1, mP2, inLine.mP1)*2.0; // Compute winding of abd (+ or -)
	double a2 =	SIGNED_AREA(mP1, mP2, inLine.mP2)*2.0; // To intersect, must have sign opposite of a1

	if(fabs(a1)<inTol || fabs(a2)<inTol)
		return false;

	if ( a1 * a2 < 0)
	{
		double a3 = 2.0*SIGNED_AREA(inLine.mP1, inLine.mP2, mP1); // Compute winding of cda (+ or -)
		double a4 = 2.0*SIGNED_AREA(inLine.mP1, inLine.mP2, mP2);
		t = a3 / (a3 - a4);
		outP = mP1 + t * (mP2 - mP1);
		return true;
	}
	return false;// Segments not intersecting (or collinear)
}
BSPTree2D::Pos BSPTree2D::Segment::split( Segment& inLine, Segment& outLeftSegment, Segment& outRightSegment, double inTol )
{
	Pos pos1 = inLine.classifyPoint(mP1,inTol);
	Pos pos2 = inLine.classifyPoint(mP2,inTol);
	if (pos1 == pos2)			return pos1;
	if (pos1 == COLLINEAR)		return pos2;
	if (pos2 == COLLINEAR)		return pos1;
	
	Vector3D intPt;
	bool rt = inLine.findInterSection(*this,intPt,inTol); assert(rt);
	if(pos1 == LEFTSIDE)
	{
		outLeftSegment.mP1 = mP1;
		outLeftSegment.mP2 = intPt;
		outRightSegment.mP1 = intPt;
		outRightSegment.mP2 = mP2;
		return STRADDLING;
	}
	else
	{
		outRightSegment.mP1 = mP1;
		outRightSegment.mP2 = intPt;
		outLeftSegment.mP1 = intPt;
		outLeftSegment.mP2 = mP2;
		return STRADDLING;
	}
}

BSPTree2D::Pos BSPTree2D::Segment::classifyPoint( const Vector3D& inParamVector, double inTol )
{
	double area = SIGNED_AREA(mP1,mP2,inParamVector);
	if(fabs(area)<inTol)return COLLINEAR;
	if(area>inTol)return LEFTSIDE;
	return RIGHTSIDE;
}

BSPTree2D::Node::Node( Segment::List& inSegmentList, std::string& outState, double inTol, size_t inMaxDepth, size_t inNumMidPlaneDivsn /*= 0*/ )
	:mSeg(nullptr)
{
	mChild[LEFT] = nullptr;
	mChild[RIGHT] = nullptr;
	if(!inMaxDepth)
	{
		outState = "BSP Build Fail Due To high No. Of Segment ";
		return;
	}

	// Select best possible partitioning plane based on the input geometry
	genSplittingLine(inSegmentList,inTol,inNumMidPlaneDivsn);

	Segment::List leftList, rightList;
	// Test each polygon against the dividing plane, adding them
	// to the front list, back list, or both, as appropriate
	for(Segment::List::iterator itrSeg = inSegmentList.begin();
		itrSeg != inSegmentList.end(); itrSeg++) 
	{
		Segment leftPart,rightPart;
		switch (itrSeg->split(*mSeg,leftPart,rightPart,inTol))
		{
		case COLLINEAR:		break;
			// Whats done in this case depends on what type of tree is being
			// built. For a node-storing tree, the polygon is stored inside
			// the node at this level (along with all other polygons coplanar
			// with the plane). Here, for a leaf-storing tree, coplanar polygons
			// are sent to either side of the plane. In this case, to the front
			// side, by falling through to the next case
		case LEFTSIDE:
			leftList.push_back(*itrSeg);
			break;

		case RIGHTSIDE:
			rightList.push_back(*itrSeg);
			break;

		case STRADDLING:									
			// Split polygon to plane and send a part to each side of the plane
			leftList.push_back(leftPart);
			rightList.push_back(rightPart);
			break;
		default:	break;	
		}
	}

	//if (inPolygonList.size() <= _DT_BSP_MIN_POLYGON_)
	//{			
	//	// If criterion for a leaf is matched, create a leaf node from remaining polygons
	//		 
	//	//return new FacetBSPNode(polygons);
	//}

	// Recursively build child subtrees and return new tree root combining them
	size_t numMidPlaneDivsn = inNumMidPlaneDivsn>0?inNumMidPlaneDivsn-1:0;
	if(leftList.size())
		mChild[LEFT] = new Node(leftList,outState,inTol,inMaxDepth-1,numMidPlaneDivsn);
	if (!outState.empty())
		return;

	if(rightList.size())
		mChild[RIGHT] = new Node(rightList,outState,inTol,inMaxDepth-1,numMidPlaneDivsn);
	if (!outState.empty())
		return;
}

BoundingBox3D BSPTree2D::Node::findBoundingBox( Segment::List& inSegmentList )
{
	BoundingBox3D bbox;
	for(Segment::List::iterator segmentItr = inSegmentList.begin();
		segmentItr != inSegmentList.end(); segmentItr++) 
	{
		bbox.add(segmentItr->point1());
		bbox.add(segmentItr->point2());
	}
	return bbox;
}

void BSPTree2D::Node::genSplittingLine( Segment::List& inSegmentList, double inTol, size_t inNumMidPlaneDivsn /*= 0*/ )
{
	assert(inSegmentList.size());
	if(inNumMidPlaneDivsn > 0)	
		return genMidLine(findBoundingBox(inSegmentList),(MidLineType)(inNumMidPlaneDivsn%2));

	Segment seg = inSegmentList.back();
	inSegmentList.pop_back();
	if(seg.point1().distance(seg.point2()) < inTol)
	{
		if(inSegmentList.size())
			return genSplittingLine(inSegmentList,inTol);
		assert(0);//"Huge problem in seg magnitude"
		return;
	}
	mSeg = new Segment(seg);
}

void BSPTree2D::Node::genMidLine( BoundingBox3D inBBox, MidLineType inMidPlaneType )
{
	Vector3D centroid = (inBBox.upper()+inBBox.lower())/2.0;
	Vector3D nextPt;
	switch(inMidPlaneType)
	{
	case PARALLEL_X:
		nextPt = centroid + Vector3D(1.0,0.0,0.0);
		break;
	case PARALLEL_Y:
	default:
		nextPt = centroid + Vector3D(0.0,1.0,0.0);
		break;
	}
	mSeg = new Segment(centroid,nextPt);
}

BSPTree2D::Pos BSPTree2D::Node::pointPosition( const Vector3D& inPoint, double inTol ) const
{
	// Compute distance of point to dividing plane
	Pos pointPos = mSeg->classifyPoint(inPoint,inTol);

	// Traverse front of tree when point in front of plane, else back of tree
	switch(pointPos)
	{
	case COLLINEAR:
		{
			Pos leftSide = mChild[LEFT]?mChild[LEFT]->pointPosition(inPoint,inTol):LEFTSIDE;
			Pos rightSide =  mChild[RIGHT]?mChild[RIGHT]->pointPosition(inPoint,inTol):RIGHTSIDE;
			return (leftSide == rightSide)?leftSide:COLLINEAR;
		}
	case LEFTSIDE:
		return mChild[LEFT]?mChild[LEFT]->pointPosition(inPoint,inTol):LEFTSIDE;
	case RIGHTSIDE:
		return mChild[RIGHT]?mChild[RIGHT]->pointPosition(inPoint,inTol):RIGHTSIDE;
	default: assert(0);
	}
	return UNKNOWN_POS;
}
