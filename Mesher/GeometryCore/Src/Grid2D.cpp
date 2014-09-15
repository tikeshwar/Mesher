#include "StdAfx.h"
#include "Grid2D.h"

using namespace GeometryCore;

Cell::Cell(double minx, double miny, double maxx, double maxy)
	:mMinx(minx), mMiny(miny), mMaxx(maxx), mMaxy(maxy)
{
	mObjects = new unordered_set<Entity*>();
}
Cell::~Cell()
{
	mObjects->clear();
	delete mObjects;
}
void Cell::insert				( Entity* inEntity)
{
	mObjects->insert(inEntity);
}
void Cell::remove				( Entity* inEntity)
{
	mObjects->erase(inEntity);
}

Grid2D::Grid2D(Vector3D start, size_t xunits, size_t yunits, double xdelta, double ydelta)
	:mStart(start), mXunits(xunits), mYunits(yunits), mXdelta(xdelta), mYdelta(ydelta)
{
	mCells = new vector<Cell*>(mXunits*mYunits);
	for(size_t i = 0; i < mXunits; i++)
	{
		for(size_t j = 0; j < mYunits; j++)
		{
			double minx = i*mXdelta + mStart.x();
			double miny = j*mYdelta + mStart.y();
			double maxx = minx + mXdelta;
			double maxy = miny + mYdelta;
			mCells->at(getIndex(i,j)) = new Cell(minx, miny, maxx, maxy);
		}
	}
}
Grid2D::~Grid2D()
{
	for(size_t i = 0; i < mXunits; i++)
	{
		for(size_t j = 0; j < mYunits; j++)
		{
			delete mCells->at(getIndex(i,j));
		}
	}

	delete mCells;
}
Cell* Grid2D::getCell				( const Vector3D & inPoint) 
{
	size_t indexI = (size_t)((inPoint.x() - mCells->at(0)->mMinx)/mXdelta);
	size_t indexJ = (size_t)((inPoint.y() - mCells->at(0)->mMiny)/mYdelta);
	if(indexI >= mXunits)indexI = mXunits - 1;
	if(indexJ >= mYunits)indexJ = mYunits - 1;
	size_t index = getIndex(indexI, indexJ);
	return mCells->at(index);
}
void Grid2D::insert				( Entity* inEntity)
{
	if(inEntity->entityType() == Entity::VERTEX)
	{
		Vertex* vertex = dynamic_cast<Vertex*>(inEntity);
		if(vertex != nullptr)
		{
			Cell* cell = getCell(vertex->point());
			cell->insert(vertex);
		}
	}
	else if(inEntity->entityType() == Entity::EDGE)
	{
		Edge* edge = dynamic_cast<Edge*>(inEntity);
		if(edge != nullptr)
		{
			GeometryCore::BoundingBox3D box;
			box.add(edge->vertex(0)->point());
			box.add(edge->vertex(1)->point());

			size_t lowerI = (size_t)((box.lower().x() - mCells->at(0)->mMinx)/mXdelta);
			size_t lowerJ = (size_t)((box.lower().y() - mCells->at(0)->mMiny)/mYdelta);
			size_t upperI = (size_t)((box.upper().x() - mCells->at(0)->mMaxx)/mXdelta) + 1;
			size_t upperJ = (size_t)((box.upper().y() - mCells->at(0)->mMaxy)/mYdelta) + 1;

			for(auto i = lowerI; i < upperI; i++)
			{
				for(auto j = lowerJ; j < upperJ; j++)
				{
					mCells->at(getIndex(i,j))->insert(edge);
				}
			}
		}
	}
	else if(inEntity->entityType() == Entity::TRIANGLE)
	{
		Triangle* triangle = dynamic_cast<Triangle*>(inEntity);
		if(triangle != nullptr)
		{
			GeometryCore::BoundingBox3D box;
			box.add(triangle->vertex(0)->point());
			box.add(triangle->vertex(1)->point());
			box.add(triangle->vertex(2)->point());

			size_t lowerI = (size_t)((box.lower().x() - mCells->at(0)->mMinx)/mXdelta);
			size_t lowerJ = (size_t)((box.lower().y() - mCells->at(0)->mMiny)/mYdelta);
			size_t upperI = (size_t)((box.upper().x() - mCells->at(0)->mMaxx)/mXdelta) + 1;
			size_t upperJ = (size_t)((box.upper().y() - mCells->at(0)->mMaxy)/mYdelta) + 1;

			for(auto i = lowerI; i < upperI; i++)
			{
				for(auto j = lowerJ; j < upperJ; j++)
				{
					mCells->at(getIndex(i,j))->insert(triangle);
				}
			}
		}
	}
}
void  Grid2D::remove				( Entity* inEntity)
{
	if(inEntity->entityType() == Entity::VERTEX)
	{
		Vertex* vertex = dynamic_cast<Vertex*>(inEntity);
		if(vertex != nullptr)
		{
			Cell* cell = getCell(vertex->point());
			cell->remove(vertex);
		}
	}
	else if(inEntity->entityType() == Entity::EDGE)
	{
		Edge* edge = dynamic_cast<Edge*>(inEntity);
		if(edge != nullptr)
		{
			GeometryCore::BoundingBox3D box;
			box.add(edge->vertex(0)->point());
			box.add(edge->vertex(1)->point());

			size_t lowerI = (size_t)((box.lower().x() - mCells->at(0)->mMinx)/mXdelta);
			size_t lowerJ = (size_t)((box.lower().y() - mCells->at(0)->mMiny)/mYdelta);
			size_t upperI = (size_t)((box.upper().x() - mCells->at(0)->mMaxx)/mXdelta) + 1;
			size_t upperJ = (size_t)((box.upper().y() - mCells->at(0)->mMaxy)/mYdelta) + 1;

			for(auto i = lowerI; i < upperI; i++)
			{
				for(auto j = lowerJ; j < upperJ; j++)
				{
					mCells->at(getIndex(i,j))->remove(edge);
				}
			}
		}
	}
	else if(inEntity->entityType() == Entity::TRIANGLE)
	{
		Triangle* triangle = dynamic_cast<Triangle*>(inEntity);
		if(triangle != nullptr)
		{
			GeometryCore::BoundingBox3D box;
			box.add(triangle->vertex(0)->point());
			box.add(triangle->vertex(1)->point());
			box.add(triangle->vertex(2)->point());

			size_t lowerI = (size_t)((box.lower().x() - mCells->at(0)->mMinx)/mXdelta);
			size_t lowerJ = (size_t)((box.lower().y() - mCells->at(0)->mMiny)/mYdelta);
			size_t upperI = (size_t)((box.upper().x() - mCells->at(0)->mMaxx)/mXdelta) + 1;
			size_t upperJ = (size_t)((box.upper().y() - mCells->at(0)->mMaxy)/mYdelta) + 1;

			for(auto i = lowerI; i < upperI; i++)
			{
				for(auto j = lowerJ; j < upperJ; j++)
				{
					mCells->at(getIndex(i,j))->remove(triangle);
				}
			}
		}
	}
}
size_t Grid2D::getIndex				( size_t i, size_t j)
{
	return i*mYunits + j;
}
unordered_set<Entity*>* Grid2D::objects				( Cell* inCell)
{
	return inCell->mObjects;
}