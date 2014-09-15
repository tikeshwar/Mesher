#include "StdAfx.h"
#include "Vector2D.h"

using namespace GeometryCore;

Vector2D::Vector2D( double _x,double _y)						//!< constructor
{
	mPosition[0] = _x;
	mPosition[1] = _y;
}
Vector2D::Vector2D( double _position[])									//!< constructor
{
	mPosition[0] = _position[0];
	mPosition[1] = _position[1];
}
Vector2D::Vector2D( const Vector2D& inVector2D)	//!< constructor
{
	mPosition[0] = inVector2D.mPosition[0];
	mPosition[1] = inVector2D.mPosition[1];
}
double		Vector2D::operator*				( const	Vector2D&	inVector)		const
{
	return (mPosition[0] * inVector.mPosition[0] +
			mPosition[1] * inVector.mPosition[1] );
}
Vector2D	Vector2D::operator*				( const	int		val)			const
{
	return Vector2D( val * mPosition[0],
					 val * mPosition[1]);
}
Vector2D	Vector2D::operator*				( const double	val)			const
{
	return Vector2D( val * mPosition[0],
					 val * mPosition[1]);
}
Vector2D	Vector2D::operator/				( const int		val)			const
{
	return Vector2D(mPosition[0]/ val,
						mPosition[1]/ val);
}
Vector2D	Vector2D::operator/				( const double	val)			const
{
	return Vector2D(mPosition[0]/val,
						mPosition[1]/val);
}
Vector2D	Vector2D::operator-				( )									const
{return Vector2D(-mPosition[0],-mPosition[1]);}
Vector2D	Vector2D::operator+				( const Vector2D&	inVector)		const
{
	return Vector2D( mPosition[0] + inVector.mPosition[0],
					 mPosition[1] + inVector.mPosition[1]);
}
Vector2D	Vector2D::operator-				( const Vector2D&	inVector)		const
{
	return Vector2D( mPosition[0] - inVector.mPosition[0],
					 mPosition[1] - inVector.mPosition[1]);
}
Vector2D	Vector2D::operator^				( const Vector2D&	inVector)		const
{
	return 	Vector2D( mPosition[1] * inVector.x() - mPosition[0] * inVector.y(),
					  mPosition[0] * inVector.y() - mPosition[1] * inVector.x());
}
Vector2D&	Vector2D::operator=				( const Vector2D&	inVector)		
{
	mPosition[0] = inVector.mPosition[0];
	mPosition[1] = inVector.mPosition[1];
	return *this;
}
Vector2D&	Vector2D::operator=				( const double	point[3])		
{
	mPosition[0] = point[0];
	mPosition[1] = point[1];
	return *this;
}
Vector2D&	Vector2D::operator*=			( const int		val)
{
	mPosition[0] *= val;
	mPosition[1] *= val;
	return *this;
}
Vector2D&	Vector2D::operator*=			( const double	val)
{
	mPosition[0] *= val;
	mPosition[1] *= val;
	return *this;
}
Vector2D&	Vector2D::operator/=			( const int		val)
{
	mPosition[0] /= val;
	mPosition[1] /= val;
	return *this;
}
Vector2D&	Vector2D::operator/=			( const double	val)
{
	mPosition[0] /= val;
	mPosition[1] /= val;
	return *this;
}
Vector2D&	Vector2D::operator+=			( const Vector2D&	inVector)
{
	mPosition[0] += inVector.x();
	mPosition[1] += inVector.y();
	return *this;
}
Vector2D&	Vector2D::operator-=			( const Vector2D&	inVector)
{
	mPosition[0] -= inVector.x();
	mPosition[1] -= inVector.y();
	return *this;
}
Vector2D&	Vector2D::operator^=			( const Vector2D&	inVector)
{
	double val1 = mPosition[1] * inVector.x() - mPosition[0] * inVector.y();
	double val2 = mPosition[0] * inVector.y() - mPosition[1] * inVector.x();
	mPosition[0] = val1;
	mPosition[1] = val2;
	return *this;
}
bool		Vector2D::operator==			( const Vector2D&	inVector)		const
{
	
	return	fabs(mPosition[0] - inVector.x())< _MFR_TOL_ && 
			fabs(mPosition[1] - inVector.y())< _MFR_TOL_ ;
}

bool		Vector2D::operator!=			( const Vector2D&	inVector)		const
{
	return !(*this==inVector);
}

bool		Vector2D::isEqual				( const Vector2D&	inVector2D,
											  const Tolerance&  inTol)		const
{
	return	inTol.isEqaul(mPosition[0],inVector2D.x())? 
			inTol.isEqaul(mPosition[1],inVector2D.y())? 
			true:false:false;
}
bool		Vector2D::isNotEqual			( const Vector2D&	inVector2D,
											  const Tolerance&  inTol)		const
{
	return	inTol.isEqaul(mPosition[0],inVector2D.x())? 
			inTol.isEqaul(mPosition[1],inVector2D.y())? 
			false:true:true;
}
bool		Vector2D::normalize				( const Tolerance&  inTol)									
{
	double ln = magnitude();
	if (inTol.isZero(ln))		return false;       
	mPosition[0] /= ln;	mPosition[1] /= ln;
	return true;
}
void		Vector2D::doubleArray			( double				outVector[3])	const
{
	outVector[0]= mPosition[0];
	outVector[1]= mPosition[1];
}
