#pragma once
#include <unordered_set>
#include <unordered_map>

//namespace CCTECH_DATABASE
//{
//	class Point3D;
//	class Vertex;
//	class Edge;
//	class Triangle;
//	class PLC;
//	class Triangulation;
//	class Entity;
//	class Cell;
//
//	//set
//	typedef set<Triangle*>						TD_Triangle_Set;
//	typedef set<Edge*>							TD_Edge_Set;
//	typedef set<Triangulation*>					TD_Triangulation_Set;
//	typedef set<PLC*>							TD_Plc_Set;
//	typedef set<Entity*>						TD_Entity_Set;
//	typedef unordered_set<const Triangle*>		USetTrianglePtr;
//	typedef set<pair<Point3D*,Point3D*>>		SetPoint3DSegment;
//
//	//pair
//	typedef pair<size_t,size_t>					TD_Vertex_Index_Pair;
//	typedef pair<Vertex*,Vertex*>				TD_Vertex_Pair;
//	typedef pair<Point3D*,Point3D*>				TD_Point3D_Pair;
//
//	//vector
//	typedef vector <Point3D*>					TD_Point3D_Vector;
//	typedef vector <Vertex*>					TD_Vertex_Vector;
//	typedef vector <Triangle*>					TD_Triangle_Vector;
//	typedef vector <Edge*>						TD_Edge_Vector;
//	typedef vector <PLC*>						TD_PLC_Vector;
//	typedef vector <size_t>						TD_Size_t_Vector;
//
//	typedef vector< vector< vector <Cell *> > >	ArrArrArrCellPtr;
//	typedef	vector<pair<Point3D*,Point3D*> >	ArrPairPoint3D;
//	typedef vector<Point3D>						ArrPoint3D;
//	typedef vector<vector<Point3D> >			ArrArrPoint3D;
//
//	//list
//	typedef list <Edge*>						TD_Edge_List;
//	typedef list <Point3D*>						TD_Point3D_List;
//	typedef TD_Edge_List::iterator 				TD_Edge_List_Itr;
//	typedef map<TD_Vertex_Pair ,Edge*>			TD_VertexPairEdge_Map;
//	typedef TD_VertexPairEdge_Map::iterator 	TD_VertexPairEdge_Map_Itr;
//
//	//map
//	typedef map<TD_Vertex_Index_Pair, Edge*>	TD_VertexIndex_Edge_Map;
//	typedef map<Edge*, TD_Vertex_Index_Pair>	TD_Edge_VertexIndex_Map;
//	typedef map<Edge*, TD_Point3D_Pair>			TD_Edge_Point3dPair_Map;
//	typedef map<TD_Point3D_Pair, Edge*>			TD_Point3dPair_Edge_Map;
//	typedef map<size_t,TD_Size_t_Vector>		TD_Size_t_Size_t_Vector_Map;
//}