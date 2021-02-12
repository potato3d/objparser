#ifndef _OBJ_TYPES_H_
#define _OBJ_TYPES_H_

#include <sig/sigslot.h>

namespace obj
{
	class face_index
	{
	public:
		int vertexIdx;
		int texCoordIdx;
		int normalIdx;

		face_index()
			: vertexIdx( 0 ), texCoordIdx( 0 ), normalIdx( 0 )
		{
			// empty
		}
	};

	//////////////////////////////////////////////////////////////////////////
	// 3-tuple container
	//////////////////////////////////////////////////////////////////////////
	template<typename T>
	class vec3
	{
	public:
		T x;
		T y;
		T z;

		vec3()
			: x( 0 ), y( 0 ), z( 0 )
		{
			// empty
		}

		void set( T value )
		{
			x = value;
			y = value;
			z = value;
		}
	};

	typedef vec3<double> vec3d;
}

#endif // _OBJ_TYPES_H_
