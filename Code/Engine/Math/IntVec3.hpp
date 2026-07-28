#pragma once


//-----------------------------------------------------------------------------------------------
struct Vec3;


//-----------------------------------------------------------------------------------------------
struct IntVec3
{
public:
	int x = 0;
	int y = 0;
	int z = 0;

	static const IntVec3 ZERO;

public:
	IntVec3()  = default;
	~IntVec3() = default;

	explicit IntVec3( int initialX, int initialY, int initialZ );	
	IntVec3( const IntVec3& copyFrom );

	IntVec3 const	operator+( IntVec3 const& intVecToAdd )                 const;	
	IntVec3 const	operator-( IntVec3 const& intVecToSubtract )            const;	
};


//-----------------------------------------------------------------------------------------------	
IntVec3 const GetAsIntVec3( float x, float y, float z );
IntVec3 const GetAsIntVec3( Vec3 const& vec3 );	