#include "Engine/Math/IntVec3.hpp"
#include "Vec3.hpp"

//-----------------------------------------------------------------------------------------------
IntVec3::IntVec3( int initialX, int initialY, int initialZ )
	: x(initialX)
	, y( initialY )
	, z( initialZ )
{
}


//-----------------------------------------------------------------------------------------------
IntVec3::IntVec3( const IntVec3& copyFrom )
	: x( copyFrom.x )
	, y( copyFrom.y )
	, z( copyFrom.z )
{
}


//-----------------------------------------------------------------------------------------------
IntVec3 const IntVec3::operator+( IntVec3 const& intVecToAdd ) const
{
	return IntVec3( x + intVecToAdd.x, y + intVecToAdd.y, z + intVecToAdd.z );
};


//-----------------------------------------------------------------------------------------------
IntVec3 const IntVec3::operator-( IntVec3 const& intVecToSubtract ) const
{
	return IntVec3( x - intVecToSubtract.x, y - intVecToSubtract.y, z - intVecToSubtract.z );
}


//-----------------------------------------------------------------------------------------------
IntVec3 const GetAsIntVec3( float x, float y, float z )
{
	return IntVec3( static_cast< int > ( x ), static_cast< int > ( y ), static_cast< int > ( z ) );
}


//-----------------------------------------------------------------------------------------------
IntVec3 const GetAsIntVec3( Vec3 const& vec3 )
{
	return IntVec3( static_cast< int > ( vec3.x ), static_cast< int > ( vec3.y ), static_cast< int > ( vec3.z ) );	
}
