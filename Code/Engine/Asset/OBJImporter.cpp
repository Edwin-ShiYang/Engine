#include "Engine/Asset/OBJImporter.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Mat44.hpp"

#pragma comment( lib, "assimp-vc143-mt.lib" )
#include "../Core/Engine.hpp"
#include "ThirdParty/assimp/Importer.hpp"
#include "ThirdParty/assimp/postprocess.h"
#include "ThirdParty/assimp/scene.h"

//-----------------------------------------------------------------------------------------------
std::map< std::string, OBJModel > OBJImporter::s_models;

//-----------------------------------------------------------------------------------------------
bool OBJImporter::CreateOrGetModelFromFile( std::string modelPath, std::string diffusePath, std::string normalPath, std::string sgePath )
{
    std::map< std::string, OBJModel >::const_iterator found = s_models.find( modelPath );
    if ( found != s_models.end() )
    {
        return true;
    }

    OBJModel         obj;
    Assimp::Importer importer;
    importer.SetPropertyFloat( AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, 1.f );
    aiScene const* scene = importer.ReadFile( modelPath, aiProcess_Triangulate | aiProcess_GlobalScale );

    GUARANTEE_OR_DIE( scene, Stringf( "Failed to load obj file: %s", modelPath.c_str() ) );
    GUARANTEE_OR_DIE( scene->mNumMeshes > 0, Stringf( "File is empty: %s", modelPath.c_str() ) );

    for ( unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex )
    {
        aiMesh const* mesh         = scene->mMeshes[ meshIndex ];
        unsigned int  vertexOffset = static_cast< unsigned int >( obj.m_vertices.size() );

        for ( unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex )
        {
            Vec3 position = Vec3( mesh->mVertices[ vertexIndex ].x, mesh->mVertices[ vertexIndex ].y, mesh->mVertices[ vertexIndex ].z );
            Vec2 uv       = Vec2::ZERO;
            Vec3 normal   = Vec3::ZERO;

            if ( mesh->mTextureCoords[ 0 ] )
            {
                uv = Vec2( mesh->mTextureCoords[ 0 ][ vertexIndex ].x, mesh->mTextureCoords[ 0 ][ vertexIndex ].y );
            }

            if ( mesh->mNormals )
            {
                normal = Vec3( mesh->mNormals[ vertexIndex ].x, mesh->mNormals[ vertexIndex ].y, mesh->mNormals[ vertexIndex ].z );
            }

            obj.m_vertices.push_back( Vertex( position, Rgba8::WHITE, uv, Vec3( 1.f, 1.f, 1.f ), Vec3( 1.f, 1.f, 1.f ), normal ) );
        }

        for ( unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex )
        {
            aiFace const& face = mesh->mFaces[ faceIndex ];
            obj.m_indices.push_back( vertexOffset + face.mIndices[ 0 ] );
            obj.m_indices.push_back( vertexOffset + face.mIndices[ 1 ] );
            obj.m_indices.push_back( vertexOffset + face.mIndices[ 2 ] );
        }
    }

    obj.m_diffuse = diffusePath.empty() ? g_engine->m_render->m_defaultDiffuseTexture : g_engine->m_render->CreateOrGetTextureFromFile( diffusePath.c_str() );
    obj.m_normal  = normalPath.empty() ? obj.m_normal = g_engine->m_render->m_defaultNormalTexture : g_engine->m_render->CreateOrGetTextureFromFile( normalPath.c_str() );
    obj.m_sge     = sgePath.empty() ? obj.m_sge = g_engine->m_render->m_defaultSGETexture : g_engine->m_render->CreateOrGetTextureFromFile( sgePath.c_str() );

    s_models[ modelPath ] = obj;
    return true;
}

//-----------------------------------------------------------------------------------------------
OBJModel const& OBJImporter::GetLoadedModelByName( std::string name )
{
    std::map< std::string, OBJModel >::const_iterator iterator = s_models.find( name );
    GUARANTEE_OR_DIE( iterator != s_models.end(), "The Model is  not found: " + name );
    return iterator->second;
}