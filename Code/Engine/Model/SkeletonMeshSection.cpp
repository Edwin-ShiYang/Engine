#include "Engine/Model/SkeletonMeshSection.hpp"

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

//-----------------------------------------------------------------------------------------------
SkeletonMeshSection::~SkeletonMeshSection()
{
    delete m_vertexBuffer;
    m_vertexBuffer = nullptr;

    delete m_indexBuffer;
    m_indexBuffer = nullptr;
}

//-----------------------------------------------------------------------------------------------
void SkeletonMeshSection::CreateVertexBuffer()
{
    unsigned int vertexBufferStride = sizeof( Vertex_Skinned );
    unsigned int vertexBufferSize   = static_cast< unsigned int >( m_vertices.size() ) * vertexBufferStride;

    m_vertexBuffer = g_engine->m_render->CreateVertexBuffer( vertexBufferSize, vertexBufferStride );
    g_engine->m_render->CopyCPUToGPU( m_vertices.data(), static_cast< unsigned int >( m_vertices.size() ) * sizeof( Vertex_Skinned ), m_vertexBuffer );
}

//-----------------------------------------------------------------------------------------------
void SkeletonMeshSection::CreateIndexBuffer()
{
    unsigned int indexBufferStride = sizeof( unsigned int );
    unsigned int indexBufferSize   = static_cast< unsigned int >( m_indices.size() ) * indexBufferStride;

    m_indexBuffer = g_engine->m_render->CreateIndexBuffer( indexBufferSize, indexBufferStride );
    g_engine->m_render->CopyCPUToGPU( m_indices.data(), static_cast< unsigned int >( m_indices.size() ) * sizeof( unsigned int ), m_indexBuffer );
}