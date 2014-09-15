#ifndef VECTOR2D_H
#define VECTOR2D_H

#include"Tolerance.h"

//-------------------------------------------------------------------------------------------------------------------
/**
 * @file 		Vector.h
 * @brief 		It provides functionalities related to Vector in 2D space.
 * @details		It provides functionalities related to Vector in 2D space. 
 *
 */
//-------------------------------------------------------------------------------------------------------------------

namespace GeometryCore
{ 

	class GeometryCore_API Vector2D
	{
	public:
		//------------------ constructor & destructor --------------------//
		Vector2D( double _x,double _y);						//!< constructor
		Vector2D( double _position[]);									//!< constructor
		Vector2D( const Vector2D& inVector2D = Vector2D(0.0,0.0));	//!< constructor
		~Vector2D() {}													//!< destructor
		//----------------------------------------------------------------//

		//-------------------- Data Set/Get Function -----------------------//
		//////////////////////////////////////////////////////////////////////
		// THESE FUNC. ARE KEPT INLINE intentionally TO ENHANCE PERFORMANCE //
		//////////////////////////////////////////////////////////////////////

		inline	void	set					( double _x,
											  double _y)	
		{mPosition[0]=_x;mPosition[1]=_y;}	//!< sets vector positon								
		/*inline*/	double	x					( )	const		
				{
					return mPosition[0];
				}						//!< return i componant of vector
		/*inline*/	double&	x					( )				
				{
					return mPosition[0];
				}						//!< return reference of i componant of vector
		inline	double	y					( )	const		
				{return mPosition[1];}						//!< return j componant of vector
		inline	double&	y					( )				
				{return mPosition[1];}						//!< return reference of j componant of vector
		inline	double*	array				( )				
				{return mPosition;}							//!< return vector position
				//----------------------------------------------------------------//

				//------------------------- Operators  ---------------------------//
		inline	double		operator[]			( int i)const	{return mPosition[i];}			
		inline	double&		operator[]			( int i)		{return mPosition[i];}	
				double		operator*			( const Vector2D&	inVector)		const;	//!< inner dot product
				Vector2D	operator*			( const int			val)			const;	//!< Scalar Multiplication with int
				Vector2D	operator*			( const double		val)			const;	//!< Scalar Multiplication with double
				Vector2D	operator/			( const int			val)			const;	//!< Scalar Division with int
				Vector2D	operator/			( const double		val)			const;	//!< Scalar Division with double
				Vector2D	operator-			( )									const;	//!< unary minus
				Vector2D	operator+			( const Vector2D&	inVector)		const;	//!< vector add
				Vector2D	operator-			( const Vector2D&	inVector)		const;	//!< vector subtract
				Vector2D	operator^			( const Vector2D&	inVector)		const;	//!< 3D exterior cross product
				Vector2D&	operator=			( const Vector2D&	inVector);				//!< vector assignment
				Vector2D&	operator=			( const double		point[3]);				//!< vector assignment
				Vector2D&	operator*=			( const int			val);					//!< vector scalar mult
				Vector2D&	operator*=			( const double		val);					//!< vector scalar mult
				Vector2D&	operator/=			( const int			val);					//!< vector scalar div
				Vector2D&	operator/=			( const double		val);					//!< vector scalar div
				Vector2D&	operator+=			( const Vector2D&	inVector);				//!< vector increment
				Vector2D&	operator-=			( const Vector2D&	inVector);				//!< vector decrement
				Vector2D&	operator^=			( const Vector2D&	inVector);				//!< 3D exterior cross product
				bool		operator==			( const Vector2D&	inVector)		const;	//!< Relational Operations
				bool		operator!=			( const Vector2D&	inVector)		const;	//!< Relational Operations
				//----------------------------------------------------------------//

				//-------------------- Tolerant Methods --------------------------//
				bool		isEqual				( const Vector2D&	inVector2D,
												  const Tolerance&  inTol = Tolerance())	const;	//!< Relational Operations

				bool		isNotEqual			( const Vector2D&	inVector2D,
												  const Tolerance&  inTol = Tolerance())	const;	//!< Relational Operations
				//----------------------------------------------------------------//
				
				//------------------------- Base Methods  ------------------------//
		inline	double		magnitude			( )											const	//!< vector length
				{return std::sqrt(magnitudeSqr( ));}			
				//----------------------------------------------------------------//
			
				//------------------ Self Modification Methods  ------------------//
				bool		normalize			( const Tolerance&		inTol = Tolerance());	//!< make base vector a unit vector
				//----------------------------------------------------------------//

				//--------------------- Performance Methods  ---------------------//
		inline	double		magnitudeSqr		( )									const	//!< vector length squared (faster)
				{return (mPosition[0]*mPosition[0] + mPosition[1]*mPosition[1]);}
				void		doubleArray			( double				outVector[2])	const;	//!< provide static array of double 
				//----------------------------------------------------------------//
		inline	double		distance			( const Vector2D & inPoint)			const
				{return (*this - inPoint).magnitude();}
	private:
		double	mPosition[2];
	};

	inline Vector2D	operator*	( const int		val,	
								  const Vector2D&	inVector)				//!< Scalar Multiplication with Int
	{return inVector*val;}

	inline Vector2D	operator*	( const double	val,
								  const Vector2D&	inVector)				//!< Scalar Multiplication with double
	{return inVector*val;}
}

#endif /*Vector2D_H*/
