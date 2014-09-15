#ifndef TRIANGULATOR_H
#define TRIANGULATOR_H

#include "Domain.h"
#include "Criteria.h"
#include "OrderedMap.h"
using namespace GeometryCore;

class Mesher
{	
public:
	typedef	vector<Vector3D>									Vectorlist;
	typedef unordered_set<Triangle*>							TriangleSet;
	typedef unordered_set<Edge*>								EdgeSet;
	typedef unordered_set<Vertex*>								VertexSet;

	typedef pair<BSPTree2D*, BSPTree2D::Pos >					BSPNodeInsidePair;
	typedef map<const Domain::PointArray *, BSPNodeInsidePair>	PLCBSPMap;	
	typedef pair<Vectorlist,Vector3D >							PairPolygonColor;

	typedef OrderedMap<double, Triangle*>						OrderedTriangleSet;
	typedef OrderedMap<double, Edge*>							OrderedEdgeSet;

	Vectorlist						mHighlightPoint;
	list <PairPolygonColor  >		mHighlightPolygon;
	list<Triangle*>					triangleList;
public:
	Mesher(Domain & domain, Criteria & criteria);
	~Mesher();

	void			chewDeluanayRefinement				(   );
	void			ruppertDeluanayRefinement			(   );

	Triangulation*  mesh								(   )const;

	const Domain &	domain								(   )const;
	const Criteria& criteria							(   )const;
	const Tolerance&tolerance							(   )const;

private:

	// initial triangulation
	void			createInitialDelaunay				(	);
	void			createSuperSquare					(   );
	void			insertConstraintPoints				(   );
	void			insertConstraintSegments			(   );
	void			initDomainData						(   );
	
	// ruppert's refinement
	bool			refineBadEdge						(   Edge* inEdge);
	bool			refineBadTriangle					(   Triangle* inTriangle);
	void			ruppertRefinement					(   );
	void			evaluateEdges						(   const EdgeSet & inEdgeSet);
	//void			evaluateEdges						(   const Triangulation::EdgeSet & inEdgeSet);
	bool			isEdgeEncroaching					(   const Edge* inEdge)const;
	void			updateEdgeSet						(   TriangleSet & trianglesCreated,
															TriangleSet & trianglesDeleted);

	// chew's refinement
	void			chewRefinement						(	);
	bool			isFixedEdgeEncroached				(	Triangle* inTriangle, Edge** outEdge);
	void			findCircumCentersInDiametralDisk	(   Edge* diametralEdge,
															VertexSet & outVertices);
	void			removeVertices						(	VertexSet & vertexSet);
	void			removeVertex						(   Vertex* inVertex);															


	// mesh smoothing may reduce the smallest angle
	void			laplacianSmoothing					(  	);
	void			angleBasedSmoothing					(	);

	// refinement criteria
	void			evaluateByRatio						(	const TriangleSet & inTriangleSet);
	void			evaluateByArea						(	const TriangleSet & inTriangleSet);
	void			evaluateByEdgeLen					(	const TriangleSet & inTriangleSet);

	//void			evaluateByRatio						(	const Triangulation::TriangleSet & inTriangleSet);
	//void			evaluateByArea						(	const Triangulation::TriangleSet & inTriangleSet);
	//void			evaluateByEdgeLen					(	const Triangulation::TriangleSet & inTriangleSet);

	// common apis
	void			removeExteriorTriangles				(	);
	bool			recoverEdge							(   Vertex* v1, Vertex* v2);
	Vertex*			splitFixedEdge						(   Edge* constrainedEdge, 
															Vector3D & insertionPoint);

	void			triangulatePolygon					(   list<Vertex*> & inPolygon,
															TriangleSet & outTriangleSet);
	void			vertexInsertion						(   Vertex* node, Vertex* next, Vertex* prev,
															TriangleSet & outTrianglesCreated,
															TriangleSet & outTrianglesDeleted);

	bool			pointInPolygon						(   const Vector3D & inPoint)const;
	void			createBSP							(   );
	void			removeExteriorTriangles				(	const Domain::PointArray & inPointList);
	bool			isInDiametricalCircle				(   const Vector3D & edgeP1, 
															const Vector3D & edgeP2, 
															const Vector3D & inPoint)const;

	bool			watsonDelaunay						(	const Vector3D & inPoint,
															Triangle* referenceTriangle = nullptr);	
	bool			watsonDelaunay						(	const Vector3D & inPoint,	
															TriangleSet & trianglesDeleted,
															TriangleSet & trianglesCreated,
															Triangle* referenceTriangle = nullptr);
	void			removeTriangles						(	TriangleSet & tmpSet,
															bool flag = true);

	void			areaRefinement						(   );
	void			edgeRefinement						(   );



	bool			hopelessTriangle					(   Triangle* inTriangle);
	bool			isVisible							(   const Vector3D & inPoint1,
															const Vector3D & inPoint2,
															Edge** fixedEdge = nullptr);

	Triangle*		locateTriangle						(	const Vector3D & inPoint, 
															Triangle* inTriangle = nullptr);
	void			findCircumScribingNeigbours			(	const Vector3D & inPoint,
															Triangle * inTriangle,
															TriangleSet & inTriangleSet,
															bool isSegmentToSplit = false);
	void			constrainedCircumScribingTriangles	(	const Vector3D & inPoint,
															Edge * inEdge,
															TriangleSet & inTriangleSet);
	void			circumScribingTriangles				(	const Vector3D & inPoint,
															Edge * inEdge,
															TriangleSet & inTriangleSet);
	void			triangleWrapper						(	TriangleSet & inTriangleSet,
															vector<Edge*> & outEdges)const;
	void			refineTriangle						(   Triangle* inTriangle,
															TriangleSet & outTrianglesCreated,
															TriangleSet & outTrianglesDeleted);
	void			updateTriangleSet					(   TriangleSet & inTrianglesCreated,
															TriangleSet & inTrianglesDeleted);
	void			updateBadTriangles					(   TriangleSet & inTrianglesSet);

	void			handleZeroAreaTriangle				(   Triangle* triangle,
															TriangleSet & outTriangleCreated,
															TriangleSet & outTriangleDeleted);
	Vector3D		offCenterPoint						(   Triangle* inTriangle)const;
	Vector3D		concentricShellPoint				(	const Vector3D & edgeP1,
															const Vector3D & edgeP2)const;
	Vector3D		splitContraintEdge					(   Edge* inEdge);
	double			findMinimumAngle					(	)const;
	double			findMinEdgeLength					(   )const;

	void			showTriangle						(   Triangle* inTriangle);
	void			showEdge							(   Edge* inEdge);
	
private:
	Criteria				*mCriteria;
	Domain					*mDomain;

	PLCBSPMap				*mPLCBSPMap;
	EdgeSet					*mFixedEdges;
	VertexSet				*mFixedVertices;
	VertexSet				*mOffCenters;
	OrderedTriangleSet		*mBadTriangleSet; 
	OrderedEdgeSet			*mBadEdgesSet; 
	Triangulation			*mTriangulation;

	Tolerance				*mTol;
	friend class OpenglDisplay;
};

#endif // TRIANGULATOR_H