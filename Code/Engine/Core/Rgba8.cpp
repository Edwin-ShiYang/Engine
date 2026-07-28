#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

//-----------------------------------------------------------------------------------------------
Rgba8 const Rgba8::WHITE( 255, 255, 255 );
Rgba8 const Rgba8::BLACK( 0, 0, 0 );
Rgba8 const Rgba8::RED( 255, 0, 0 );
Rgba8 const Rgba8::CYAN( 0, 255, 255 );
Rgba8 const Rgba8::GREEN( 0, 255, 0 );
Rgba8 const Rgba8::BLUE( 0, 0, 255 );
Rgba8 const Rgba8::YELLOW( 255, 255, 0 );
Rgba8 const Rgba8::MAGENTA( 255, 0, 255 );
Rgba8 const Rgba8::GREY( 211, 211, 211 );
Rgba8 const Rgba8::TRANSPARENT_BLACK( 0, 0, 0, 128 );
Rgba8 const Rgba8::ORANGE( 255, 165, 0 );
Rgba8 const Rgba8::PEACH( 255, 218, 185 );

// Catppuccin
Rgba8 const Rgba8::CATPPUCCIN_RED( 237, 135, 150 );
Rgba8 const Rgba8::CATPPUCCIN_PINK( 245, 194, 231 );
Rgba8 const Rgba8::CATPPUCCIN_YELLOW( 238, 212, 159 );
Rgba8 const Rgba8::CATPPUCCIN_GREEN( 166, 218, 149 );
Rgba8 const Rgba8::CATPPUCCIN_SKY( 137, 220, 235 );
Rgba8 const Rgba8::CATPPUCCIN_PEACH( 250, 179, 135 );
Rgba8 const Rgba8::CATPPUCCIN_BLUE( 138, 173, 244 );
Rgba8 const Rgba8::CATPPUCCIN_MAUVE( 202, 158, 230 );
Rgba8 const Rgba8::CATPPUCCIN_TEAL( 139, 213, 202 );

//-----------------------------------------------------------------------------------------------
Rgba8::Rgba8( unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte )
    : r( redByte )
    , g( greenByte )
    , b( blueByte )
    , a( alphaByte )
{
}

//-----------------------------------------------------------------------------------------------
void Rgba8::SetFromText( char const* text )
{
    Strings result = SplitStringOnDelimiter( text, ',' );

    if ( result.size() < 3 )
    {
        r = 255;
        g = 255;
        b = 255;
        a = 255;

        return;
    }

    r = static_cast< unsigned char >( atoi( result[ 0 ].c_str() ) );
    g = static_cast< unsigned char >( atoi( result[ 1 ].c_str() ) );
    b = static_cast< unsigned char >( atoi( result[ 2 ].c_str() ) );

    if ( result.size() == 3 )
    {
        a = 255;
    }

    if ( result.size() == 4 )
    {
        a = static_cast< unsigned char >( atoi( result[ 3 ].c_str() ) );
    }
}

//-----------------------------------------------------------------------------------------------
void Rgba8::GetAsFloats( float* colorAsFloats ) const
{
    constexpr float RGB_SCALE = 255.f;
    colorAsFloats[ 0 ]        = static_cast< float >( r ) / RGB_SCALE;
    colorAsFloats[ 1 ]        = static_cast< float >( g ) / RGB_SCALE;
    colorAsFloats[ 2 ]        = static_cast< float >( b ) / RGB_SCALE;
    colorAsFloats[ 3 ]        = static_cast< float >( a ) / RGB_SCALE;
}

//-----------------------------------------------------------------------------------------------
void Rgba8::ScaleRGBA( float colorScale, float alphaScale )
{
    r = (unsigned char)GetClamped( r * colorScale, 0.f, 255.f );
    g = (unsigned char)GetClamped( g * colorScale, 0.f, 255.f );
    b = (unsigned char)GetClamped( b * colorScale, 0.f, 255.f );
    a = (unsigned char)GetClamped( a * alphaScale, 0.f, 255.f );
}

//-----------------------------------------------------------------------------------------------
bool Rgba8::IsRGBEqual( Rgba8 const& compare ) const
{
    return r == compare.r && g == compare.g && b == compare.b;
}

//-----------------------------------------------------------------------------------------------
Rgba8 Interpolate( Rgba8 start, Rgba8 end, float fractionOfEnd )
{
    float r = Interpolate( static_cast< float >( start.r ), static_cast< float >( end.r ), fractionOfEnd );
    float g = Interpolate( static_cast< float >( start.g ), static_cast< float >( end.g ), fractionOfEnd );
    float b = Interpolate( static_cast< float >( start.b ), static_cast< float >( end.b ), fractionOfEnd );
    float a = Interpolate( static_cast< float >( start.a ), static_cast< float >( end.a ), fractionOfEnd );

    return Rgba8( static_cast< unsigned char >( r ), static_cast< unsigned char >( g ), static_cast< unsigned char >( b ), static_cast< unsigned char >( a ) );
}

//-----------------------------------------------------------------------------------------------
bool Rgba8::operator==( Rgba8 const& compare ) const
{
    return r == compare.r && g == compare.g && b == compare.b && a == compare.a;
}