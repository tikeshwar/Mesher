#include "stdafx.h"
#include "MesherUtility.h"
#include "CircularList.h"
#include "Mesher.h"

static Triangle* lastTriangle = nullptr;
static Edge* constraintEdge = nullptr;

Mesher::Mesher										(Domain & domain, Criteria & criteria):
mCriteria(new Criteria(criteria)),
mDomain(new Domain(domain)),
mPLCBSPMap(new PLCBSPMap),
mFixedEdges(new EdgeSet),
mFixedVertices(new VertexSet),
mOffCenters(new VertexSet),
mBadTriangleSet(new  OrderedTriangleSet),
mBadEdgesSet(new OrderedEdgeSet), 
mTriangulation(new Triangulation()),
mTol(new Tolerance())
{
	mTol->zero() = mTol->tol() = 1e-10;
	createBSP();
}
void Mesher::chewDeluanayRefinement					(   )
{
	createInitialDelaunay();
	edgeRefinement();
	//chewRefinement();
	//removeExteriorTriangles();
	mTriangulation;
}
void Mesher::ruppertDeluanayRefinement				(   )
{
	createInitialDelaunay();
	//edgeRefinement();
	ruppertRefinement();
	//ruppertRefinement();
	removeExteriorTriangles();
}
Triangulation* Mesher::mesh							(   )const
{
	return mTriangulation;
}
const Domain & Mesher::domain						(   )const
{
	return *mDomain;
}
const Criteria & Mesher::criteria					(   )const
{
	return *mCriteria;
}
const Tolerance &	Mesher::tolerance				(   )const
{
	return *mTol;
}
Mesher::~Mesher()
{
	mPLCBSPMap->clear();
	mFixedEdges->clear();
	mFixedVertices->clear();
	mOffCenters->clear();

	if(mCriteria)
		delete mCriteria;
	if(mDomain)
		delete mDomain;
	if(mPLCBSPMap)
		delete mPLCBSPMap;
	if(mFixedEdges)
		delete mFixedEdges;
	if(mFixedVertices)
		delete mFixedVertices;
	if(mOffCenters)
		delete mOffCenters;
	if(mBadTriangleSet)
		delete mBadTriangleSet;
	if(mBadEdgesSet)
		delete mBadEdgesSet; 
	if(mTriangulation)
		delete mTriangulation;
	if(mTol)
		delete mTol;
}
void Mesher::createInitialDelaunay					(	)
{
	createSuperSquare();
	insertConstraintPoints();
	insertConstraintSegments();
	initDomainData();
}
void Mesher::createSuperSquare						(   )
{
	auto box = mDomain->boundingBox();
	Vector3D diff = (box.upper() - box.lower())/2.0;
	box.add(box.lower() - diff);
	box.add(box.upper() + diff);

	diff = box.upper() - box.lower();
	double maxlen = max(diff.x(), diff.y())/2.0;

	auto centroid = box.centroid();
	auto v1 = centroid + Vector3D(-maxlen, -maxlen, 0);
	auto v2 = centroid + Vector3D(maxlen, -maxlen, 0);
	auto v3 = centroid + Vector3D(maxlen, maxlen, 0);
	auto v4 = centroid + Vector3D(-maxlen, maxlen, 0);

	auto vertex1 = mTriangulation->addVertex(v1);
	auto vertex2 = mTriangulation->addVertex(v2);
	auto vertex3 = mTriangulation->addVertex(v3);
	auto vertex4 = mTriangulation->addVertex(v4);

	auto triangle1 = mTriangulation->addTriangle(vertex1, vertex2, vertex3);
	auto triangle2 = mTriangulation->addTriangle(vertex3, vertex4, vertex1);
}
void Mesher::insertConstraintPoints					(   )
{
	TriangleSet trianglesCreated, trianglesDeleted;
	for(auto itBoundry = mDomain->discretizedBoundary().begin(); itBoundry != mDomain->discretizedBoundary().end(); itBoundry++)
	{
		for(auto itPoint = itBoundry->begin(); itPoint != itBoundry->end(); itPoint++)
		{
			trianglesCreated.clear();
			trianglesDeleted.clear();
			watsonDelaunay(*itPoint, trianglesDeleted, trianglesCreated);
			removeTriangles(trianglesDeleted);
		}
	}
}
void Mesher::insertConstraintSegments				(   )
{
	for(auto itBoundry = mDomain->discretizedBoundary().begin(); itBoundry != mDomain->discretizedBoundary().end(); itBoundry ++)
	{
		for(auto itPoint = itBoundry->begin(); itPoint != --itBoundry->end();)
		{
			auto & p1 = *itPoint;
			auto & p2 = *(++itPoint);
			auto vertex1 = mTriangulation->find(p1);
			auto vertex2 = mTriangulation->find(p2);
			if(vertex1 == nullptr || vertex2 == nullptr)
				return;
			auto edge = mTriangulation->find(vertex1, vertex2);
			if(!edge)
				recoverEdge(vertex1, vertex2);
		}
	}
}
void Mesher::initDomainData							(   )
{
	// init fixed edges
	if(!mFixedEdges->size())
	{
		for(auto itPlc = mDomain->discretizedBoundary().begin();
			itPlc != mDomain->discretizedBoundary().end(); itPlc++)
		{
			for(auto itPoint = itPlc->begin(); itPoint != --itPlc->end();)
			{
				auto & p1 = *itPoint;
				auto & p2 = *(++itPoint);
				auto v1 = mTriangulation->addVertex(p1);
				auto v2 = mTriangulation->addVertex(p2);
				auto edge = mTriangulation->addEdge(v1, v2);
				if(edge != nullptr)
					mFixedEdges->insert(edge);
			}
		}
	}

	// init fixed vertices
	if(!mFixedVertices->size())	
	{
		for(auto itPlc = mDomain->discretizedBoundary().begin();
			itPlc != mDomain->discretizedBoundary().end(); itPlc++)
		{
			for(auto itPoint = itPlc->begin(); itPoint != itPlc->end(); itPoint++)
				mFixedVertices->insert(mTriangulation->find(*itPoint));
		}
	}
}
void Mesher::removeExteriorTriangles				(	)
{
	Triangulation::TriangleSet trglSet;
	mTriangulation->triangles(trglSet); 

	TriangleSet exteriorTriangles;
	for(auto it = trglSet->begin(); it != trglSet->end(); it++)
	{
		if(!pointInPolygon((*it)->centroid()))
			exteriorTriangles.insert(*it);
	}

	removeTriangles(exteriorTriangles);
	//for(auto it = exteriorTriangles.begin(); it != exteriorTriangles.end(); it++)
	//	mTriangulation->removeTriangle(*it);
}
bool Mesher::recoverEdge							(   Vertex * inVertex1,  Vertex * inVertex2)
{
	// step1 : find the reference triangle which is 
	// intersected by the edge
	Triangle* refTriangle = nullptr;
	{
		list<Triangle*> triangleList;
		list<Edge*>	edgeList;
		inVertex1->neighbours(triangleList, edgeList);

		for(auto it = triangleList.begin(); it != triangleList.end(); it++)
		{
			auto triangle = *it;
			auto oppEdge = triangle->opposite(inVertex1);
			
			double t1 = mTol->inf(), t2 = mTol->inf();
			MesherUtility::IntersectionType type = MesherUtility::intersect(inVertex1->point(), inVertex2->point(), oppEdge->vertex(0)->point(), oppEdge->vertex(1)->point(), t1, t2, *mTol);
			if(type == MesherUtility::Intersecting)
			{
				refTriangle = triangle;
				break;
			}
		}

	}

	// step2 : find the other triangles which intersect the edge
	TriangleSet triangleDeleted;
	triangleDeleted.insert(refTriangle);

	list<Vertex*> upperPolygon, lowerPolygon;
	upperPolygon.push_back(inVertex1);
	lowerPolygon.push_back(inVertex1);
	auto oppEdge = refTriangle->opposite(inVertex1);
	auto triangle = oppEdge->otherTriangle(refTriangle);
	while(1)
	{		
		if(!triangle)
			return false;
		triangleDeleted.insert(triangle);
		{
			if(MesherUtility::isCounterClockwise(inVertex1->point(), inVertex2->point(), oppEdge->vertex(0)->point(), *mTol)
				&& ( !upperPolygon.size() || upperPolygon.back() != oppEdge->vertex(0)))
				upperPolygon.push_back(oppEdge->vertex(0));
			else if(!MesherUtility::isCounterClockwise(inVertex1->point(), inVertex2->point(), oppEdge->vertex(0)->point(), *mTol)
				&& ( !lowerPolygon.size() || lowerPolygon.back() != oppEdge->vertex(0)))
				lowerPolygon.push_back(oppEdge->vertex(0));

			if(MesherUtility::isCounterClockwise(inVertex1->point(), inVertex2->point(), oppEdge->vertex(1)->point(), *mTol)
				&& ( !upperPolygon.size() || upperPolygon.back() != oppEdge->vertex(1)))
				upperPolygon.push_back(oppEdge->vertex(1));
			else if(!MesherUtility::isCounterClockwise(inVertex1->point(), inVertex2->point(), oppEdge->vertex(1)->point(),*mTol)
 				&& ( !lowerPolygon.size() || lowerPolygon.back() != oppEdge->vertex(1)))
				lowerPolygon.push_back(oppEdge->vertex(1));

		}

		// stopping criteria;
		if(triangle->opposite(oppEdge) == inVertex2)
		{
			upperPolygon.push_back(inVertex2);
			lowerPolygon.push_back(inVertex2);
			break;
		}

		for(size_t i = 0; i < 3; i++)
		{
			auto edge = triangle->edge(i);
			if(edge == oppEdge)
				continue;

			double t1 = mTol->inf(), t2 = mTol->inf();
			MesherUtility::IntersectionType type = MesherUtility::intersect(inVertex1->point(), inVertex2->point(), edge->vertex(0)->point(), edge->vertex(1)->point(), t1, t2, *mTol);
			if( type == MesherUtility::Intersecting ) 
			{
				oppEdge = edge;
				triangle = oppEdge->otherTriangle(triangle);
				break;
			}
		}
	}

	for(auto it = triangleDeleted.begin(); it != triangleDeleted.end(); it++)
		mTriangulation->removeTriangle(*it, false);

	TriangleSet triangleCreated;
	triangulatePolygon(upperPolygon, triangleCreated);
	triangleCreated.clear();
	triangulatePolygon(lowerPolygon, triangleCreated);

	removeTriangles(triangleDeleted);

	return true;
}
void Mesher::removeVertices							(	VertexSet & vertexSet)
{
	for(auto itvtx = vertexSet.begin(); itvtx != vertexSet.end(); itvtx++ )
		removeVertex(*itvtx);
}
void Mesher::removeVertex							(   Vertex* inVertex)
{
	// ordered triangles
	list<Triangle*> orderedTriangles;
	list<Edge*> orderedEdges;
	mTriangulation->neighbours(inVertex, orderedTriangles, orderedEdges);
	for(auto it = orderedTriangles.begin(); it != orderedTriangles.end(); it++)
		mTriangulation->removeTriangle(*it, false);

	list<Vertex*> orderedVertices;
	for(auto it = orderedEdges.begin(); it != orderedEdges.end(); it++)
		orderedVertices.push_back((*it)->otherVertex(inVertex));

	TriangleSet trianglesCreated;
	triangulatePolygon(orderedVertices, trianglesCreated);

	TriangleSet trianglesDeleted;
	for(auto it = orderedTriangles.begin(); it != orderedTriangles.end(); it++)
	{
		if(*it != nullptr)
			trianglesDeleted.insert(*it);
	}


	//removeTriangles(trianglesDeleted);
	for(auto it = trianglesDeleted.begin(); it != trianglesDeleted.end(); it++)
	{
 		for(size_t i = 0; i < 3; i++)
		{
			if(	(mFixedEdges->find((*it)->edge(i)) != mFixedEdges->end()) && 
				((*it)->edge(i)->triangle(0) == nullptr || (*it)->edge(i)->triangle(1) == nullptr))
				mFixedEdges->erase((*it)->edge(i));
		}
	}
	updateBadTriangles(trianglesDeleted);
	removeTriangles(trianglesDeleted);
	evaluateByRatio(trianglesCreated);
}
Vertex* Mesher::splitFixedEdge						(   Edge* constrainedEdge,
														Vector3D & insertionPoint)
{
	auto insertionVertex = mTriangulation->addVertex(insertionPoint);
	if(insertionVertex->neighbors() == nullptr)
		return nullptr;
	auto edge1 = mTriangulation->addEdge(constrainedEdge->vertex(0), insertionVertex);
	auto edge2 = mTriangulation->addEdge(constrainedEdge->vertex(1), insertionVertex);

	if(edge1)
		mFixedEdges->insert(edge1);
	if(edge2)
		mFixedEdges->insert(edge2);

	mFixedEdges->erase(constrainedEdge);
	mFixedVertices->insert(insertionVertex);
	mOffCenters->insert(insertionVertex);
	return insertionVertex;
}
void Mesher::removeExteriorTriangles				(	const Domain::PointArray & inPointList)
{
	auto & bspNode = mPLCBSPMap->find(&inPointList);
	auto bsp = bspNode->second.first;

	vector<Triangle*> exteriorTriangles;
	auto trglSet = mTriangulation->triangles(); 
	for(auto it = trglSet->begin(); it != trglSet->end(); it++)
	{
		Vector3D centroid = (*it)->centroid();
		auto & node = mPLCBSPMap->find(&inPointList);
		auto bsp = node->second.first;
		auto refPos = node->second.second;
		if(bsp)
		{
			BSPTree2D::Pos pos = bsp->pointPosition(centroid);
			if(pos == refPos)
				exteriorTriangles.push_back(*it);
		}
	}

	for(auto it = exteriorTriangles.begin(); it != exteriorTriangles.end(); it++)
		mTriangulation->removeTriangle(*it);	
}
void Mesher::triangulatePolygon						(   list<Vertex*> & inPolygon, TriangleSet & outTriangleSet)
{
	if(!MesherUtility::isCounterClockwise(inPolygon))
		inPolygon.reverse();

	if(inPolygon.front() == inPolygon.back())
		inPolygon.erase(--inPolygon.end());

	CircularList<Vertex*> clist;
	for(auto it = inPolygon.begin(); it != inPolygon.end(); it++)
		clist.insert(*it);

	list<Node<Vertex*>*> deletedNode;
	size_t i = clist.size();
	auto node = clist.start()->next();
	
	i = clist.size();
	while(i-- > 3)
	{
		size_t j = 0;
		while(j++ < clist.size())
		{
			if(MesherUtility::isCounterClockwise(node->data()->point(),node->next()->data()->point(), node->prev()->data()->point(), *mTol)
				&& !MesherUtility::isCollinear(node->data()->point(),node->next()->data()->point(), node->prev()->data()->point(), *mTol))
			{
				break;
			}
			else
				node = node->next();
		}
		node->prev()->setNext(node->next());
		node->next()->setPrev(node->prev());
		deletedNode.push_back(node);
		node = node->next();
	}
	auto tri = mTriangulation->addTriangle(node->data(), node->next()->data(), node->prev()->data());
	outTriangleSet.insert(tri);

	TriangleSet triangleDeleted;
	deletedNode.reverse();
	for(auto it = deletedNode.begin(); it != deletedNode.end(); it++)
	{
		triangleDeleted.clear();
		vertexInsertion((*it)->data(), (*it)->next()->data(), (*it)->prev()->data(),outTriangleSet,triangleDeleted);
		removeTriangles(triangleDeleted);

		(*it)->prev()->setNext(*it);
		(*it)->next()->setPrev(*it);
	}
}
void Mesher::vertexInsertion						(   Vertex* node, Vertex* next, Vertex* prev,
														TriangleSet & outTrianglesCreated,
														TriangleSet & outTrianglesDeleted)
{
	auto edge = mTriangulation->addEdge(next,prev);
	Triangle* triangle = nullptr;

	for(size_t i = 0; i < 2; i++)
	{
		if(outTrianglesCreated.find(edge->triangle(i)) != outTrianglesCreated.end())
		{
			triangle = edge->triangle(i);
			break;
		}
	}
	if(triangle != nullptr)
	{
		auto oppVertex = triangle->opposite(edge);
	
		Vector3D center;
		double radius;
		Triangle::findCircumCircle(node->point(),next->point(),prev->point(),center,radius);
		Circle circle(center, radius);

		if(circle.isInside(oppVertex->point()))
		{
			outTrianglesDeleted.insert(triangle);
			outTrianglesCreated.erase(triangle);
			mTriangulation->removeTriangle(triangle, false);
			vertexInsertion(node, next, oppVertex, outTrianglesCreated, outTrianglesDeleted);
			vertexInsertion(node, prev, oppVertex, outTrianglesCreated, outTrianglesDeleted);
		}
		else
		{
			auto tri = mTriangulation->addTriangle(node,next,prev);
			outTrianglesCreated.insert(tri);

		}
	}
	else
	{
		auto tri = mTriangulation->addTriangle(node,next,prev);
		outTrianglesCreated.insert(tri);
	}

	if(edge->triangle(0) == nullptr && edge->triangle(1) == nullptr)
	{
		if(mFixedEdges->find(edge) == mFixedEdges->end())
			mTriangulation->removeEdge(edge);
	}
}
bool Mesher::pointInPolygon							(   const Vector3D & inPoint)const
{
	for(auto it = mPLCBSPMap->begin(); it != mPLCBSPMap->end(); it++)
	{
		BSPTree2D* bsp = it->second.first;
		BSPTree2D::Pos ref = it->second.second;
		BSPTree2D::Pos pos = bsp->pointPosition(inPoint);
		if(pos == ref)
			return false;
	}
	return true;
}
void Mesher::createBSP								(   )
{
	auto reverseSegment = [](const Domain::PointArray & inPointlist, Domain::PointArray & outPointlist)
	{
		outPointlist.resize(inPointlist.size());
		for(size_t i = 0; i < inPointlist.size(); i++)
			outPointlist[inPointlist.size() - i - 1] = inPointlist[i];
	};

	// creating bsp for boudnaries
	string status;
	Vector3D referencePt(1e12, 1e12, 1e12);
	auto bspBoundary = BSPTree2D::create(mDomain->boundary(), status);
	auto posBoundary = bspBoundary->pointPosition(referencePt);
	mPLCBSPMap->insert(PLCBSPMap::value_type(&mDomain->boundary(),
						 BSPNodeInsidePair(bspBoundary, posBoundary))); 

	// creating bsp for holes
	auto & holes = mDomain->holes();
	for(size_t i = 0; i < holes.size(); i++)
	{
		auto bspHole = BSPTree2D::create(holes[i], status);
		auto posHole = bspHole->pointPosition(referencePt);

		// if bsp of hole direction is same as  boundaries
		// then change the direction of the hole.
		if(posHole == posBoundary)
		{
			Domain::PointArray reversedHole;
			reverseSegment(holes[i], reversedHole);
			auto bspReversedHole = BSPTree2D::create(reversedHole, status);
			auto posReversed = bspReversedHole->pointPosition(referencePt);
			mPLCBSPMap->insert(PLCBSPMap::value_type(&holes[i],
								 BSPNodeInsidePair(bspReversedHole, posBoundary)));
		}
		else
		{
			mPLCBSPMap->insert(PLCBSPMap::value_type(&holes[i], 
								 BSPNodeInsidePair(bspHole, posBoundary)));
		}
	}
}
inline bool Mesher::isInDiametricalCircle			(   const Vector3D & edgeP1, 
														const Vector3D & edgeP2, 
														const Vector3D & inPoint)const
{
	return mTol->isLess((inPoint - edgeP1)*(inPoint - edgeP2), 0);
}
void Mesher::laplacianSmoothing						(   )
{
	auto modifiedLocation = [&](Vertex* inVertex)->Vector3D
	{
		list<Edge*> edgeList;
		list<Triangle*> triangleList;
		inVertex->neighbours(triangleList, edgeList);

		Vector3D newLocation;
		for(auto vitr = edgeList.begin(); vitr != edgeList.end(); vitr++)
			newLocation = newLocation + (*vitr)->otherVertex(inVertex)->point();

		newLocation = newLocation/(double)edgeList.size();
		return newLocation;
	};

	const Triangulation::VertexSet* vertices = mTriangulation->vertices();
	for(auto pitr = vertices->begin(); pitr != vertices->end(); pitr++)
	{
		if((*pitr)->triangle() == 0)
			continue;
		if(mFixedVertices->find(*pitr) != mFixedVertices->end())
			continue;
		
		//auto prevLocation = (*pitr)->point();
		//double prevMin = minAngleAroundVertex(*pitr);
		auto newLocation = modifiedLocation(*pitr);
		//double newMin = minAngleAroundVertex(*pitr);
		//if(newMin < prevMin)
		//	(*pitr)->set(prevLocation);
		//else
			(*pitr)->set(newLocation);
	}
}
void Mesher::angleBasedSmoothing					(	)
{
	auto refinedLocation = [this](Vertex* inVertex, Edge* inEdge)->Vector3D
	{
		if(inEdge->triangle(0) == nullptr || inEdge->triangle(1) == nullptr)
			return Vector3D(0,0,0);

		Edge* e1 = inEdge->triangle(0)->opposite(inVertex);
		Edge* e2 = inEdge->triangle(1)->opposite(inVertex);
	
		Vertex* common = inEdge->otherVertex(inVertex);

		Vector3D v = common->point() - inVertex->point();
		Vector3D v1 = common->point() - e1->otherVertex(common)->point();
		Vector3D v2 = common->point() - e2->otherVertex(common)->point();

		v.normalize();
		v1.normalize();
		v2.normalize();

		//double angle1 = acos(v*v1), angle2 = acos(v*v2);
		double angle1 = 0, angle2 = 0;
		if(MesherUtility::isCounterClockwise(common->point(), inVertex->point(), e1->otherVertex(common)->point(),*mTol))
		{
			angle2 = acos(v*v1);
			angle1 = acos(v*v2);
		}
		else
		{
			angle2 = acos(v*v2);
			angle1 = acos(v*v1);
		}

		double avgAngle = (angle2 - angle1)/2.0; 

		auto ni = inVertex->point();
		auto nj = common->point();

		double x = nj.x() + (ni.x() - nj.x())*cos(avgAngle) - (ni.y() - nj.y())*sin(avgAngle);
		double y = nj.y() + (ni.x() - nj.x())*sin(avgAngle) + (ni.y() - nj.y())*cos(avgAngle);

		return Vector3D(x, y, ni.z());
		
	};
	auto modifiedLocation = [&](Vertex* inVertex)->Vector3D
	{
		list<Edge*> edgeList;
		list<Triangle*> triangleList;
		inVertex->neighbours(triangleList,edgeList);

		Vector3D newLocation;
		for(auto vitr = edgeList.begin(); vitr != edgeList.end(); vitr++)
			newLocation = newLocation + refinedLocation(inVertex, *vitr);

		newLocation = newLocation/(double)edgeList.size();
		return newLocation;
	};

	const Triangulation::VertexSet* vertices = mTriangulation->vertices();
	for(auto pitr = vertices->begin(); pitr != vertices->end(); pitr++)
	{
		if((*pitr)->triangle() == 0)
			continue;
		if(mFixedVertices->find(*pitr) != mFixedVertices->end())
			continue;
		
		//auto prevLocation = (*pitr)->point();
		//double prevMin = minAngleAroundVertex(*pitr);
		auto newLocation = modifiedLocation(*pitr);
		//double newMin = minAngleAroundVertex(*pitr);
		//if(newMin <= prevMin)
		//	(*pitr)->set(prevLocation);
		//else
			(*pitr)->set(newLocation);
	}
}
bool Mesher::watsonDelaunay							(	const Vector3D & inPoint,
														TriangleSet & trianglesDeleted,
														TriangleSet & trianglesCreated,
														Triangle* referenceTriangle)
{
	if(mTriangulation->find(inPoint))
		return false;

	// delaunay by bowyer-watson method
	Triangle* trgl = locateTriangle(inPoint, referenceTriangle);

	if(trgl == nullptr)
		return false;

	//showTriangle(trgl);
	trianglesDeleted.insert(trgl);
	findCircumScribingNeigbours(inPoint, trgl, trianglesDeleted, true);

	vector<Edge*> wrapperEdges;
	triangleWrapper(trianglesDeleted, wrapperEdges);

	auto newvtx = mTriangulation->addVertex(inPoint);
	if(&newvtx->point() == nullptr)
	{
		int a = 0;
		cout << "found";
		cin >> a;
	}

	for(auto it = trianglesDeleted.begin(); it != trianglesDeleted.end(); it++)
		mTriangulation->removeTriangle(*it, false);

	for(auto it = wrapperEdges.begin(); it != wrapperEdges.end(); it++)
	{
		auto tri = mTriangulation->addTriangle(newvtx, (*it)->vertex(0), (*it)->vertex(1));
		if(tri && !mTol->isZero(tri->area()))
			trianglesCreated.insert(tri);
		else
			trianglesDeleted.insert(tri);
	}
	return true;
}
void Mesher::removeTriangles						(   TriangleSet& tmpTriangles, bool flag)
{
	// remove triangles
	//while(tmpSet.size())
	//{
	//	auto tri = *tmpSet.begin();
	//	mTriangulation->removeTriangle(tri, flag);
	//	tmpSet.erase(tmpSet.begin());
	//}

	EdgeSet tmpEdges;
	VertexSet tmpVertices;
	for(auto trItr = tmpTriangles.begin(); trItr != tmpTriangles.end(); trItr++)
	{
		tmpEdges.insert((*trItr)->edge(0));
		tmpEdges.insert((*trItr)->edge(1));
		tmpEdges.insert((*trItr)->edge(2));

		tmpVertices.insert((*trItr)->vertex(0));
		tmpVertices.insert((*trItr)->vertex(1));
		tmpVertices.insert((*trItr)->vertex(2));
	}

	for(auto trItr = tmpTriangles.begin(); trItr != tmpTriangles.end(); trItr++)
		mTriangulation->removeTriangle(*trItr, flag);

	for(auto edgeItr = tmpEdges.begin(); edgeItr != tmpEdges.end(); edgeItr++)
		mTriangulation->removeEdge(*edgeItr, flag);

	//mHighlightPoint.clear();
	for(auto vtxItr = tmpVertices.begin(); vtxItr != tmpVertices.end(); vtxItr++)
	{
		//mHighlightPoint.push_back((*vtxItr)->point());
		mTriangulation->removeVertex(*vtxItr);
	}
}
bool Mesher::watsonDelaunay							(	const Vector3D & inPoint,
														Triangle* referenceTriangle)
{
	if(mTriangulation->find(inPoint))
		return false;

	// delaunay by bowyer-watson method
	Triangle* trgl = locateTriangle(inPoint, referenceTriangle);
	
	if(trgl == nullptr)
		return false;

	TriangleSet trianglesToDelete;
	trianglesToDelete.insert(trgl);
	findCircumScribingNeigbours(inPoint, trgl, trianglesToDelete);
	
	auto newvtx = mTriangulation->addVertex(inPoint);
	assert(newvtx);

	vector<Edge*> wrapperEdges;
	triangleWrapper(trianglesToDelete, wrapperEdges);

	// remove connecetions
	for(auto it = trianglesToDelete.begin(); it != trianglesToDelete.end(); it++)
		mTriangulation->removeTriangle(*it,false);

	// create triangles
	vector<Triangle*> newTriangles;
	for(auto it = wrapperEdges.begin(); it != wrapperEdges.end(); it++)
		newTriangles.push_back(mTriangulation->addTriangle(newvtx, (*it)->vertex(0), (*it)->vertex(1)));

	// actually remove triangles
	while(trianglesToDelete.size())
	{
		mTriangulation->removeTriangle(*trianglesToDelete.begin());
		trianglesToDelete.erase(trianglesToDelete.begin());
	}

	return true;
}
void Mesher::refineTriangle							(   Triangle* inTriangle,
														TriangleSet & outTrianglesCreated,
														TriangleSet & outTrianglesDeleted)
{
	bool toSplit = false;
	constraintEdge = nullptr;
	Vector3D insertionPoint = offCenterPoint(inTriangle);
	//Vector3D insertionPoint = inTriangle->circumCircle().center();
	if(isFixedEdgeEncroached(inTriangle, &constraintEdge))
	{
		insertionPoint = splitContraintEdge(constraintEdge);
		//mHighlightPoint.push_back(insertionPoint);
		//insertionPoint = constraintEdge->midPoint();
		if(!mTriangulation->find(insertionPoint))
		{
			VertexSet circumCenters;
			findCircumCentersInDiametralDisk(constraintEdge, circumCenters);
			removeVertices(circumCenters);

			lastTriangle = constraintEdge->triangle(0);
			bool success = watsonDelaunay(insertionPoint, outTrianglesDeleted, outTrianglesCreated, constraintEdge->triangle(0));
			if(success)
				splitFixedEdge(constraintEdge, insertionPoint);
		}
	}
	else
	{
		watsonDelaunay(insertionPoint, outTrianglesDeleted, outTrianglesCreated);
	}
	mBadTriangleSet->erase(inTriangle->aspectRatio(), inTriangle);
}
void Mesher::chewRefinement							(	)
{
	mBadTriangleSet->clear();
	evaluateByRatio(*mTriangulation->triangles());
	TriangleSet trianglesDeleted, trianglesCreated;

	while(mBadTriangleSet->size())
	{
		trianglesDeleted.clear();
		trianglesCreated.clear();

		auto triangle = (--mBadTriangleSet->end())->second;

		/*if(i++ == 100);*/
			//return;
		//showTriangle(triangle);
		//mHighlightPoint.push_back(triangle->circumCircle().center());

		lastTriangle = triangle;

		if(hopelessTriangle(triangle))
		{
			mBadTriangleSet->erase(triangle->aspectRatio(),triangle);
			continue;
		}
		else
		{ 
			refineTriangle(triangle, trianglesCreated, trianglesDeleted);
			updateTriangleSet(trianglesCreated, trianglesDeleted);
		}
	}
}
void Mesher::areaRefinement							(   )
{
	mBadTriangleSet->clear();
	evaluateByArea(*mTriangulation->triangles());
	TriangleSet trianglesDeleted, trianglesCreated;

	while(mBadTriangleSet->size())
	{
		trianglesDeleted.clear();
		trianglesCreated.clear();

		auto triangle = (--mBadTriangleSet->end())->second;
		lastTriangle = triangle;

		if(triangle == nullptr)
		{
			mBadTriangleSet->erase(triangle->aspectRatio(),triangle);
			mTriangulation->removeTriangle(triangle);
			continue;
		}

		refineTriangle(triangle, trianglesCreated, trianglesDeleted);
		updateBadTriangles(trianglesDeleted);
		removeTriangles(trianglesDeleted);
		evaluateByArea(trianglesCreated);
	}
}
static int i = 0, size = 0, num = 30608;

