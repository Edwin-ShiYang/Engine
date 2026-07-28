#include "Engine/Core/Vertex_Skinned.hpp"

//-----------------------------------------------------------------------------------------------
Vertex_Skinned::Vertex_Skinned( Vec3 const& position, Rgba8 const& color, Vec2 const& uvTexCoords, Vec3 const& tangent, Vec3 const& bitangent, Vec3 const& normal, IntVec4 jointIndices, Vec4 const& jointWeights )
    : m_position( position )
    , m_color( color )
    , m_uvTexCoords( uvTexCoords )
    , m_tangent( tangent )
    , m_bitangent( bitangent )
    , m_normal( normal )
    , m_jointIndices( jointIndices )
    , m_jointWeights( jointWeights )

{
}