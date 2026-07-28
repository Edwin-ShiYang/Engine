#include "Engine/Model/StaticMesh.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Model/ModelImporter.hpp"

//-----------------------------------------------------------------------------------------------
void StaticMesh::Render( StaticModel const* staticModel, Mat44 const& modelToWorldTransform ) const
{
    std::vector< Node > const&              nodes     = staticModel->m_nodes;
    std::vector< Material > const&          materials = staticModel->m_materials;

    std::vector< StaticMeshSection > const& staticMeshSections = m_sections;

    for ( int nodeIndex = 0; nodeIndex < static_cast< int >( nodes.size() ); ++nodeIndex )
    {
        Node node = nodes[ nodeIndex ];
        for ( int meshIndex = 0; meshIndex < static_cast< int >( node.m_meshIndexes.size() ); ++meshIndex )
        {
            Mat44 finalMatrix = modelToWorldTransform;
            finalMatrix.Append( node.m_worldTransform );

            unsigned int   meshSectionIndex = node.m_meshIndexes[ meshIndex ];
            unsigned int   materialIndex    = staticMeshSections[ meshSectionIndex ].m_materialIndex;

            Texture const* diffuseTexture       = materials[ materialIndex ].m_diffuseTexture;
            Texture const* normalTexture        = materials[ materialIndex ].m_normalTexture;
            Texture const* specGlossEmitTexture = materials[ materialIndex ].m_specGlossEmissTexture;

            VertexBuffer*  vertexBuffer = staticMeshSections[ meshSectionIndex ].m_vertexBuffer;
            IndexBuffer*   indexbuffer  = staticMeshSections[ meshSectionIndex ].m_indexBuffer;

            g_engine->m_render->BindTexture( diffuseTexture, ResourceSlot::DIFFUSE );
            g_engine->m_render->BindTexture( normalTexture, ResourceSlot::NORMAL );
            g_engine->m_render->BindTexture( g_engine->m_render->m_defaultMetallicTexture, ResourceSlot::METALLIC );
            g_engine->m_render->BindTexture( g_engine->m_render->m_defaultRoughnessTexture, ResourceSlot::ROUGHNESS );
            g_engine->m_render->BindTexture( g_engine->m_render->m_defaultAmbientOcclusionTexture, ResourceSlot::AMBIENT_OCCLUSION );
            g_engine->m_render->BindTexture( specGlossEmitTexture, ResourceSlot::SPEC_GLOSS_EMIT );

            g_engine->m_render->SetModelConstants( finalMatrix );
            g_engine->m_render->DrawIndexedVertexBuffer( vertexBuffer, indexbuffer, static_cast< unsigned int >( staticMeshSections[ meshSectionIndex ].m_indices.size() ) );

            g_engine->m_render->UnbindTexture( ResourceSlot::DIFFUSE );
            g_engine->m_render->UnbindTexture( ResourceSlot::NORMAL );
            g_engine->m_render->UnbindTexture( ResourceSlot::METALLIC );
            g_engine->m_render->UnbindTexture( ResourceSlot::ROUGHNESS );
            g_engine->m_render->UnbindTexture( ResourceSlot::AMBIENT_OCCLUSION );
            g_engine->m_render->UnbindTexture( ResourceSlot::SPEC_GLOSS_EMIT );
        }
    }
}