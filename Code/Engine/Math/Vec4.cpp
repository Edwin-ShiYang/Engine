#include "Engine/Math/Vec4.hpp"
#include <math.h>


const Vec4 Vec4::ZERO( 0.f, 0.f, 0.f, 0.f );


//-----------------------------------------------------------------------------------------------
Vec4::Vec4( float initialX, float initialY, float initialZ, float initialW )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
	, w( initialW )
{
}


//-----------------------------------------------------------------------------------------------
Vec4::Vec4( Vec4 const& copyFrom )
	: x( copyFrom.x )
	, y( copyFrom.y )
	, z( copyFrom.z )
	, w( copyFrom.w )
{
}


//-----------------------------------------------------------------------------------------------
float Vec4::GetLength() const
{
	float lengthSquared = ( x * x ) + ( y * y ) + ( z * z ) + ( w * w );

	return sqrtf( lengthSquared );
}


//-----------------------------------------------------------------------------------------------
void Vec4::Normalize()
{
	float length = GetLength();
	
	if ( length <= 0.f )
	{
		x = 0.f;
		y = 0.f;
		z = 0.f;
		w = 1.f;
		return;
	}
	
	float inverseLength = 1.f / length;
	x *= inverseLength;
	y *= inverseLength;
	z *= inverseLength;
	w *= inverseLength;
}


//-----------------------------------------------------------------------------------------------
Vec4 Vec4::GetNormalized() const
{
	float length = GetLength();
	if ( length == 0.f ) 
	{
		return Vec4::ZERO;
	}
	float inverseLength = 1.f / length;
	return Vec4( x * inverseLength, y * inverseLength, z * inverseLength, w * inverseLength );
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator=( Vec4 const& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
	w = copyFrom.w;
}


//-----------------------------------------------------------------------------------------------
Vec4 const Vec4::operator-() const
{
	return Vec4( -x, -y, -z, -w );
}
