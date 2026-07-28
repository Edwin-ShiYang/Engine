#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Vertex_Skinned.hpp"

//-----------------------------------------------------------------------------------------------
#include <vector>

//-----------------------------------------------------------------------------------------------
class VertexBuffer;
class IndexBuffer;

//-----------------------------------------------------------------------------------------------
class SkeletonMeshSection
{
public:
    SkeletonMeshSection() = default;
    ~SkeletonMeshSection();

    void                          CreateVertexBuffer();
    void                          CreateIndexBuffer();

    std::vector< Vertex_Skinned > m_vertices;
    std::vector< unsigned int >   m_indices;
    VertexBuffer*                 m_vertexBuffer  = nullptr;
    IndexBuffer*                  m_indexBuffer   = nullptr;
    unsigned int                  m_materialIndex = 4294967295u;
};