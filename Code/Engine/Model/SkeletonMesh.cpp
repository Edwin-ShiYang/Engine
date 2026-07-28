#include "Engine/Model/SkeletonMesh.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Model/ModelImporter.hpp"

//-----------------------------------------------------------------------------------------------
void SkeletonMesh::RenderMesh( SkeletonModel const* skeletonModel, Mat44 const& modelToWorldTransform ) const
{
    std::vector< Node > const&                nodes     = skeletonModel->m_nodes;
    std::vector< Material > const&            materials = skeletonModel->m_materials;

    std::vector< SkeletonMeshSection > const& staticMeshSections = m_sections;

    for ( int nodeIndex = 0; nodeIndex < static_cast< int >( nodes.size() ); ++nodeIndex )
    {
        Node node = nodes[ nodeIndex ];
        for ( int meshIndex = 0; meshIndex < static_cast< int >( node.m_meshIndexes.size() ); ++meshIndex )
        {
            Mat44 finalMatrix = modelToWorldTransform;
            finalMatrix.Append( node.m_globalTransform );

            unsigned int   meshSectionIndex = node.m_meshIndexes[ meshIndex ];
            unsigned int   materialIndex    = staticMeshSections[ meshSectionIndex ].m_materialIndex;

            Texture const* diffuseTexture       = materials[ materialIndex ].m_diffuseTexture;
            Texture const* normalTexture        = materials[ materialIndex ].m_normalTexture;
            Texture const* specGlossEmitTexture = materials[ materialIndex ].m_specGlossEmissTexture;

            VertexBuffer*  vertexBuffer = staticMeshSections[ meshSectionIndex ].m_vertexBuffer;
            IndexBuffer*   indexbuffer  = staticMeshSections[ meshSectionIndex ].m_indexBuffer;

            g_engine->m_render->BindTexture( diffuseTexture, ShaderResourceSlot ::DIFFUSE );
            g_engine->m_render->BindTexture( normalTexture, ShaderResourceSlot ::NORMAL );
            g_engine->m_render->BindTexture( g_defaultMetallicTexture, ShaderResourceSlot ::METALLIC );
            g_engine->m_render->BindTexture( g_defaultRoughnessTexture, ShaderResourceSlot ::ROUGHNESS );
            g_engine->m_render->BindTexture( g_defaultAmbientOcclusionTexture, ShaderResourceSlot ::AMBIENT_OCCLUSION );
            g_engine->m_render->BindTexture( specGlossEmitTexture, ShaderResourceSlot ::SPEC_GLOSS_EMIT );

            g_engine->m_render->SetModelConstants( finalMatrix );
            g_engine->m_render->DrawIndexedVertexBuffer( vertexBuffer, indexbuffer, static_cast< unsigned int >( staticMeshSections[ meshSectionIndex ].m_indices.size() ) );

            g_engine->m_render->UnbindTexture( ShaderResourceSlot ::DIFFUSE );
            g_engine->m_render->UnbindTexture( ShaderResourceSlot ::NORMAL );
            g_engine->m_render->UnbindTexture( ShaderResourceSlot ::METALLIC );
            g_engine->m_render->UnbindTexture( ShaderResourceSlot ::ROUGHNESS );
            g_engine->m_render->UnbindTexture( ShaderResourceSlot ::AMBIENT_OCCLUSION );
            g_engine->m_render->UnbindTexture( ShaderResourceSlot ::SPEC_GLOSS_EMIT );
        }
    }
}