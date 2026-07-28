#include "Engine/AnimationClip.hpp"
#include "Engine/Math/MathUtils.hpp"

//-----------------------------------------------------------------------------------------------
AnimationClip::AnimationClip()
{
}

//-----------------------------------------------------------------------------------------------
AnimationClip::~AnimationClip()
{
}

//-----------------------------------------------------------------------------------------------
Vec3 AnimationClip::GetSampleTranslation( float startTime, float endTime, Vec3 const& startValue, Vec3 endValue, float sampleTime ) const
{
    float x = RangeMap( sampleTime, startTime, endTime, startValue.x, endValue.x );
    float y = RangeMap( sampleTime, startTime, endTime, startValue.y, endValue.y );
    float z = RangeMap( sampleTime, startTime, endTime, startValue.z, endValue.z );

    return Vec3( x, y, z );
}

//-----------------------------------------------------------------------------------------------
Vec4 AnimationClip::GetSampleRotation( float startTime, float endTime, Vec4 const& startValue, Vec4 endValue, float sampleTime ) const
{
    float x = RangeMap( sampleTime, startTime, endTime, startValue.x, endValue.x );
    float y = RangeMap( sampleTime, startTime, endTime, startValue.y, endValue.y );
    float z = RangeMap( sampleTime, startTime, endTime, startValue.z, endValue.z );
    float w = RangeMap( sampleTime, startTime, endTime, startValue.w, endValue.w );

    return Vec4( x, y, z, w );
}

//-----------------------------------------------------------------------------------------------
Vec3 AnimationClip::GetSampleScale( float startTime, float endTime, Vec3 const& startValue, Vec3 endValue, float sampleTime ) const
{
    float x = RangeMap( sampleTime, startTime, endTime, startValue.x, endValue.x );
    float y = RangeMap( sampleTime, startTime, endTime, startValue.y, endValue.y );
    float z = RangeMap( sampleTime, startTime, endTime, startValue.z, endValue.z );

    return Vec3( x, y, z );
}