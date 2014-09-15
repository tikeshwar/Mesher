#pragma once

namespace GeometryCore
{
	class GeometryCore_API Circle
	{
	public:
		Circle(Vector3D inCenter, double inRadius);
		~Circle();

		const	Vector3D &	center				( )const;
				double		radius				( )const;

				bool		isInside			( const Vector3D & inPoint,
												  const Tolerance inTol = Tolerance())const;

	private:
		Vector3D			mCenter;
		double				mRadius;
	};
}