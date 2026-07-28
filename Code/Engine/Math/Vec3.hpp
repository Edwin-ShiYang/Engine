#pragma once

//-----------------------------------------------------------------------------------------------
struct Vec3
{
public:
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;

public:
    /* Construction / Destruction */
    ~Vec3()
    {
    }
    Vec3()
    {
    }
    Vec3( Vec3 const& copyFrom );
    explicit Vec3( float initialX, float initialY, float initialZ );

    /* Accessors( const ) */
    float      GetLength() const;
    float      GetLengthXY() const;
    float      GetLengthSquared() const;
    float      GetLengthXYSquared() const;
    float      GetOrientationAboutZDegrees() const;
    float      GetOrientationAboutZRadians() const;
    Vec3 const GetRotatedAboutZDegrees( float rotationDegreesAboutZ ) const;
    Vec3 const GetRotatedAboutZRadians( float rotationRadiansAboutZ ) const;
    Vec3 const GetClampedToMaxLength( float maxLength ) const;
    Vec3 const GetNormalized() const;
    void       Normalize();
    void       SetFromText( char const* text );

    /* Operators ( const ) */
    bool       operator==( Vec3 const& compare ) const;
    bool       operator!=( Vec3 const& compare ) const;
    Vec3 const operator+( Vec3 const& vecToAdd ) const;
    Vec3 const operator-( Vec3 const& vecToSubtract ) const;
    Vec3 const operator-() const;
    Vec3 const operator*( float uniformScale ) const;
    Vec3 const operator*( Vec3 const& vecToMultiply ) const;
    Vec3 const operator/( float inverseScale ) const;

    /* Operators ( self-mutating / non-const ) */
    void operator+=( Vec3 const& vecToAdd );
    void operator-=( Vec3 const& vecToSubtract );
    void operator*=( float const uniformScale );
    void operator/=( float const uniformDivisor );
    void operator=( Vec3 const& copyFrom );

    /* Static */
    static Vec3 const ZERO;
    static Vec3 const WORLD_UP;
    static Vec3 const WORLD_DOWN;
    static Vec3 const WORLD_FORWARD;
    static Vec3 const WORLD_LEFT;

    static Vec3 const MakeFromPolarRadians( float yawRadians, float pitchRadians, float length = 1.0f );
    static Vec3 const MakeFromPolarDegrees( float yawDegrees, float pitchDegrees, float length = 1.0f );

    /* Friends */
    friend Vec3 const operator*( float uniformScale, Vec3 const& vecToScale );  // float * Vec3
};