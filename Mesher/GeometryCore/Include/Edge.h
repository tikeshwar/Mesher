#pragma once

#include "Defines.h"

namespace GeometryCore
{
	class Triangle;
	class Vertex;
	class GeometryCore_API Edge : public Entity
	{
	private:
		Edge( Vertex* v1, Vertex* v2);					
		~Edge();				

		EntityType				entityType			( )const{return EDGE;} 
	public:

		Vertex*					vertex				( size_t index);
		const Vertex*			vertex				( size_t index)const;

		Vertex*					otherVertex			( const Vertex* inVertex);
		const Vertex*			otherVertex			( const Vertex* inVertex)const;

		Triangle*				triangle			( size_t index);
		const Triangle*			triangle			( size_t index)const;

		Triangle*				otherTriangle		( const Triangle*);
		const Triangle*			otherTriangle		( const Triangle*)const;

		bool					addTriangle			( Triangle* inTriangle);		
		bool					removeTriangle		( Triangle* inTriangle);	
				
		bool					isEndPoint			( const Vertex* inVertex)const;
		bool					isNeighbour			( const Triangle* inTriangle)const;
		double					length				( )const;
		double					lengthSqr			( )const;
		Vector3D				midPoint			( )const;

	private:
		Triangle*			mTriangles[2];			//!<neighbour triangle set.
		Vertex*				mVertices[2];

	friend class Triangulation;
	friend class Triangle;

	};
}

