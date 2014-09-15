#include "stdafx.h"
//#include <vld.h>
#include "Domain.h"
#include "OpenglDisplay.h"
#include "Mesher.h"

int main()
{
	Domain domain;
	domain.readPLC("examples\\nicey.zeus");

	Tolerance inTol;
	const BoundingBox3D & box = domain.boundingBox();
	Vector3D diff = box.upper() - box.lower();
	double edgeLen = diff.magnitude()/1000;
	double boxArea = edgeLen*edgeLen/2;

	Criteria criteria;
	//criteria.setRatio(0.888);
	criteria.setRatio(1.0);
	criteria.setMinArea(boxArea);
	criteria.setMaxArea(boxArea*100);
	criteria.setMinEdgeLength(edgeLen);
	criteria.setMaxEdgeLength(edgeLen);

	Mesher triangulator(domain, criteria);
	triangulator.chewDeluanayRefinement();
	//triangulator.ruppertDeluanayRefinement();

	//display code
	OpenglDisplay ogldisplay(&triangulator);
	ogldisplay.init();
	ogldisplay.add_display_list(&triangulator,string("Delauney Triangulation"));
	ogldisplay.add_display_list(triangulator.mHighlightPoint,string("HighlightPoint"));
	ogldisplay.add_display_list(triangulator.mHighlightPolygon,string("HighlightPolygon"));

	//ogldisplay.add_display_list(&triangulator.mDiscretizedPoints,string("Boundry"));
	ogldisplay.display();
	ogldisplay.exec();
	return 1;

	return 1;
}
