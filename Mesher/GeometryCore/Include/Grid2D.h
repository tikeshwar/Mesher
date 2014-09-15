#pragma once

namespace GeometryCore
{
	class Entity;
	class Vertex;
	class GeometryCore_API Cell
	{
	private:
		Cell( double minx, double miny, double maxx, double maxy);
		~Cell();

		void							insert				( Entity* inEntity);
		void							remove				( Entity* inEntity);
	private:
		unordered_set<Entity*>*			mObjects;
		double							mMinx;
		double							mMiny;
		double							mMaxx;
		double							mMaxy;

		friend class Grid2D;
	};
	class GeometryCore_API Grid2D
	{
	public:
		typedef							unordered_set<Entity*>		EntitySet;
	public:
		Grid2D(Vector3D start, size_t xunits = 100, size_t yunits = 100, double xdelta = 0.1, double ydelta = 0.1);
		~Grid2D();

		Cell*							getCell				( const Vector3D & inPoint);
		void							insert				( Entity* inEntity);
		void							remove				( Entity* inEntity);

		inline size_t					getIndex			( size_t i, size_t j);
		EntitySet*						objects				( Cell* inCell);
	private:
		vector<Cell*>*					mCells;
		size_t							mXunits;
		size_t							mYunits;
		double							mXdelta;
		double							mYdelta;
		Vector3D						mStart;
	};
}