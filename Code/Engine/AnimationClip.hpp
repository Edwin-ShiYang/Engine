#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"

//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>

//-----------------------------------------------------------------------------------------------
struct TranslationKeyFrame
{
    float m_time = 0.f;
    Vec3  m_value;
};

//-----------------------------------------------------------------------------------------------
struct RotationKeyFrame
{
    float m_time = 0.f;
    Vec4  m_value;
};

//-----------------------------------------------------------------------------------------------
struct ScaleKeyFrame
{
    float m_time = 0.f;
    Vec3  m_value;
};

//-----------------------------------------------------------------------------------------------
struct AnimationTrack
{
    std::string                        m_boneName;
    std::string                        m_interpolation;
    std::vector< TranslationKeyFrame > m_translationKeyFrames;
    std::vector< RotationKeyFrame >    m_rotationKeyFrames;
    std::vector< ScaleKeyFrame >       m_scaleKeyFrames;
};

//-----------------------------------------------------------------------------------------------
class AnimationClip
{
public:
    AnimationClip();
    ~AnimationClip();

    Vec3                          GetSampleTranslation( float startTime, float endTime, Vec3 const& startValue, Vec3 endValue, float sampleTime ) const;
    Vec4                          GetSampleRotation( float startTime, float endTime, Vec4 const& startValue, Vec4 endValue, float sampleTime ) const;
    Vec3                          GetSampleScale( float startTime, float endTime, Vec3 const& startValue, Vec3 endValue, float sampleTime ) const;

    std::string                   m_name;
    std::vector< AnimationTrack > m_tracks;
    bool                          m_isLooping      = false;
    float                         m_duration       = 0.f;
    float                         m_ticksPerSecond = 0.f;
};