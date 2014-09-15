#pragma once

namespace GeometryCore
{
	class GeometryCore_API Entity
	{
	public:
		enum EntityType
		{
			VERTEX = 0,
			EDGE,
			TRIANGLE,
		};
	public:
				Entity();
		virtual ~Entity();

		virtual EntityType entityType			( )const = 0;

	};
};