#pragma once
#include <list>
#include <vector>
#include <string>
#include "BoundingBox3D.h"

namespace GeometryCore
{
	class GeometryCore_API BSPTree2D
	{
	public:
		typedef std::list<Vector3D>		PointList;
		typedef std::vector<Vector3D>	PointArray;

		enum Pos
		{
			UNKNOWN_POS,
			COLLINEAR,				///< coplanar collinear.		
			STRADDLING,				///< coplanar straddling.
			LEFTSIDE,				///< coplanar left side.
			RIGHTSIDE,				///< coplanar right side.
		};
	private:
		enum MidLineType
		{
			PARALLEL_X,						///< Middle Line parallel to x.
			PARALLEL_Y,						///< Middle Line parallel to y.
		};
		enum ChildType
		{
			LEFT,					///< left child.
			RIGHT					///< Right child.
		};
		class Segment
		{
		public:
			typedef list<Segment>	List; 
		public:
			Segment( ){}
			Segment(Vector3D inP1,Vector3D inP2)
				:mP1(inP1),mP2(inP2){};

			Vector3D&	point1			( ){return mP1;}
			Vector3D&	point2			( ){return mP2;}
			Vector3D		point1			( )const{return mP1;}
			Vector3D		point2			( )const{return mP2;}
			Pos				split			( Segment&		inLine,
											  Segment&		outLeftSegment,
											  Segment&		outRightSegment,
											  double		inTol);
			Pos				classifyPoint	( const Vector3D&	inParamVector,
											  double		inTol);

		private:
			bool			findInterSection	( Segment&		inLine, 
												  Vector3D&	outP,
												  double		inTol);
		private:
			Vector3D	mP1;
			Vector3D	mP2;
		};
		class Node
		{
		public:
			Node	( Segment::List&		inSegmentList,
					  std::string&			outState,
					  double				inTol,
					  size_t				inMaxDepth,
					  size_t				inNumMidPlaneDivsn = 0);
			~Node	( ){delete mSeg; delete mChild[LEFT];delete mChild[RIGHT];}

			void		build				( Segment::List&		inSegmentList,
											  size_t				inMaxDepth,
											  size_t				inNumMidPlaneDivsn = 0);

			Pos			pointPosition		( const Vector3D&			inPoint,
											  double				inTol)const;
		private:
			
				BoundingBox3D	findBoundingBox		( Segment::List&			inSegmentList);

				void			genSplittingLine	( Segment::List&			inSegmentList,
													  double					inTol,
													  size_t					inNumMidPlaneDivsn = 0);

				void			genMidLine			( BoundingBox3D					inBBox,
													  MidLineType				inMidPlaneType);
		private:
			Segment*		mSeg;
			Node*			mChild[2];
		};

	private:
		BSPTree2D(Node* baseNode,double	inTol)
			:mBaseNode(baseNode),mTolerance(inTol){};
		BSPTree2D(const BSPTree2D&):mBaseNode(nullptr){}
		BSPTree2D& operator=(const BSPTree2D&){return *this;}
		~BSPTree2D(void){}

	public:
		static	BSPTree2D*	create			( const PointList&					inPointList,
											  std::string&						outState,
											  double							inTol = 1.0e-13,
											  size_t							inMaxNodeCount = 7000,
											  size_t							numAxialDivsn = 0);
		static	BSPTree2D*	create			( const PointArray&					inPointArray,
											  std::string&						outState,
											  double							inTol = 1.0e-13,
											  size_t							inMaxNodeCount = 7000,
											  size_t							numAxialDivsn = 0);
				Pos			pointPosition	( const Vector3D&						inPoint)const;
				void		release			( ){delete this;}

	private:
		Node*		mBaseNode;
		double		mTolerance;
	};
}