void Mesher::edgeRefinement							(   )
{
	mBadEdgesSet->clear();
	evaluateByEdgeLen(*mTriangulation->triangles());
	TriangleSet trianglesDeleted, trianglesCreated;

	while(mBadEdgesSet->size())
	{
		if(i++ == num)
		{
			trianglesDeleted.clear();
			//break;
		}

		trianglesDeleted.clear();
		trianglesCreated.clear();

		auto edge = (--mBadEdgesSet->end())->second;
		if(edge->triangle(0) == nullptr && edge->triangle(1) == nullptr)
		{
			bool success = mBadEdgesSet->erase(edge->lengthSqr(), edge);
			continue;
		}
		lastTriangle = edge->triangle(0);

		auto insertionPoint = edge->midPoint();
		if(i == num)
		{
			mHighlightPoint.push_back(insertionPoint);
			//break;
		}

		bool success = watsonDelaunay(insertionPoint, trianglesDeleted, trianglesCreated);
		if(success && mFixedEdges->find(edge) != mFixedEdges->end())
		{
			splitFixedEdge(edge, insertionPoint);
		}

		for(auto it = trianglesDeleted.begin(); it != trianglesDeleted.end(); it++)
		{
			for(size_t i = 0; i < 3; i++)
				mBadEdgesSet->erase((*it)->edge(i)->lengthSqr(), (*it)->edge(i));
		}

		mBadEdgesSet->erase(edge->lengthSqr(), edge);

		updateBadTriangles(trianglesDeleted);
		removeTriangles(trianglesDeleted);
		evaluateByEdgeLen(trianglesCreated);
	}
}
void Mesher::ruppertRefinement						(   )
{
	// we have to keep the encroached edges and
	// bad triangles list and encroached edges
	// are given priority
	mBadEdgesSet->clear();
	mBadTriangleSet->clear();

	evaluateEdges(*mTriangulation->edges());
	evaluateByRatio(*mTriangulation->triangles());
	while(/*mBadEdgesSet->size()||*/mBadTriangleSet->size())
	{
		//if(mBadEdgesSet->size())
		//{
		//	//   handle bad edges
		//	auto edge = (--mBadEdgesSet->end())->second;
		//	if(isEdgeEncroaching(edge))
		//		refineBadEdge(edge);
		//	else
		//		mBadEdgesSet->erase(edge->lengthSqr(), edge);
		//}
		//else
		{
			// handle bad triangles
			auto triangle = (--mBadTriangleSet->end())->second;
			if(hopelessTriangle(triangle))
			{
				mBadTriangleSet->erase(triangle->aspectRatio(),triangle);
				continue;
			}
			refineBadTriangle(triangle);
		}
	}
}
bool Mesher::refineBadEdge							(   Edge* inEdge)
{
	// we know its bad edge thats why we are here
	// just split the edge and see if its still encroaching.
	TriangleSet triangleCreated, triangleDeleted;
	auto v1 = inEdge->vertex(0), v2 =  inEdge->vertex(1);
	//auto insertionPoint = inEdge->midPoint();
	auto insertionPoint = splitContraintEdge(inEdge);
	bool success = watsonDelaunay(insertionPoint, triangleDeleted, triangleCreated, inEdge->triangle(0));
	//for(auto it = triangleDeleted.begin(); it != triangleDeleted.end(); it++)
	//{
	//	mHighlightPoint.push_back((*it)->vertex(0)->point());
	//	mHighlightPoint.push_back((*it)->vertex(1)->point());
	//	mHighlightPoint.push_back((*it)->vertex(2)->point());
	//}

	if(!success)
	{
		triangleDeleted.clear();
		triangleCreated.clear();
		insertionPoint = inEdge->midPoint();
		success = watsonDelaunay(insertionPoint, triangleDeleted, triangleCreated, inEdge->triangle(0));
		if(!success)
			return false;
		//mBadEdgesSet->erase(inEdge->lengthSqr(), inEdge);
		//return false;
	}

	auto midVertex = mTriangulation->find(insertionPoint);
	mBadEdgesSet->erase(inEdge->lengthSqr(), inEdge);
	// update the fixed edge if it is
	if(success && mFixedEdges->find(inEdge) != mFixedEdges->end())
		splitFixedEdge(inEdge, insertionPoint);	
		
	updateEdgeSet(triangleCreated, triangleDeleted);
	updateTriangleSet(triangleCreated, triangleDeleted ) ;

	int size = mFixedEdges->size();
	Edge* halfEdge[2];
	halfEdge[0] = mTriangulation->find(v1, midVertex);
	halfEdge[1] = mTriangulation->find(v2, midVertex);

	for(size_t i = 0; i < 2; i++)
	{
		if(halfEdge[i] == nullptr)
			continue;

		if(isEdgeEncroaching(halfEdge[i]))
			refineBadEdge(halfEdge[i]);
	}
	return true;
}
void Mesher::evaluateEdges							(   const EdgeSet & inEdgeSet)
{
	for(auto it = inEdgeSet.begin(); it != inEdgeSet.end(); it++)
	{
		if(mFixedEdges->find(*it) != mFixedEdges->end() && isEdgeEncroaching(*it))
			mBadEdgesSet->insert((*it)->lengthSqr(), *it);
	}
}
//void Mesher::evaluateEdges							(   const Triangulation::EdgeSet & inEdgeSet)
//{
//	for(auto it = inEdgeSet.begin(); it != inEdgeSet.end(); it++)
//	{
//		if(mFixedEdges->find(*it) != mFixedEdges->end() && isEdgeEncroaching(*it))
//			mBadEdgesSet->insert((*it)->lengthSqr(), *it);
//	}
//}
bool Mesher::isEdgeEncroaching						(   const Edge* inEdge)const
{
	if(!inEdge) return nullptr;

	for(size_t i = 0; i < 2; i++)
	{
		auto triangle = inEdge->triangle(i);
		if(!triangle)continue;
		if(!pointInPolygon(triangle->centroid()))
			continue;
		auto oppVertex = inEdge->triangle(i)->opposite(inEdge);
		if(isInDiametricalCircle(inEdge->vertex(0)->point(), inEdge->vertex(1)->point(), oppVertex->point()))
			return true;
	}
	return false;
}
void Mesher::updateEdgeSet							(   TriangleSet & trianglesCreated,
														TriangleSet & trianglesDeleted)
{
	for(auto it = trianglesDeleted.begin(); it != trianglesDeleted.end(); it++)
	{
		for(size_t i = 0; i < 3; i++)
			mBadEdgesSet->erase((*it)->edge(i)->lengthSqr(), (*it)->edge(i));
	}

	EdgeSet edges;
	for(auto it = trianglesCreated.begin(); it != trianglesCreated.end(); it++)
	{
		for(size_t i = 0; i < 3; i++)
			edges.insert((*it)->edge(i));
	}	
	evaluateEdges(edges);
}
bool Mesher::refineBadTriangle						(   Triangle* inTriangle)
{
	// we know its bad tri  angle, we have to insert its
	// circumcenter maintaining CDT
	//auto insertionPoint = inTriangle->circumCircle().center();
	auto insertionPoint = offCenterPoint(inTriangle);
	TriangleSet triangleCreated, triangleDeleted;

	if(!watsonDelaunay(insertionPoint, triangleDeleted, triangleCreated, inTriangle))
	{
		mBadTriangleSet->erase(inTriangle->aspectRatio(), inTriangle);
		return false;
	}

	Edge* ecroachingEdge = nullptr;
	auto insertionVtx = mTriangulation->find(insertionPoint);
	for(auto it = triangleCreated.begin(); it != triangleCreated.end(); it++)
	{
		auto oppEdge = (*it)->opposite(insertionVtx);
		if(	mFixedEdges->find(oppEdge) != mFixedEdges->end() &&
			isInDiametricalCircle(oppEdge->vertex(0)->point(), oppEdge->vertex(1)->point(), insertionPoint))
		{
			ecroachingEdge = oppEdge;
			break;
		}
	}
	if(ecroachingEdge)
	{
		for(auto it = triangleCreated.begin(); it != triangleCreated.end(); it++)
			mTriangulation->removeTriangle(*it, false);

		TriangleSet restoredTriangles;
		for(auto it = triangleDeleted.begin(); it != triangleDeleted.end(); it++)
			restoredTriangles.insert(mTriangulation->addTriangle((*it)->vertex(0), (*it)->vertex(1), (*it)->vertex(2)));

		updateBadTriangles(triangleDeleted);
		updateEdgeSet(restoredTriangles,triangleCreated); 
		updateTriangleSet(restoredTriangles,triangleCreated);

		removeTriangles(triangleDeleted);
		removeTriangles(triangleCreated);

		refineBadEdge(ecroachingEdge);
	}

	mBadTriangleSet->erase(inTriangle->aspectRatio(), inTriangle);
	updateEdgeSet(triangleCreated, triangleDeleted);
	updateTriangleSet(triangleCreated, triangleDeleted ) ;
	return true;
}
bool Mesher::hopelessTriangle						(   Triangle* inTriangle)
{
	// bad triangles having two fixed edges
	// are in the danger of continuous spilt
	size_t count = 0;
	for(size_t i = 0; i < 3; i++)
	{
		if(mFixedEdges->find(inTriangle->edge(i)) != mFixedEdges->end())
			count++;
	}
	if(count > 1)
	{

		//for(size_t i = 0;i < 3; i++)
		//{
		//	auto edge = inTriangle->edge(i);
		//	if(mFixedEdges->find(edge) != mFixedEdges->end())
		//	{
		//		auto oppVertex = inTriangle->opposite(edge);
		//		if(isInDiametricalCircle(edge->vertex(0)->point(), edge->vertex(1)->point(), oppVertex->point()))
		//			return false;
		//	}
		//}
		//return true;
		
		auto edge = inTriangle->smallestEdge();
		if( mOffCenters->find(edge->vertex(0)) != mOffCenters->end() &&
			mOffCenters->find(edge->vertex(1)) != mOffCenters->end())
			return true;
	}
	return false;
	
	
}
bool Mesher::isVisible								(   const Vector3D & inPoint1,
														const Vector3D & inPoint2,
														Edge** fixedEdge)
{
	//for(auto it = mFixedEdges->begin(); it != mFixedEdges->end(); it++)
	//{
	//	double t1, t2;
	//	MesherUtility::IntersectionType type = MesherUtility::intersect(inPoint1, inPoint2, (*it)->vertex(0)->point(), (*it)->vertex(1)->point(), t1, t2, *mTol);
	//	if(type == MesherUtility::Intersecting || type == MesherUtility::OnSegmentEnds)
	//		return false;
	//}
	//return true;
	
	auto triangle = locateTriangle(inPoint1);
	bool found = false;
	while(!found)
	{
		if(triangle == nullptr)
			return nullptr;
		auto v0 = triangle->vertex(0);
		auto v1 = triangle->vertex(1);
		auto v2 = triangle->vertex(2);

		if(!MesherUtility::isCounterClockwise(v0->point(),v1->point(),v2->point(),*mTol))
		{
			v0 = triangle->vertex(2);
			v2 = triangle->vertex(0);
		}
		
		auto & a = v0->point();
		auto & b = v1->point();
		auto & c = v2->point();

		auto abc = (b - a)^(c - a);
		auto abcMagSqr = abc.magnitudeSqr();

		auto bcp = (c - b)^(inPoint2 - b);
		double vala = (abc*bcp)/abcMagSqr;

		auto cap = (a - c)^(inPoint2 - c);
		double valb = (abc*cap)/abcMagSqr;

		auto valc = 1 - vala - valb;

		//if(mTol->isZero(vala)) return triangle->opposite(v0)->otherTriangle(triangle);
		//if(mTol->isZero(valb)) return triangle->opposite(v1)->otherTriangle(triangle);
		//if(mTol->isZero(valc)) return triangle->opposite(v2)->otherTriangle(triangle);

		double min = vala;
		Edge* edge = triangle->opposite(v0);
		if(mTol->isLess(valb, min))
		{
			edge = triangle->opposite(v1);
			min = valb;
		}
		if(mTol->isLess(valc, min))
		{
			edge = triangle->opposite(v2);
			min = valc;
		}
		if(mTol->isNegative(min))
		{
			if(mFixedEdges->find(edge) != mFixedEdges->end())
			{
				if(*fixedEdge)
					*fixedEdge = edge;
				return false;
			}
			triangle = edge->otherTriangle(triangle);
		}
		else  
			found = true;
	}
	return true;
}
bool Mesher::isFixedEdgeEncroached					(	Triangle* inTriangle,
														Edge** outEdge)
{
	auto center = inTriangle->circumCircle().center();
	auto triangle = inTriangle;
	Edge* lastEdge = nullptr;
	int i = 0;
	while(i < 3)
	{
		if(triangle == nullptr)
			return false;
		auto edge = triangle->edge(i);
		if(edge == nullptr || edge == lastEdge)
		{	
			i++;
			continue;
		}
		auto oppVertex = triangle->opposite(edge);
		if(!oppVertex)
			return false;

		double t1, t2;
		MesherUtility::IntersectionType type = MesherUtility::intersect(oppVertex->point(), center, edge->vertex(0)->point(), edge->vertex(1)->point(), t1, t2, *mTol);
		if(type == MesherUtility::Intersecting || type == MesherUtility::OnSegmentEnds)
		{
			if(mFixedEdges->find(edge) != mFixedEdges->end())
			{
				*outEdge = edge;
				lastTriangle = triangle;
				return true;
			}

			triangle = edge->otherTriangle(triangle);
			lastEdge = edge;
			i = 0;
		}
		else
			i++;
	}
	return false;
}
void Mesher::findCircumCentersInDiametralDisk		(   Edge* diametralEdge,
														VertexSet & outVertices)
{
	VertexSet visitedPoints;

	list<Vertex*> queue;
	for(size_t i = 0; i < 2; i++)
	{
		auto triangle = diametralEdge->triangle(i);
		if(!triangle)continue;
		auto vertex = triangle->opposite(diametralEdge);
		queue.push_back(vertex);
	}

	while(!queue.empty())
	{
		auto vertex = queue.front();
		queue.pop_front();
		visitedPoints.insert(vertex);

		if(isInDiametricalCircle(diametralEdge->vertex(0)->point(), diametralEdge->vertex(1)->point(), vertex->point()))
		{
			if(mFixedVertices->find(vertex) == mFixedVertices->end())
			{
				if(isVisible(vertex->point(), diametralEdge->midPoint()))
				{
					outVertices.insert(vertex);
					list<Edge*> edgeList;
					list<Triangle*> triangleList;
					vertex->neighbours(triangleList, edgeList);
					for(auto it = edgeList.begin(); it != edgeList.end(); it++)
					{
						auto otherVertex = (*it)->otherVertex(vertex);
						if(visitedPoints.find(otherVertex) == visitedPoints.end())
							queue.push_back(otherVertex);
					}
				}
			}
		}
	}
}
Triangle* Mesher::locateTriangle					(	const Vector3D & inPoint,
			 											Triangle* inTriangle)
{
	//const Triangulation::TriangleSet* triangles = mTriangulation->triangles();
	//for(auto it = triangles->begin(); it != triangles->end(); it++)
	//{
	//	if((*it)->isInside(inPoint, *mTol) == Triangle::Inside || 
	//		(*it)->isInside(inPoint, *mTol) == Triangle::OnVertex ||
	//		(*it)->isInside(inPoint, *mTol) == Triangle::OnEdge)
	//		return *it;
	//}
	//return nullptr;
	
	auto triangle = (inTriangle)?(inTriangle):(*mTriangulation->triangles()->begin());
	assert(triangle);
	//showTriangle(triangle);
	// we use barycentric method for point location
	// move the next triangle which is opp to the edge
	// whose opp vertex has lowest barycentric (-)
	// triangle is found when all barycentrics are +ve.
	// works for any kind of triangulation and degenerate
	// cases where point may lie on edge and vertex.
	// counter clockwise being +ve.
	
	bool found = false;
	while(!found)
	{
		if(triangle == nullptr)
			return nullptr;
		auto v0 = triangle->vertex(0);
		auto v1 = triangle->vertex(1);
		auto v2 = triangle->vertex(2);

		if(!MesherUtility::isCounterClockwise(v0->point(),v1->point(),v2->point(),*mTol))
		{
			v0 = triangle->vertex(2);
			v2 = triangle->vertex(0);
		}
		
		auto & a = v0->point();
		auto & b = v1->point();
		auto & c = v2->point();

		auto abc = (b - a)^(c - a);
		auto abcMagSqr = abc.magnitudeSqr();

		auto bcp = (c - b)^(inPoint - b);
		double vala = (abc*bcp)/abcMagSqr;

		auto cap = (a - c)^(inPoint - c);
		double valb = (abc*cap)/abcMagSqr;

		auto valc = 1 - vala - valb;

		//if(mTol->isZero(vala)) return triangle->opposite(v0)->otherTriangle(triangle);
		//if(mTol->isZero(valb)) return triangle->opposite(v1)->otherTriangle(triangle);
		//if(mTol->isZero(valc)) return triangle->opposite(v2)->otherTriangle(triangle);

		double min = vala;
		Edge* edge = triangle->opposite(v0);
		if(mTol->isLess(valb, min))
		{
			edge = triangle->opposite(v1);
			min = valb;
		}
		if(mTol->isLess(valc, min))
		{
			edge = triangle->opposite(v2);
			min = valc;
		}
		if(mTol->isNegative(min))
		{
			triangle = edge->otherTriangle(triangle);
		}
		else  
			found = true;
	}
	return triangle;
}
void Mesher::findCircumScribingNeigbours			(	const Vector3D & inPoint,
														Triangle * inTriangle,
														TriangleSet & inTriangleSet,
														bool isSegmentToSplit)
{
	if(isSegmentToSplit)
	{
		Edge* edge = nullptr;
		for(size_t i = 0; i < 3; i++)
		{
			auto tEdge =  inTriangle->edge(i);
			if(MesherUtility::isCollinear( tEdge->vertex(0)->point(),tEdge->vertex(1)->point(), inPoint, *mTol))
			{
				edge = tEdge;
				break;
			}
		}
		//auto edge = constraintEdge;
		if(edge == nullptr)
		{
			inTriangleSet.insert(inTriangle);
			for(size_t i = 0; i < 3; i++)
				constrainedCircumScribingTriangles(inPoint, inTriangle->edge(i), inTriangleSet);
		}
		else
		{
			for(size_t i = 0; i < 2; i++)
			{
				auto triangle = edge->triangle(i);
				if(!triangle)continue;
				inTriangleSet.insert(triangle);
				for(size_t j = 0; j < 3; j++)
				{
					if(triangle->edge(j) == edge)
						continue;
					constrainedCircumScribingTriangles(inPoint, triangle->edge(j), inTriangleSet);
				}
			}
		}
	}
	else
	{
		inTriangleSet.insert(inTriangle);
		for(size_t i = 0; i < 3; i++)
			circumScribingTriangles(inPoint, inTriangle->edge(i), inTriangleSet);
	}
}
void Mesher::constrainedCircumScribingTriangles		(	const Vector3D & inPoint,
														Edge * inEdge,
														TriangleSet & inTriangleSet)
{
	if(mFixedEdges->find(inEdge) != mFixedEdges->end())
		return;

	for(size_t i = 0; i < 2; i++)
	{
		auto triangle = inEdge->triangle(i);
		if(!triangle)continue;
		if(inTriangleSet.find(triangle) != inTriangleSet.end())
			continue;
		if(triangle->circumCircle().isInside(inPoint))
		{
			inTriangleSet.insert(triangle);
			for(size_t j = 0; j < 3; j++)
			{
				if(triangle->edge(j) == inEdge)
					continue;
				constrainedCircumScribingTriangles(inPoint, triangle->edge(j), inTriangleSet);
			}
		}
	}
}
void Mesher::circumScribingTriangles				(	const Vector3D & inPoint,
														Edge * inEdge,
														TriangleSet & inTriangleSet)
{
	for(size_t i = 0; i < 2; i++)
	{
		auto triangle = inEdge->triangle(i);
		if(!triangle)continue;
		if(inTriangleSet.find(triangle) != inTriangleSet.end())
			continue;
		if(triangle->circumCircle().isInside(inPoint))
		{
			inTriangleSet.insert(triangle);
			for(size_t j = 0; j < 3; j++)
			{
				if(triangle->edge(j) == inEdge)
					continue;
				circumScribingTriangles(inPoint, triangle->edge(j), inTriangleSet);
			}
		}
	}
}
void Mesher::triangleWrapper						(	TriangleSet & inTriangleSet,
														vector<Edge*> & outEdges)const
{
	for(auto it = inTriangleSet.begin(); it != inTriangleSet.end(); it++)
	{
		for(size_t i = 0; i < 3; i++)
		{
			Edge* edge = (*it)->edge(i);

			size_t count = 0;
			for(size_t j = 0; j < 2; j++)
			{
				if(inTriangleSet.find(edge->triangle(j)) != inTriangleSet.end())
					count++;
			}
			if(count < 2)
				outEdges.push_back(edge);
		}
	}
}
void Mesher::updateTriangleSet						(   TriangleSet & inTrianglesCreated,
														TriangleSet & inTrianglesDeleted)
{
	updateBadTriangles(inTrianglesDeleted);
	removeTriangles(inTrianglesDeleted);
	evaluateByRatio(inTrianglesCreated);	
	//evaluateTriangle(*mTriangulation->triangles());
}
void Mesher::updateBadTriangles						(   TriangleSet & inTrianglesSet)
{
	for(auto itTri = inTrianglesSet.begin(); itTri != inTrianglesSet.end(); itTri++)
		mBadTriangleSet->erase((*itTri)->aspectRatio(), *itTri);
}
void Mesher::evaluateByRatio						(	const TriangleSet & inTriangleSet)
{
	for(auto it = inTriangleSet.begin(); it != inTriangleSet.end(); it++)
	{
		if((*it) == nullptr || !pointInPolygon((*it)->centroid()))
			continue;

		double area = (*it)->area();
		if(mTol->isZero(area))
		{
			mTriangulation->removeTriangle((*it));
			return;
		}
		if(	mTol->isGreater((*it)->aspectRatio(), mCriteria->ratio()))
			mBadTriangleSet->insert((*it)->aspectRatio(), *it);
	}
}
void Mesher::evaluateByArea							(   const TriangleSet & inTriangleSet)
{
	for(auto it = inTriangleSet.begin(); it != inTriangleSet.end(); it++)
	{
		if(!pointInPolygon((*it)->centroid()))
			continue;

		double area = (*it)->area();
		if(mTol->isZero(area))
		{
			mTriangulation->removeTriangle((*it));
			return;
		}

		if(	mTol->isGreater((*it)->area(), mCriteria->maxArea()))
			mBadTriangleSet->insert((*it)->aspectRatio(), *it);
	}
}
void Mesher::evaluateByEdgeLen						(	const TriangleSet & inTriangleSet)
{
	for(auto it = inTriangleSet.begin(); it != inTriangleSet.end(); it++)
	{
		if(!pointInPolygon((*it)->centroid()))
			continue;

		double area = (*it)->area();
		if(mTol->isZero(area))
		{
			mTriangulation->removeTriangle((*it), false);
			return;
		}
		for(size_t i = 0 ; i < 3; i++)
		{
			if(mTol->isGreater((*it)->edge(i)->length(), mCriteria->maxEdgeLength()))
				mBadEdgesSet->insert((*it)->edge(i)->lengthSqr(), (*it)->edge(i));
		}
	}
}
//void Mesher::evaluateByRatio						(	const Triangulation::TriangleSet & inTriangleSet)
//{
//	for(auto it = inTriangleSet.begin(); it != inTriangleSet.end(); it++)
//	{
//		if((*it) == nullptr || !pointInPolygon((*it)->centroid()))
//			continue;
//
//		double area = (*it)->area();
//		if(mTol->isZero(area))
//		{
//			mTriangulation->removeTriangle((*it));
//			return;
//		}
//		if(	mTol->isGreater((*it)->aspectRatio(), mCriteria->ratio()))
//			mBadTriangleSet->insert((*it)->aspectRatio(), *it);
//	}
//}
//void Mesher::evaluateByArea							(   const Triangulation::TriangleSet & inTriangleSet)
//{
//	for(auto it = inTriangleSet.begin(); it != inTriangleSet.end(); it++)
//	{
//		if(!pointInPolygon((*it)->centroid()))
//			continue;
//
//		double area = (*it)->area();
//		if(mTol->isZero(area))
//		{
//			mTriangulation->removeTriangle((*it));
//			return;
//		}
//
//		if(	mTol->isGreater((*it)->area(), mCriteria->maxArea()))
//			mBadTriangleSet->insert((*it)->aspectRatio(), *it);
//	}
//}
//void Mesher::evaluateByEdgeLen						(	const Triangulation::TriangleSet & inTriangleSet)
//{
//	for(auto it = inTriangleSet.begin(); it != inTriangleSet.end(); it++)
//	{
//		if(!pointInPolygon((*it)->centroid()))
//			continue;
//
//		double area = (*it)->area();
//		if(mTol->isZero(area))
//		{
//			mTriangulation->removeTriangle((*it), false);
//			return;
//		}
//		for(size_t i = 0 ; i < 3; i++)
//		{
//			if(mTol->isGreater((*it)->edge(i)->length(), mCriteria->maxEdgeLength()))
//				mBadEdgesSet->insert((*it)->edge(i)->lengthSqr(), (*it)->edge(i));
//		}
//	}
//}
void Mesher::handleZeroAreaTriangle					(   Triangle* triangle,
														TriangleSet & outTriangleCreated,
														TriangleSet & outTriangleDeleted)
{
	//auto largestEdge = triangle->largestEdge();
	//auto otherTriangle = largestEdge->otherTriangle(triangle);
	//if(otherTriangle == nullptr)
	//	return;
	//auto oppVertex = otherTriangle->opposite(largestEdge);

	//outTriangleCreated.insert(mTriangulation->addTriangle(largestEdge->vertex(0), largestEdge->vertex(1), oppVertex));
	//outTriangleCreated.insert(mTriangulation->addTriangle(largestEdge->vertex(0), largestEdge->vertex(1), triangle->opposite(largestEdge)));

	//auto largestEdgeV1 = largestEdge->vertex(0);
	//auto largestEdgeV2 = largestEdge->vertex(1);

	//outTriangleCreated.erase(triangle);
	//outTriangleCreated.erase(otherTriangle);

	//outTriangleDeleted.insert(triangle);
	//outTriangleDeleted.insert(otherTriangle);

	//mTriangulation->removeTriangle(triangle, false);
	//mTriangulation->removeTriangle(otherTriangle, false);

	//mFixedEdges->erase(largestEdge);
	//auto edge1 = mTriangulation->find(largestEdgeV1, oppVertex);
	//auto edge2 = mTriangulation->find(largestEdgeV2, oppVertex);
	//if(edge1 && edge1->triangleCount())
	//	mFixedEdges->insert(edge1);
	//else
	//{
	//	return;
	//}
	//if(edge2 && edge2->triangleCount())
	//	mFixedEdges->insert(edge2);
	//else
	//{
	//	return;
	//}
}
Vector3D Mesher::offCenterPoint						(   Triangle* inTriangle)const
{
	auto center = inTriangle->circumCircle().center();
	auto smallestEdge = inTriangle->smallestEdge();

	// angle at the center
	auto ce1 = center - smallestEdge->vertex(0)->point();
	auto ce2 = center - smallestEdge->vertex(1)->point();
	ce1.normalize();
	ce2.normalize();
	auto cosofangle = ce1*ce2;
	auto minAngle = mCriteria->minAngle();
	if(mTol->isGreater(cosofangle,cos(minAngle)))
	{
		// calculate off- center point
		auto mid = smallestEdge->midPoint();
		auto dir = center - mid;
		dir.normalize();
		auto len = (smallestEdge->length()/2.0)/tan(minAngle/2.0);
		return smallestEdge->midPoint() + dir*len;
	}
	return center;
}
Vector3D Mesher::concentricShellPoint				(	const Vector3D & edgeP1,
														const Vector3D & edgeP2)const
{
	auto factor = mCriteria->minEdgeLength();
	auto dir = edgeP2 - edgeP1;
	dir.normalize();

	auto ratio = (edgeP2 - edgeP1).magnitude()/factor;
	auto k = floor(log(ratio)/log(2.0));
	auto len = factor*pow(2,k);
	return edgeP1 + dir*len;
}
Vector3D Mesher::splitContraintEdge					(   Edge* inEdge)
{
	auto commonVertex = [](Edge* edge1, Edge* edge2)->Vertex*
	{
		if(edge1->vertex(0) == edge2->vertex(0) || edge1->vertex(0) == edge2->vertex(1))
			return edge1->vertex(0);
		else
			return edge1->vertex(1);
	};
	auto findCosineAngle = [&](Edge* edge1, Edge* edge2)->double
	{
		Vertex* vertex = commonVertex(edge1, edge2);
		auto v1 = vertex->point() - edge1->otherVertex(vertex)->point();
		auto v2 = vertex->point() - edge2->otherVertex(vertex)->point();
		v1.normalize();
		v2.normalize();
		return v1*v2;
	};

	double cos90 = cos(M_PI/2.0);
	//double cosMinAgle = 0;
	Edge* edge = nullptr;
	for(size_t i = 0; i < 2; i++)
	{
		auto triangle = inEdge->triangle(i);
		if(!triangle || !pointInPolygon( triangle->centroid()))
			continue;

		for(size_t j = 0; j < 3; j++)
		{
			auto lEdge = triangle->edge(j);
			if(lEdge == inEdge || mFixedEdges->find(lEdge) == mFixedEdges->end())
				continue;

			double cosAngle = findCosineAngle(inEdge, lEdge);
			if(cosAngle > cos90)
				edge = lEdge;
		}
	}
	if(edge)
	{
		auto commonVtx = commonVertex(edge, inEdge);
		return concentricShellPoint(commonVtx->point(), inEdge->otherVertex(commonVtx)->point());
	}
	return inEdge->midPoint();
}
double Mesher::findMinimumAngle						(	)const
{
	double minAngle = 1e10;
	const Triangulation::TriangleSet* trglSet = mTriangulation->triangles();
	for(auto it = trglSet->begin(); it != trglSet->end(); it++)
	{
		double angle = (*it)->smallestAngle();
		if(angle < minAngle)
			minAngle = angle;
	}
	return minAngle*180/M_PI;
}
double Mesher::findMinEdgeLength					(   )const
{
	double minEdgelen = 1e10;
	const Triangulation::EdgeSet* edgeSet = mTriangulation->edges();
	for(auto it = edgeSet->begin(); it != edgeSet->end(); it++)
	{
		double distance = (*it)->lengthSqr();
		if(distance < minEdgelen)
			minEdgelen = distance;
	}
	return sqrt(minEdgelen);
}
void Mesher::showTriangle							(   Triangle* inTriangle)
{
	//mHighlightPoint.clear();
	//mHighlightPoint.push_back(inTriangle->vertex(0)->point());
	//mHighlightPoint.push_back(inTriangle->vertex(1)->point());
	//mHighlightPoint.push_back(inTriangle->vertex(2)->point());
}
void Mesher::showEdge								(   Edge* inEdge)
{
	//mHighlightPoint.clear();
	//mHighlightPoint.push_back(inEdge->vertex(0)->point());
	//mHighlightPoint.push_back(inEdge->vertex(1)->point());
}
