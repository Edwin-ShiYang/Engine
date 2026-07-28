#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>
#include <Engine/Core/StringUtils.hpp>


//-----------------------------------------------------------------------------------------------
const IntVec2 IntVec2::ZERO( 0, 0 );
const IntVec2 IntVec2::INVALID( -1, -1 );

//-----------------------------------------------------------------------------------------------
IntVec2::IntVec2( const IntVec2& copyFrom )
	: x( copyFrom.x )
	, y( copyFrom.y )
{
}


//-----------------------------------------------------------------------------------------------
IntVec2::IntVec2( int initialX, int initialY )
	: x( initialX )
	, y( initialY )
{
}


//-----------------------------------------------------------------------------------------------
float IntVec2::GetLength() const
{
	float length = sqrtf( static_cast<float>( x * x + y * y ) );

	return length;
}


//-----------------------------------------------------------------------------------------------
int IntVec2::GetLengthSquared() const
{
	return ( x * x ) + ( y * y );
};


//-----------------------------------------------------------------------------------------------
float IntVec2::GetOrientationRadians() const
{
	float orientationRadians = static_cast<float>( atan2( y, x ) );

	return orientationRadians;
};


//-----------------------------------------------------------------------------------------------
float IntVec2::GetOrientationDegrees() const
{
	float orientationRadians = static_cast<float>( atan2( y, x ) );
	float orientationDegrees = ConvertRadiansToDegrees( orientationRadians );

	return orientationDegrees;
}


//-----------------------------------------------------------------------------------------------
int IntVec2::GetTaxicabLength() const
{
	return abs( x ) + abs( y );
}


//-----------------------------------------------------------------------------------------------
IntVec2 const IntVec2::GetRotatedBy90Degrees() const
{
	return IntVec2( -y, x );
}


//-----------------------------------------------------------------------------------------------
IntVec2 const IntVec2::GetRotatedByMinus90Degrees() const 
{
	return IntVec2( y, -x );
};


//-----------------------------------------------------------------------------------------------
void IntVec2::SetFromText( char const* text )
{
	Strings result = SplitStringOnDelimiter( text, ',' );
	if ( static_cast < int >( result.size() ) != 2 )
	{
		x = 0;
		y = 0;
		return;
	}

	x = atoi( result [ 0 ].c_str() );
	y = atoi( result [ 1 ].c_str() );
}


//-----------------------------------------------------------------------------------------------
void IntVec2::Rotate90Degrees()
{
	int originalX = x;
	x = -y;
	y = originalX;
}


//-----------------------------------------------------------------------------------------------
void IntVec2::RotateMinus90Degrees()
{
	int originalX = x;
	x = y;
	y = -originalX;
}


//-----------------------------------------------------------------------------------------------
void IntVec2::operator=( const IntVec2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}


//-----------------------------------------------------------------------------------------------
IntVec2 const IntVec2::operator+( IntVec2 const& intVecToAdd ) const 
{
	return IntVec2( x + intVecToAdd.x, y + intVecToAdd.y );
};


//-----------------------------------------------------------------------------------------------
IntVec2 const IntVec2::operator-( IntVec2 const& intVecToSubtract ) const
{
	return IntVec2( x - intVecToSubtract.x, y - intVecToSubtract.y );
}


//-----------------------------------------------------------------------------------------------
bool IntVec2::operator==( IntVec2 const& compare ) const
{
	return  x == compare.x && y == compare.y;
}


//-----------------------------------------------------------------------------------------------
bool IntVec2::operator!=( IntVec2 const& compare ) const
{
	return  x != compare.x || y != compare.y;
}


//-----------------------------------------------------------------------------------------------
IntVec2 const GetAsIntVec2( float x, float y )
{
	return IntVec2( static_cast< int > ( x ), static_cast< int > ( y ) );
}


//-----------------------------------------------------------------------------------------------
void IntVec2::operator+=( IntVec2 const& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
IntVec2 const IntVec2::operator/( int inverseScale ) const
{
	return IntVec2( x / inverseScale, y / inverseScale );
}


//-----------------------------------------------------------------------------------------------
IntVec2 const IntVec2::operator-() const
{
	return IntVec2( -x, -y );
}
