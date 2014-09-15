#include "System.h"
#include "Domain.h"

Domain::Domain()
{
}
Domain::~Domain()
{
}
void Domain::readPLC(const char* inputFile)
{
	ifstream file(inputFile);
	if(!file.is_open())
		std::cout << "file can not be opened" << endl;
	
	string text;
	Point2DArray plclist;
	while(!file.eof())
	{
		file >> text;
		if(text == "PLC")
		{
			bool closedLoop;
			size_t size;
			PointArray plc;
			file >> closedLoop >> size;
			while(size--)
			{
				double x,y,z;
				file >> x >> y >> z;
				plc.push_back(Vector3D(x,y,z));
			}

			if(closedLoop && !(plc[0]== plc[plc.size()-1]))
				plc.push_back(plc[0]);

			plclist.push_back(plc);
		}
		else if(text == "END")
		{
			break;
		}
	}

	setData(plclist);
}
void Domain::readPSLG(const char* inputFile)
{
}
void Domain::setData(Point2DArray & inPlclist)
{
	// setting bounding box
	for(auto itPlc = inPlclist.begin(); itPlc != inPlclist.end(); itPlc++)
	{
		for(auto itPoint = itPlc->begin(); itPoint != itPlc->end(); itPoint++)
			mBox.add(*itPoint);
	}

	map<PointArray *, double> plcNBoxMap;
	for(auto itPlc = inPlclist.begin(); itPlc != inPlclist.end(); itPlc++)
	{
		if(itPlc->front() == itPlc->back())
		{
			BoundingBox3D box;
			for(auto itPoint = itPlc->begin(); itPoint != itPlc->end(); itPoint++)
				box.add(*itPoint);

			double diagonal = (box.upper() - box.lower()).magnitude();
			plcNBoxMap.insert(map<PointArray *, double>::value_type(itPlc._Ptr, diagonal));
		}
		else
			mConstraints.push_back(*itPlc);
	}

	double max = 0;
	for(auto it = plcNBoxMap.begin(); it != plcNBoxMap.end(); it++)
	{
		if(it->second > max)
		{
			max = it->second;
			mBoundary = *it->first;
		}
	}

	for(auto it = plcNBoxMap.begin(); it != plcNBoxMap.end(); it++)
	{
		auto & hole = *it->first;
		if(mBoundary == hole)
			continue;
		if(hole.size() == 1)
			mConstraintPoints.push_back(hole[0]);
		else
			mHoles.push_back(hole);
	}

	// creating discrete boundary list
	PointList boundry(mBoundary.begin(),mBoundary.end());
	mDiscretizedBoundary.push_back(boundry);

	// creating discrete holes list
	for(auto it = mHoles.begin(); it != mHoles.end(); it++)
	{
		PointList holeList(it->begin(), it->end());
		mDiscretizedBoundary.push_back(holeList);
	}
	
	// creating discrete constraint list
	for(auto it = mConstraints.begin(); it != mConstraints.end(); it++)
	{
		PointList constrainList(it->begin(), it->end());
		mDiscretizedBoundary.push_back(constrainList);
	}
}
void Domain::discretizePoints(const PointArray & inPlc, PointArray & outPlc, double value)
{
	// putting points in between the segments 
	for(auto itPoint = inPlc.begin(); itPoint != --inPlc.end(); )
	{
		const Vector3D & p1 = *itPoint;
		const Vector3D & p2 = *(++itPoint);

		double distance = p1.distance(p2);
		size_t numofpts = (size_t)(distance/value)+1;
		if(numofpts > 1)
		{
			double u = 1/(double)(numofpts);
			for(size_t j = 0; j < numofpts; j++)
			{
				double t = u*j;
				Vector3D pt = p1*(1 - t) + p2*t;
				outPlc.push_back(pt);
			}
		}
		else
		{
			outPlc.push_back(p1);
		}
	}
	outPlc.push_back(inPlc.back());
}
void Domain::setBoundary(PointArray inBoundary)
{
	mBoundary = inBoundary;
}
void Domain::setHoles(Point2DArray inHoles)
{
	mHoles = inHoles;
}
void Domain::setConstraints(Point2DArray inConstraints)
{
	mConstraints.resize(inConstraints.size());
	copy(inConstraints.begin(), inConstraints.end(), mConstraints.begin());
}
void Domain::setConstraintPoints(PointArray inConstraintPoints)
{
	mConstraintPoints.resize(inConstraintPoints.size());
	copy(inConstraintPoints.begin(), inConstraintPoints.end(), mConstraintPoints.begin());
}
const Domain::PointArray & Domain::boundary()const
{
	return mBoundary;
}
const Domain::Point2DArray & Domain::holes()const
{
	return mHoles;
}
const Domain::Point2DArray & Domain::constraints()const
{
	return mConstraints;
}
const Domain::PointArray & Domain::constraintPoints()const
{
	return mConstraintPoints;
}
const BoundingBox3D & Domain::boundingBox()const
{
	return mBox;
}
Domain::Point2DList & Domain::discretizedBoundary()
{
	return mDiscretizedBoundary;
}
const Domain::Point2DList & Domain::discretizedBoundary()const
{
	return mDiscretizedBoundary;
}
/*
Domain::Point2DList & Domain::discretizedHoles()
{
	//if(mDiscretizedHoles.size())
		return mDiscretizedHoles;
	//return mHoles;
}
const Domain::Point2DArray & Domain::discretizedConstraints()const
{
	if(mDiscretizedConstraints.size())
		return mDiscretizedConstraints;
	return mConstraints;
}
*/
