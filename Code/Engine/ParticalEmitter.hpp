#pragma once

//----------------------------------------------------------------------------------------------

#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Math/RandomNumberGenerator.hpp"

//----------------------------------------------------------------------------------------------
#include <vector>

//----------------------------------------------------------------------------------------------
struct Particle
{
    Vec3  m_position;
    Vec3  m_velocity;
    float m_age      = 0.f;
    float m_lifetime = 1.f;

    float m_startSize = 1.f;
    float m_endSize   = 1.f;

    Rgba8 m_startColor = Rgba8::WHITE;
    Rgba8 m_endColor   = Rgba8( 255, 255, 255, 0 );
};

//----------------------------------------------------------------------------------------------
class ParticleEmitter
{
public:
    ParticleEmitter( Vec3 const& position );
    ~ParticleEmitter() = default;

    void  Spawn();
    void  Update();
    void  Render() const;

    Mat44 GetModelToWorldTransform() const;

public:
    Vec3        m_position = Vec3::ZERO;
    EulerAngles m_orientation;
    Vec3        m_scale = Vec3( 1.0f, 1.0f, 1.0f );

private:
    std::vector< Particle > m_particles;
    float                   m_spawnTimer    = 0.f;
    float                   m_spawnInterval = 0.02f;
};