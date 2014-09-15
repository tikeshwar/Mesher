#include "StdAfx.h"
#include "Typedefs.h"
#include "Triangle.h"
#include "Edge.h"
#include "Vertex.h"
#include "Triangulation.h"
#include <functional>

using namespace GeometryCore;

Triangulation::Triangulation()
{
	mV2VMap = new V2VMap();
	mTlist = new TriangleSet();
}
Triangulation::~Triangulation()
{
	TriangleSet outTriangles;
	triangles(&outTriangles);
	for(auto it = outTriangles.begin(); it != outTriangles.end(); it++)
		delete *it;
	mTlist->clear();

	EdgeSet outEdges;
	edges(&outEdges);
	for(auto it = outEdges.begin(); it != outEdges.end(); it++)
		delete *it;

	VertexSet outVertices;
	vertices(&outVertices);
	for(auto it = outEdges.begin(); it != outEdges.end(); it++)
		delete *it;

	mV2VMap->clear();
}

Triangle* Triangulation::addTriangle					( Vertex* v1, Vertex* v2, Vertex* v3)
{
	if(	v1 == v2 || v1 == v3 || v2 == v3 ||
		v1 == nullptr || v2 == nullptr || v3 == nullptr)
		return nullptr;

	auto trgl = find(v1, v2, v3);
	if(trgl == nullptr)
	{
		if(!isCounterClockwise(v1->point(), v2->point(), v3->point()))
		{
			auto temp = v1;
			v1 = v3;
			v3 = temp;
		}

		auto e1 = addEdge(v1, v2);
		auto e2 = addEdge(v2, v3);
		auto e3 = addEdge(v3, v1);
		auto newTriangle = new Triangle(v1,v2,v3, e2, e3, e1);

		e1->addTriangle(newTriangle);
		e2->addTriangle(newTriangle);
		e3->addTriangle(newTriangle);

		//v1->addTriangle(trgl);
		//v2->addTriangle(trgl);
		//v3->addTriangle(trgl);

		mTlist->insert(newTriangle);
		//mGrid2D->insert(trgl);
	}
	return const_cast<Triangle*>(trgl);
}
Edge* Triangulation::addEdge							( Vertex* v1, Vertex* v2)
{
	if(v1 == v2 || v1 == nullptr || v2 == nullptr)
		return nullptr;
	auto edge = find(v1, v2);
	if(edge == nullptr)
	{
		Edge* newEdge = new Edge(v1, v2);
		mV2VMap->find(v1)->second.push_back(newEdge);
		mV2VMap->find(v2)->second.push_back(newEdge);
	}
	return const_cast<Edge*>(edge);
}
Vertex* Triangulation::addVertex						( Vector3D & v)
{
	//Vertex* newVtx = new Vertex(v);
	auto vertex = find(v);
	if(vertex == nullptr)
	{	
		Vertex* newVtx = new Vertex(v);
		mV2VMap->insert(V2VMap::value_type(newVtx, EdgeList()));
		return newVtx;
	}
	return const_cast<Vertex*>(vertex);
}
bool Triangulation::removeTriangle						( Triangle* inTriangle, bool removeConnection)
{
	auto findItr = mTlist->find(inTriangle);
	if(findItr == mTlist->end())
		return false;

	if(removeConnection )
	{
		if(*findItr == inTriangle)
		{
			mTlist->erase(findItr);
			for(int i = 0; i < 3; i++)
				inTriangle->edge(i)->removeTriangle(inTriangle);

			delete inTriangle;
			return true;
		}
	}
	else
	{
		for(int i = 0; i < 3; i++)
			inTriangle->edge(i)->removeTriangle(inTriangle);
		return true;
	}
	return false;
}
bool Triangulation::removeEdge							( Edge* inEdge)
{
	if( inEdge->triangle(0) != nullptr ||
		inEdge->triangle(1) != nullptr )
		return false;

	auto findItr = mV2VMap->find(inEdge->vertex(0));
	if(findItr != mV2VMap->end())
		for(auto it = findItr->second.begin(); it != findItr->second.end(); it++)
			if(inEdge == (*it))
				findItr->second.erase(it);

	findItr = mV2VMap->find(inEdge->vertex(1));
	if(findItr != mV2VMap->end())
		for(auto it = findItr->second.begin(); it != findItr->second.end(); it++)
			if(inEdge == (*it))
				findItr->second.erase(it);

	delete inEdge;
	return true;

}
bool Triangulation::removeVertex						( Vertex* inVertex)
{
	auto findItr = mV2VMap->find(inVertex);
	if(findItr != mV2VMap->end())
	{
		if(findItr->second.empty())
		{
			mV2VMap->erase(findItr);
			delete inVertex;
			return true;
		}
	}
	return false;
}
void Triangulation::vertices							( VertexSet* outVertices)const
{
	for(auto it = mV2VMap->begin(); it != mV2VMap->end(); it++)
		outVertices->insert(it->first);
}
void Triangulation::edges								( EdgeSet* outEdges)const
{
	for(auto it = mV2VMap->begin(); it != mV2VMap->end(); it++)
		for(auto itr = it->second.begin(); itr != it->second.end(); itr++)
			outEdges->insert(*itr);
}
void Triangulation::triangles							( TriangleSet* outTriangles)const
{
	for(auto it = mTlist->begin(); it != mTlist->end(); it++)
		outTriangles->insert(*it);
}
Triangle* Triangulation::find							( Vertex* v1, Vertex* v2, Vertex* v3)const
{
	auto e1 = find(v1, v2);
	auto e2 = find(v2, v3);
	 
	if(e1 == nullptr || e2 == nullptr)
		return nullptr;

	for(size_t i = 0 ; i < 2; i++)
	{
		for(size_t j = 0; j < 2; j++)
		{
			if(e1->triangle(i) == e2->triangle(j) && e1->triangle(i))
				return e1->triangle(i);
		}
	}
	return nullptr;
}
Edge* Triangulation::find								( Vertex* v1, Vertex* v2)const
{
	if(v1 == nullptr || v2 == nullptr)
		return nullptr;

	auto findItr = mV2VMap->find(v1);
	if( findItr != mV2VMap->end())
	{
		for(auto it = findItr->second.begin(); it != findItr->second.end(); it++)
		{
			if(v2 == (*it)->otherVertex(v1))
				return *it;
		}
	}
	return nullptr;
}
Vertex* Triangulation::find								( Vector3D & v)const
{
	Vertex vertex(v);
	auto findItr = mV2VMap->find(&vertex);
	if( findItr != mV2VMap->end())
		return findItr->first;
	return nullptr;
}
inline const Triangulation::EdgeList *	Triangulation::neighbors	( Vertex* inVertex)const
{
	auto findItr = mV2VMap->find(inVertex);
	if( findItr != mV2VMap->end())
		return &findItr->second;
	return nullptr;
}
void							Triangulation::triangleFan		( Vertex* inVertex, list<Triangle*> & outTriangles, list<Edge*> & outEdges)const
{
	auto edges = neighbors(inVertex);
	if(edges == nullptr)
		return;

	auto edge = *edges->begin();
	auto triangle = edge->triangle(0);
	while(triangle)
	{
		outTriangles.push_back(triangle);
		outEdges.push_back(edge);
		edge = triangle->opposite(edge->otherVertex(inVertex));
		triangle = edge->otherTriangle(triangle);

		if(triangle == nullptr) 
			break;

		if(triangle == outTriangles.front())
			return;

	}

	edge = *edges->begin();;
 	triangle = edge->triangle(1);
	while(triangle)
	{
		if(triangle == nullptr) 
			break;

		outTriangles.push_front(triangle);
		outEdges.push_front(edge);
		edge = triangle->opposite(edge->otherVertex(inVertex));
		triangle = edge->otherTriangle(triangle);
	}
}
bool							Triangulation::equal			( const Vertex* v1, const Vertex* v2)
{
	return (v1->point() == v2->point());

}
bool							Triangulation::equal			( const Edge* e1, const Edge* e2)
{
	auto & p1 = e1->vertex(0)->point();
	auto & p2 = e1->vertex(1)->point();
	auto & q1 = e2->vertex(0)->point();
	auto & q2 = e2->vertex(1)->point();

	return (p1 == q1 && p2 == q2) || (p1 == q2 && p2 == q1);
}
bool							Triangulation::equal			( const Triangle* t1, const Triangle* t2)
{
	auto & p1 = t1->vertex(0)->point();
	auto & p2 = t1->vertex(1)->point();
	auto & p3 = t1->vertex(2)->point();
	auto & q1 = t2->vertex(0)->point();
	auto & q2 = t2->vertex(1)->point();	
	auto & q3 = t2->vertex(2)->point();

	return	(p1 == q1 && p2 == q2 && p3 == q3) ||
			(p1 == q1 && p2 == q3 && p2 == q2) ||
			(p1 == q2 && p2 == q1 && p3 == q3) ||
			(p1 == q2 && p2 == q3 && p3 == q1) ||
			(p1 == q3 && p2 == q1 && p3 == q2) ||
			(p1 == q3 && p2 == q2 && p3 == q1);
}
 size_t							Triangulation::hash				( const Vector3D & point)
{
	std::hash<double> hashD;
	size_t n = hashD(point.x()*PRIME*PRIME + point.y()*PRIME +  point.z());
	return n;
 }
bool							Triangulation::isCounterClockwise(	const Vector3D & inPoint1, 
																	const Vector3D & inPoint2, 
																	const Vector3D & inPoint3)
{
	return	(inPoint3.y() - inPoint1.y())*(inPoint2.x() - inPoint1.x()) > 
			(inPoint2.y() - inPoint1.y())*(inPoint3.x() - inPoint1.x()); 
}