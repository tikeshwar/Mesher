#pragma once

class Domain
{
public:
	typedef	vector<Vector3D > PointArray;
	typedef vector<PointArray > Point2DArray;

	typedef	list<Vector3D> PointList;
	typedef list<PointList> Point2DList;

public:
	Domain();
	~Domain();
	
	void readPLC(const char* inputFile);
	void readPSLG(const char* inputFile);

	const PointArray & boundary()const;
	const Point2DArray & holes()const;
	const Point2DArray & constraints()const;
	const PointArray & constraintPoints()const;

	Point2DList & discretizedBoundary();
	const Point2DList & discretizedBoundary()const;
	//Point2DList & discretizedHoles();
//	const Point2DArray & discretizedConstraints()const;

	const BoundingBox3D & boundingBox()const;

	void setBoundary(PointArray inBoundary);
	void setHoles(Point2DArray inHoles);
	void setConstraints(Point2DArray inConstraints);
	void setConstraintPoints(PointArray inConstraintPoints);

private:
	void setData(Point2DArray & inPlclist);
	void discretizePoints(const PointArray & inPlc, PointArray & outPlc, double value);

private:
	PointArray mBoundary;
	Point2DArray mHoles;			
	Point2DArray mConstraints;
	PointArray mConstraintPoints;
	BoundingBox3D mBox;

	Point2DList mDiscretizedBoundary;
};