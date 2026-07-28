#include "Engine/Math/Vec3.hpp"
#include "../Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>

//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::ZERO( 0.f, 0.f, 0.f );
Vec3 const Vec3::WORLD_UP( 0.f, 0.f, 1.f );
Vec3 const Vec3::WORLD_DOWN( 0.f, 0.f, -1.f );
Vec3 const Vec3::WORLD_LEFT( 0.f, 1.f, 0.f );
Vec3 const Vec3::WORLD_FORWARD( 1.f, 0.f, 0.f );

//-----------------------------------------------------------------------------------------------
Vec3::Vec3( Vec3 const& copyFrom )
{
    x = copyFrom.x;
    y = copyFrom.y;
    z = copyFrom.z;
}

//-----------------------------------------------------------------------------------------------
Vec3::Vec3( float initialX, float initialY, float initialZ )
    : x( initialX )
    , y( initialY )
    , z( initialZ )
{
}

//-----------------------------------------------------------------------------------------------
float Vec3::GetLength() const
{
    return sqrtf( ( x * x ) + ( y * y ) + ( z * z ) );
}

//-----------------------------------------------------------------------------------------------
float Vec3::GetLengthXY() const
{
    return sqrtf( ( x * x ) + ( y * y ) );
}

//-----------------------------------------------------------------------------------------------
float Vec3::GetLengthSquared() const
{
    return ( x * x ) + ( y * y ) + ( z * z );
}

//-----------------------------------------------------------------------------------------------
float Vec3::GetLengthXYSquared() const
{
    return ( x * x ) + ( y * y );
}

//-----------------------------------------------------------------------------------------------
float Vec3::GetOrientationAboutZDegrees() const
{
    float orientationAboutZRadians = atan2f( y, x );

    return ConvertRadiansToDegrees( orientationAboutZRadians );
}

//-----------------------------------------------------------------------------------------------
float Vec3::GetOrientationAboutZRadians() const
{
    return atan2f( y, x );
}

//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::GetRotatedAboutZDegrees( float rotationDegreesAboutZ ) const
{
    float rotatedDegreesAboutZ = rotationDegreesAboutZ + GetOrientationAboutZDegrees();
    float rotatedRadiansAboutZ = ConvertDegreesToRadians( rotatedDegreesAboutZ );

    float lengthXY = GetLengthXY();

    float newX = cosf( rotatedRadiansAboutZ ) * lengthXY;
    float newY = sinf( rotatedRadiansAboutZ ) * lengthXY;

    return Vec3( newX, newY, z );
}

//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::GetRotatedAboutZRadians( float rotationRadiansAboutZ ) const
{
    float rotatedRadiansAboutZ = rotationRadiansAboutZ + GetOrientationAboutZRadians();

    float lengthXY = GetLengthXY();

    float newX = cosf( rotatedRadiansAboutZ ) * lengthXY;
    float newY = sinf( rotatedRadiansAboutZ ) * lengthXY;

    return Vec3( newX, newY, z );
}

//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::GetClampedToMaxLength( float maxLength ) const
{
    float lengthXY    = GetLengthXY();
    float scaleFactor = maxLength / lengthXY;

    return lengthXY <= maxLength ? Vec3( x, y, z ) : Vec3( x, y, z ) * scaleFactor;
}

//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::GetNormalized() const
{
    float length = GetLength();
    if ( length <= 0.f )
    {
        return Vec3::ZERO;
    }

    return Vec3( x, y, z ) / length;
}

//-----------------------------------------------------------------------------------------------
void Vec3::Normalize()
{
    float length = GetLength();
    if ( length <= 0.f )
    {
        x = 0.f;
        y = 0.f;
        z = 0.f;
        return;
    }

    float inverseLength = 1.f / length;
    x *= inverseLength;
    y *= inverseLength;
    z *= inverseLength;
}

//-----------------------------------------------------------------------------------------------
void Vec3::SetFromText( char const* text )
{
    Strings result = SplitStringOnDelimiter( text, ',' );
    if ( static_cast< int >( result.size() ) != 3 )
    {
        x = 0.f;
        y = 0.f;
        z = 0.f;
        return;
    }

    x = static_cast< float >( atof( result[ 0 ].c_str() ) );
    y = static_cast< float >( atof( result[ 1 ].c_str() ) );
    z = static_cast< float >( atof( result[ 2 ].c_str() ) );
}

//-----------------------------------------------------------------------------------------------
bool Vec3::operator==( Vec3 const& compare ) const
{
    return x == compare.x && y == compare.y && z == compare.z;
}

//-----------------------------------------------------------------------------------------------
bool Vec3::operator!=( Vec3 const& compare ) const
{
    return x != compare.x || y != compare.y || z != compare.z;
}

//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::operator+( Vec3 const& vecToAdd ) const
{
    return Vec3( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z );
}

//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::operator-( Vec3 const& vecToSubtract ) const
{
    return Vec3( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z );
}

//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::operator-() const
{
    return Vec3( -x, -y, -z );
}

//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::operator*( float uniformScale ) const
{
    return Vec3( x * uniformScale, y * uniformScale, z * uniformScale );
}

//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::operator*( Vec3 const& vecToMultiply ) const
{
    return Vec3( x * vecToMultiply.x, y * vecToMultiply.y, z * vecToMultiply.z );
}

//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::operator/( float inverseScale ) const
{
    return Vec3( x / inverseScale, y / inverseScale, z / inverseScale );
}

//-----------------------------------------------------------------------------------------------
void Vec3::operator+=( Vec3 const& vecToAdd )
{
    x += vecToAdd.x;
    y += vecToAdd.y;
    z += vecToAdd.z;
}

//-----------------------------------------------------------------------------------------------
void Vec3::operator-=( Vec3 const& vecToSubtract )
{
    x -= vecToSubtract.x;
    y -= vecToSubtract.y;
    z -= vecToSubtract.z;
}

//-----------------------------------------------------------------------------------------------
void Vec3::operator*=( float const uniformScale )
{
    x *= uniformScale;
    y *= uniformScale;
    z *= uniformScale;
}

//-----------------------------------------------------------------------------------------------
void Vec3::operator/=( float const uniformDivisor )
{
    x /= uniformDivisor;
    y /= uniformDivisor;
    z /= uniformDivisor;
}

//-----------------------------------------------------------------------------------------------
void Vec3::operator=( Vec3 const& copyFrom )
{
    x = copyFrom.x;
    y = copyFrom.y;
    z = copyFrom.z;
}

//-----------------------------------------------------------------------------------------------
Vec3 const operator*( float uniformScale, Vec3 const& vecToScale )
{
    return vecToScale * uniformScale;
}

Vec3 const Vec3::MakeFromPolarDegrees( float yawDegrees, float pitchDegrees, float length /*= 1.0f */ )
{
    return Vec3(
        length * CosDegrees( yawDegrees ) * CosDegrees( pitchDegrees ),
        length * SinDegrees( yawDegrees ) * CosDegrees( pitchDegrees ),
        length * -SinDegrees( pitchDegrees ) );
}

//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::MakeFromPolarRadians( float yawRadians, float pitchRadians, float length /*= 1.0f */ )
{
    return Vec3(
        cosf( yawRadians ) * cosf( pitchRadians ) * length,
        sinf( yawRadians ) * cosf( pitchRadians ) * length,
        -sinf( pitchRadians ) * length );
}