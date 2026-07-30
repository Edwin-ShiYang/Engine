#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Vertex.hpp"

//-----------------------------------------------------------------------------------------------
#include <vector>

//-----------------------------------------------------------------------------------------------
class VertexBuffer;
class IndexBuffer;

//-----------------------------------------------------------------------------------------------
class StaticMeshSection
{
public:
    StaticMeshSection() = default;
    ~StaticMeshSection();

    void                        CreateVertexBuffer();
    void                        CreateIndexBuffer();

    std::vector< Vertex >       m_vertices;
    std::vector< unsigned int > m_indices;
    VertexBuffer*               m_vertexBuffer  = nullptr;
    IndexBuffer*                m_indexBuffer   = nullptr;
    unsigned int                m_materialIndex = 4294967295u;
};