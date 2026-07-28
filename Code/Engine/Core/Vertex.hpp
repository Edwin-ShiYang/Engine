#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

//-----------------------------------------------------------------------------------------------
/*
    P( Position )
    C( Color )
    U( UVs )
    T( Tangent )
    B( Bitangent  )
    N( Normal )
*/
struct Vertex
{
public:
    Vertex() = default;
    explicit Vertex( Vec3 const& position, Rgba8 const& color, Vec2 const& uvTexCoords );
    Vertex( Vec3 const& position, Rgba8 const& color, Vec2 const& uvTexCoords, Vec3 const& tangent, Vec3 const& bitangent, Vec3 const& normal );
    Vertex( float px, float py, float pz, unsigned char r, unsigned char g, unsigned char b, unsigned char a, float u, float v, float tx, float ty, float tz, float bx, float by, float bz, float nx, float ny, float nz );

    bool operator==( Vertex const& rhs ) const
    {
        return m_position == rhs.m_position &&
               m_color == rhs.m_color &&
               m_uvTexCoords == rhs.m_uvTexCoords &&
               m_tangent == rhs.m_tangent &&
               m_bitangent == rhs.m_bitangent &&
               m_normal == rhs.m_normal;
    }

    Vec3  m_position;
    Rgba8 m_color;
    Vec2  m_uvTexCoords;
    Vec3  m_tangent   = Vec3::ZERO;
    Vec3  m_bitangent = Vec3::ZERO;
    Vec3  m_normal    = Vec3::ZERO;
};