#pragma once

#include "Defines.h"

namespace GeometryCore
{
	class Vertex;
	class Edge;
	class Circle;
	class GeometryCore_API Triangle : public Entity
	{
	public:

		enum Position
		{
			Inside,
			OnEdge,
			OnVertex,
			Outside
		};

		static void findCircumCircle				( const Vector3D & p1,
													  const Vector3D & p2,
													  const Vector3D & p3,
													  Vector3D & outCenter, 
													  double & outRadius);
	private:
		Triangle(Vertex* v1, Vertex* v2, Vertex* v3, Edge* e1, Edge* e2, Edge* e3);
		~Triangle();

		EntityType				entityType		( )const{return TRIANGLE;} 
	public:
		Edge*					edge			( size_t index);
		const Edge*				edge			( size_t index)const;

		Vertex*					vertex			( size_t index);
		const Vertex*			vertex			( size_t index)const;

		bool					isVertex		( const Vertex* inVertex)const;
		bool					isEdge			( const Edge* inEdge)const;

		void					neighbours		( const Triangle* neighbors[3])const;

		double					angleAt			( const Vertex* inVertex)const;

		Edge*					opposite		( const Vertex* inVertex);
		const Edge*				opposite		( const Vertex* inVertex)const;

		Vertex*					opposite		( const Edge* inVertex);
		const Vertex*			opposite		( const Edge* inVertex)const;

		Position				isInside		( const Vector3D & inPoint,
												  const Tolerance & inTol)const; 
		const Circle &			circumCircle	( );
		Vector3D				centroid		( )const;
		double					area			( )const;
		double					aspectRatio		( )const;
		const Edge*				smallestEdge	( )const;
		const Edge*				largestEdge		( )const;
		double					smallestAngle	( )const;
		double					largestAngle	( )const;
		//void					Delete();
		//bool					isDeleted(){return mDeleted; }

	private:
		Edge*					mEdges[3];
		Vertex*					mVertices[3];
		Circle*					mCircle;
		double					mRatio;
	friend class Triangulation;
	};
}

