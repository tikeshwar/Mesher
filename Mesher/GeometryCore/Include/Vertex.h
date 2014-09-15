#ifndef VERTEX_H
#define VERTEX_H

#include "Vector3D.h"

namespace GeometryCore
{
	class GeometryCore_API Vertex : public Entity
	{
	private:
		Vertex(Vector3D inPoint);		
		~Vertex(void);			

		EntityType					entityType			( )const{return VERTEX;} 
	public:
		const Vector3D &			point				( )const;
		void						set					( const Vector3D & inPoint);

	private:
		Vector3D 					mPoint;

	friend class Triangulation;
	friend class Edge;
	};
}
#endif /*VERTEX_H*/
