#pragma once

#include "Math/Vec3.hpp"
#include "Core/Rgba8.hpp"

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
    void Spawn( Particle const& particle );
    void Update( float deltaSeconds );
    void Render() const;

public:
    Vec3 m_position = Vec3::ZERO;

private:
    std::vector< Particle >
        m_particles;
};