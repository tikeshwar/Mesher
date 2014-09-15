#include "stdafx.h"
#include "Vector3D.h"
#include "Triangulation.h"

//using namespace CCTECH_DATABASE;
int main()
{
	Triangulation * trgln = new Triangulation;
	std::vector<Vertex*> vertices;
	vertices.push_back(trgln->addVertex(Vector3D(0, 0, 0)));
	vertices.push_back(trgln->addVertex(Vector3D(0, 0.0, 0)));
	vertices.push_back(trgln->addVertex(Vector3D(1.0, 1.0, 0)));
	vertices.push_back(trgln->addVertex(Vector3D(1.0, 0, 0)));
	vertices.push_back(trgln->addVertex(Vector3D(1.0, 1.0, 1.0)));

	Triangle* trgl1 = trgln->addTriangle(vertices[0], vertices[1], vertices[2]);
	bool falg = trgln->removeTriangle(trgl1);

	return 1;
}