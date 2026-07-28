#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec4.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"

//-----------------------------------------------------------------------------------------------
struct Vertex_Skinned
{
public:
    Vertex_Skinned() = default;
    explicit Vertex_Skinned( Vec3 const& position, Rgba8 const& color, Vec2 const& uvTexCoords, Vec3 const& tangent, Vec3 const& bitangent, Vec3 const& normal, IntVec4 jointIndices, Vec4 const& jointWeights );

    Vec3    m_position     = Vec3::ZERO;
    Rgba8   m_color        = Rgba8::WHITE;
    Vec2    m_uvTexCoords  = Vec2::ZERO;
    Vec3    m_tangent      = Vec3::ZERO;
    Vec3    m_bitangent    = Vec3::ZERO;
    Vec3    m_normal       = Vec3::ZERO;
    IntVec4 m_jointIndices = IntVec4::ZERO;
    Vec4    m_jointWeights = Vec4::ZERO;
};