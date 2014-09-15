#pragma once

#include "Defines.h"
#include "Vector3D.h"
#include <unordered_set>
#include <unordered_map>

#define PRIME	2147483647

namespace GeometryCore
{
	class Entity;
	class Vertex;
	class Edge;
	class Triangle;
	class Grid2D;
	class GeometryCore_API Triangulation
	{
	private:
		struct VertexHash {
			size_t operator() ( const Vertex * v) const  { 
				return hash(v->point());
			}
		};
		struct VertexComparator {
			bool operator()( const Vertex* a, const Vertex* b) const 
			{
				return equal(a,b);
			}
		};

	public:
		typedef	list<Edge*>																EdgeList;
		typedef unordered_map<Vertex*, EdgeList, VertexHash, VertexComparator>			V2VMap;
		typedef unordered_set<Triangle*> 												TriangleSet;

		typedef unordered_set<const Vertex*>											VertexSet;
		typedef unordered_set<const Edge*>												EdgeSet;

	public:
		Triangulation();
		~Triangulation();

		Triangle*				addTriangle			( Vertex* v1, Vertex* v2, Vertex* v3);
		Edge*					addEdge				( Vertex* v1, Vertex* v2);
		Vertex*					addVertex			( Vector3D & v);

		bool					removeTriangle		( Triangle* inTriangle, bool removeConnection = true);
		bool					removeEdge			( Edge* inEdge);
		bool					removeVertex		( Vertex* inVertex);

		void					vertices			( VertexSet* outVertices)const;
		void					edges				( EdgeSet* outEdges)const;
		void					triangles			( TriangleSet* outTriangles)const;

		Triangle*				find				( Vertex* v1, Vertex* v2, Vertex* v3)const;
		Edge*					find				( Vertex* v1, Vertex* v2)const;
		Vertex*					find				( Vector3D & v)const;

		const EdgeList *		neighbors			( Vertex* inVertex)const;
		void					triangleFan			( Vertex* inVertex, list<Triangle*> & outTriangles, list<Edge*> & outEdges)const;

	private:
		static bool					equal			( const Vertex* v1, const Vertex* v2);
		static bool					equal			( const Edge* e1, const Edge* e2);
		static bool					equal			( const Triangle* t1, const Triangle* t2);

		static size_t				hash			( const Vector3D & point);
		static bool					isCounterClockwise( const Vector3D & inPoint1, 
														const Vector3D & inPoint2, 
														const Vector3D & inPoint3);

	private:
		TriangleSet*					mTlist;
		V2VMap*							mV2VMap;
	};
}
