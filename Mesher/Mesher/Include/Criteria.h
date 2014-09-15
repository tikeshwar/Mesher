#pragma once

class Criteria
{
public:
	Criteria();
	~Criteria();
	
	void setMinEdgeLength(double inMinEdgeLength);
	void setMaxEdgeLength(double inMaxEdgeLength);
	void setMinArea(double inMinArea);
	void setMaxArea(double inMaxArea);
	void setRatio(double inRatio);

	double minEdgeLength()const;
	double maxEdgeLength()const;
	double minArea()const;
	double maxArea()const;
	double ratio()const;
	double minAngle()const;
private:
	double mMinEdgeLength;
	double mMaxEdgeLength;
	double mMinArea;
	double mMaxArea;
	double mRatio;
};