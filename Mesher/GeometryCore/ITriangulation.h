#pragma once

#include <list>
#include "Vector3D.h"

using namespace std;

class IVertex;
class IEdge;
class ITriangle;

namespace GeometryCore
{
	// vertex contains one reference to any one of the triangles it belongs to
	// that's how fan queries are performed using that ref. triangle
	class IVertex
	{
	public:
		virtual	const Vector3D & point() const = 0;

		virtual bool add(IEdge* inTriangle) const = 0;
		virtual bool remove(IEdge* inTriangle) const = 0;

		virtual	void triangleFan(list<ITriangle*> & outTriangleFan) const = 0;	
		virtual	void triangleFan(list<IEdge*> & outTriangleFan)	const = 0;	

		virtual void set(double x, double y, double z);
	};

	// edge contains two vertex and two side triangles
	class IEdge
	{
	public:
		virtual const IVertex* vertex(size_t index) const = 0;
		virtual IVertex* vertex(size_t index) = 0;

		virtual bool add(ITriangle* inTriangle) const = 0;
		virtual bool remove(ITriangle* inTriangle) const = 0;

		virtual const ITriangle* triangle(const ITriangle* inTriangle) const = 0;
		virtual ITriangle* triangle(const ITriangle* inTriangle) = 0;

		virtual bool contains(const IVertex* inEdge) const = 0;
		virtual bool contains(const ITriangle* inEdge) const = 0;

		virtual void split(const Vector3D & inSplitPoint, IEdge* outEdges[2]) const ;

		virtual double length() const;
		virtual double lengthSqr() const;
	};

	class ITriangle
	{
	public:
		virtual const IVertex* vertex(size_t index) const = 0;
		virtual IVertex* vertex(size_t index) = 0;

		virtual const IEdge* edge(size_t index) const  = 0;
		virtual IEdge* edge(size_t index) = 0;

		virtual const IEdge* opposite(const IVertex* inVertex) const = 0;
		virtual IEdge* opposite(const IVertex* inVertex) = 0;

		virtual const IVertex* opposite(const IEdge* inEdge) const = 0;
		virtual IVertex* opposite(const IEdge* inEdge) = 0;

		virtual double aspectRatio() const ;
		virtual double area() const;
	};

	class ITriangulation
	{
	public:
		virtual ITriangle* add(IVertex* inVertex1, IVertex* inVertex2, IVertex* inVertex3) = 0;
		virtual IEdge* add(IVertex* inVertex1, IVertex* inVertex2) = 0;
		virtual IVertex* add(IVertex* inVertex) = 0;

		virtual ITriangle* add(ITriangle* inTriangle);
		virtual IEdge* add(IEdge* inEdge);

		virtual ITriangle* addPsuedo(IVertex* inVertex1, IVertex* inVertex2, IVertex* inVertex3);
		virtual IEdge* addPsuedo(IVertex* inVertex1, IVertex* inVertex2);
		virtual IVertex* addPsuedo(IVertex* inVertex);

		virtual bool remove(ITriangle* inTriangle) = 0;
		virtual bool remove(IEdge* inEdge) = 0;
		virtual bool remove(IVertex* inVertex) = 0;
	};
}