#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include <math.h>
#include "Engine/Core/StringUtils.hpp"
#include <string>


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::ZERO( 0.f, 0.f );
const Vec2 Vec2::ONE( 1.f, 1.f );
const Vec2 Vec2::UP( 0.f, 1.f );
const Vec2 Vec2::DOWN( 0.f, -1.f );
const Vec2 Vec2::LEFT( -1.f, 0.f );
const Vec2 Vec2::RIGHT( 1.f, 0.f );


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( Vec2 const& copy )
	: x( copy.x )
	, y( copy.y )
{
}


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( float initialX, float initialY )
	: x( initialX )
	, y( initialY )
{
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::MakeFromPolarDegrees( float orientationDegrees, float length )
{
	float orentationRadians = ConvertDegreesToRadians( orientationDegrees );

	float x = cosf( orentationRadians ) * length;
	float y = sinf( orentationRadians ) * length;

	return Vec2( x, y );
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::MakeFromPolarRadians( float orentationRadians, float length )
{
	float x = cosf( orentationRadians ) * length;
	float y = sinf( orentationRadians ) * length;

	return Vec2( x, y );
}


//-----------------------------------------------------------------------------------------------
float Vec2::GetLength() const
{
	float lengthSquared = ( x * x ) + ( y * y );

	return sqrtf( lengthSquared );
}


//-----------------------------------------------------------------------------------------------
float Vec2::GetLengthSquared() const
{
	return ( x * x ) + ( y * y );
}


//-----------------------------------------------------------------------------------------------
float Vec2::GetOrientationDegrees() const
{
	return Atan2Degrees( y, x );
};


//-----------------------------------------------------------------------------------------------
float Vec2::GetOrientationRadians() const
{
	return atan2f( y, x );
};


//-----------------------------------------------------------------------------------------------
Vec2 const  Vec2::GetRotatedBy90Degrees() const
{
	return Vec2( -y, x );
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetRotatedByMinus90Degrees() const
{
	return Vec2( y, -x );
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetRotatedByDegrees( float rotationDegrees )
{
	float rotatedDegrees = rotationDegrees + GetOrientationDegrees();
	float length = GetLength();

	const Vec2 rotatedVector = MakeFromPolarDegrees( rotatedDegrees, length );

	return rotatedVector;
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetRotatedByRadians( float rotationRadians )
{
	float rotatedRadians = rotationRadians + GetOrientationRadians();
	float length = GetLength();

	const Vec2 rotatedVector = MakeFromPolarRadians( rotatedRadians, length );

	return rotatedVector;
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetClamped( float maxLength ) const
{
	float length = GetLength();
	float scaleFactor = maxLength / length;

	return length <= maxLength ? Vec2( x, y ) : Vec2( x, y ) * scaleFactor;
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetNormalized() const
{
	float length = GetLength();

	if ( length > 0.f ) 
	{
		return Vec2( x / length, y / length );
	}

	return Vec2( 0.f,  0.f );
}


//-----------------------------------------------------------------------------------------------
void Vec2::SetOrientationDegrees( float newOrientationDegrees )
{
	float length = GetLength();
	const Vec2 rotatedVector = MakeFromPolarDegrees( newOrientationDegrees, length );

	x = rotatedVector.x;
	y = rotatedVector.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::SetOrientationRadians( float newOrientationRadians )
{
	float length = GetLength();
	const Vec2 rotatedVector = MakeFromPolarRadians( newOrientationRadians, length );
	 
	x = rotatedVector.x;
	y = rotatedVector.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::SetPolarDegrees( float newOrientationDegrees, float newLength )
{
	const Vec2 polarVector = MakeFromPolarDegrees( newOrientationDegrees, newLength );

	x = polarVector.x;
	y = polarVector.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::SetPolarRadians( float newOrientationRadians, float newLength )
{
	const Vec2 polarVector = MakeFromPolarRadians( newOrientationRadians, newLength );

	x = polarVector.x;
	y = polarVector.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::Rotate90Degrees()
{
	const Vec2 rotatedVector = GetRotatedBy90Degrees();

	x = rotatedVector.x;
	y = rotatedVector.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::RotateMinus90Degrees()
{
	const Vec2 rotatedVector = GetRotatedByMinus90Degrees();

	x = rotatedVector.x;
	y = rotatedVector.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::RotateDegrees( float rotationDegrees )
{
	const Vec2 rotatedVector = GetRotatedByDegrees( rotationDegrees );

	x = rotatedVector.x;
	y = rotatedVector.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::RotateRadians( float rotationRadians )
{
	const Vec2 rotatedVector = GetRotatedByRadians( rotationRadians );

	x = rotatedVector.x;
	y = rotatedVector.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::SetLength( float newLength )
{
	const Vec2 normalizedVector = GetNormalized();

	x = normalizedVector.x * newLength;
	y = normalizedVector.y * newLength;
}


//-----------------------------------------------------------------------------------------------
void Vec2::ClampLength( float maxLength )
{
	float length = GetLength();
	float scaleFactor = maxLength / length;

	const Vec2 clampedLength = length <= maxLength ? Vec2(x, y) : Vec2(x, y) * scaleFactor;

	x = clampedLength.x;
	y = clampedLength.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::Normalize()
{
	float length = GetLength();
	const Vec2 normalizedVector = Vec2( x / length, y / length );

	x = normalizedVector.x;
	y = normalizedVector.y;
}


//-----------------------------------------------------------------------------------------------
float Vec2::NormalizeAndGetPreviousLength()
{
	float length = GetLength();

	x /= length;
	y /= length;

	return length;
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetReflected( Vec2 const& normalOfSurfaceToReflectOffOf ) const 
{
	const float projectedLength  = DotProduct2D( Vec2( x, y ), normalOfSurfaceToReflectOffOf );
	const Vec2  projectedVector  = projectedLength * normalOfSurfaceToReflectOffOf;
	const Vec2  tangentialVector = Vec2( x, y ) - projectedVector;
	const Vec2  reflectedVector  = tangentialVector + ( -projectedVector );

	return reflectedVector;
}


//-----------------------------------------------------------------------------------------------
void Vec2::Reflect( Vec2 const& normalOfSurfaceToReflectOffOf )
{
	const Vec2  reflectedVector  = GetReflected( normalOfSurfaceToReflectOffOf );

	x = reflectedVector.x;
	y = reflectedVector.y;
}


void Vec2::SetFromText( char const* text )
{
	Strings result = SplitStringOnDelimiter( text, ',');
	if ( static_cast < int >( result.size() ) != 2 )
	{
		x = 0.f;
		y = 0.f;
		return;
	}

	x = static_cast< float > ( atof( result [ 0 ].c_str() ) );
	y = static_cast< float > ( atof( result [ 1 ].c_str() ) );
}

//-----------------------------------------------------------------------------------------------
bool Vec2::operator==( Vec2 const& compare ) const
{
	return  x == compare.x && y == compare.y;
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator!=( Vec2 const& compare ) const
{
	return  x != compare.x || y != compare.y;
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::operator+( Vec2 const& vecToAdd ) const
{
	return Vec2( x + vecToAdd.x, y + vecToAdd.y );
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::operator-( Vec2 const& vecToSubtract ) const
{
	return Vec2( x - vecToSubtract.x, y - vecToSubtract.y );
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::operator-() const
{
	return Vec2( -x, -y );
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::operator*( float uniformScale ) const
{
	return Vec2( x * uniformScale, y * uniformScale );
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::operator*( Vec2 const& vecToMultiply ) const
{
	return Vec2( x * vecToMultiply.x, y * vecToMultiply.y );
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::operator/( float inverseScale ) const
{
	return Vec2( x / inverseScale, y / inverseScale );
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator+=( Vec2 const& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator-=( Vec2 const& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator=( Vec2 const& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}


//-----------------------------------------------------------------------------------------------
Vec2 const operator*( float uniformScale, Vec2 const& vecToScale )
{
	return vecToScale * uniformScale;
}