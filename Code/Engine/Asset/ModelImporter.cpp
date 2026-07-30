#include "Engine/Asset/ModelImporter.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Vec4.hpp"

//-----------------------------------------------------------------------------------------------
#pragma comment( lib, "assimp-vc143-mt.lib" )
#include "../Core/VertexUtils.hpp"
#include "ThirdParty/assimp/Importer.hpp"
#include "ThirdParty/assimp/postprocess.h"
#include "ThirdParty/assimp/scene.h"
#include "ThirdParty/stb/stb_image.h"

//-----------------------------------------------------------------------------------------------
std::map< std::string, Model* > ModelImporter::s_models;

//-----------------------------------------------------------------------------------------------
static Mat44                    GetMatrixFromFile( aiMatrix4x4 const& transform )
{
    aiMatrix4x4 m = transform;
    m.Transpose();

    Mat44 result = Mat44( Vec4( m.a1, m.a2, m.a3, m.a4 ),
                          Vec4( m.b1, m.b2, m.b3, m.b4 ),
                          Vec4( m.c1, m.c2, m.c3, m.c4 ),
                          Vec4( m.d1, m.d2, m.d3, m.d4 ) );
    return result;
}

//-----------------------------------------------------------------------------------------------
Model* ModelImporter::CreateOrGetModelFromFile( std::string const& filePath, ModelFormat const& format, ModelImportOptions const& options )
{
    std::map< std::string, Model* >::const_iterator found = s_models.find( filePath );
    if ( found != s_models.end() )
    {
        return found->second;
    }

    Assimp::Importer importer;
    importer.SetPropertyFloat( AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, 1.f );
    aiScene const* scene = importer.ReadFile( filePath, aiProcess_Triangulate | aiProcess_GlobalScale | aiProcess_CalcTangentSpace );
    GUARANTEE_OR_DIE( scene, Stringf( "ModelImporter::CreateOrGetModelFromFile - Failed to load file: %s", filePath.c_str() ) );

    Model* model    = new Model();
    model->m_format = format;

    InitializeStaticMesh( model->m_staticMesh, scene->mMeshes, scene->mNumMeshes );
    InitializeMaterials( model->m_materials, scene, filePath.c_str(), options );
    TraverseNode( model->m_nodes, scene->mRootNode, Mat44() );

    s_models[ filePath ] = model;
    return model;
}

//-----------------------------------------------------------------------------------------------
Model const& ModelImporter::GetLoadedModelByName( std::string name )
{
    std::map< std::string, Model* >::const_iterator iterator = s_models.find( name );
    GUARANTEE_OR_DIE( iterator != s_models.end(), "The Model is  not found: " + name );
    return *iterator->second;
}

//-----------------------------------------------------------------------------------------------
void ModelImporter::InitializeStaticMesh( StaticMesh& staticMesh, aiMesh const* const* meshes, unsigned int numMeshes )
{
    staticMesh.m_sections.resize( numMeshes );
    for ( unsigned int i = 0; i < numMeshes; ++i )
    {
        aiMesh const* mesh = meshes[ i ];
        staticMesh.m_sections[ i ].m_vertices.resize( mesh->mNumVertices );

        // Vertex
        for ( unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex )
        {
            Vec3 position                                                   = Vec3( mesh->mVertices[ vertexIndex ].x, mesh->mVertices[ vertexIndex ].y, mesh->mVertices[ vertexIndex ].z );
            staticMesh.m_sections[ i ].m_vertices[ vertexIndex ].m_position = position;

            if ( mesh->mTextureCoords[ 0 ] )
            {
                Vec2 uv = Vec2( mesh->mTextureCoords[ 0 ][ vertexIndex ].x, mesh->mTextureCoords[ 0 ][ vertexIndex ].y );

                staticMesh.m_sections[ i ].m_vertices[ vertexIndex ].m_uvTexCoords = uv;
            }

            if ( mesh->HasNormals() )
            {
                Vec3 normal                                                   = Vec3( mesh->mNormals[ vertexIndex ].x, mesh->mNormals[ vertexIndex ].y, mesh->mNormals[ vertexIndex ].z );
                staticMesh.m_sections[ i ].m_vertices[ vertexIndex ].m_normal = normal;
            }
        }

        // Index
        for ( unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex )
        {
            aiFace const& face = mesh->mFaces[ faceIndex ];
            staticMesh.m_sections[ i ].m_indices.push_back( face.mIndices[ 0 ] );
            staticMesh.m_sections[ i ].m_indices.push_back( face.mIndices[ 1 ] );
            staticMesh.m_sections[ i ].m_indices.push_back( face.mIndices[ 2 ] );
        }

        GenerateTangentsFromUVs( staticMesh.m_sections[ i ].m_vertices, staticMesh.m_sections[ i ].m_indices );

        staticMesh.m_sections[ i ].CreateVertexBuffer();
        staticMesh.m_sections[ i ].CreateIndexBuffer();
        staticMesh.m_sections[ i ].m_materialIndex = mesh->mMaterialIndex;
    }
}

//-----------------------------------------------------------------------------------------------
void ModelImporter::InitializeMaterials( std::vector< Material >& materials, aiScene const* scene, char const* fileDirectory, ModelImportOptions const& options )
{
    for ( unsigned int materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex )
    {
        aiMaterial const* material = scene->mMaterials[ materialIndex ];
        Material          mat      = Material();

        aiString          diffusePath;
        if ( material->GetTexture( aiTextureType_DIFFUSE, 0, &diffusePath ) == AI_SUCCESS )
        {
            aiTexture const* texture = scene->GetEmbeddedTexture( diffusePath.C_Str() );
            if ( texture )
            {
                int            width, height, channels;
                unsigned char* decodedData = stbi_load_from_memory( reinterpret_cast< unsigned char const* >( texture->pcData ), texture->mWidth, &width, &height, &channels, 4 );
                GUARANTEE_OR_DIE( decodedData, "stbi_load_from_memory failed" );

                Image image          = Image( IntVec2( width, height ), decodedData, 4 );
                mat.m_diffuseTexture = g_engine->m_render->CreateTextureFromImage( image );
                stbi_image_free( decodedData );
            }
        }

        aiString normalPath;
        if ( material->GetTexture( aiTextureType_NORMALS, 0, &normalPath ) == AI_SUCCESS )
        {
            aiTexture const* texture = scene->GetEmbeddedTexture( normalPath.C_Str() );
            if ( texture )
            {
                int            width, height, channels;
                unsigned char* decodedData = stbi_load_from_memory( reinterpret_cast< unsigned char const* >( texture->pcData ), texture->mWidth, &width, &height, &channels, 4 );
                GUARANTEE_OR_DIE( decodedData, "stbi_load_from_memory failed" );

                Image image         = Image( IntVec2( width, height ), decodedData, 4 );
                mat.m_normalTexture = g_engine->m_render->CreateTextureFromImage( image );
                stbi_image_free( decodedData );
            }
        }

        aiString emissivePath;
        if ( material->GetTexture( aiTextureType_EMISSIVE, 0, &emissivePath ) == AI_SUCCESS )
        {
            aiTexture const* texture = scene->GetEmbeddedTexture( emissivePath.C_Str() );
            if ( texture )
            {
                int            width, height, channels;
                unsigned char* decodedData = stbi_load_from_memory( reinterpret_cast< unsigned char const* >( texture->pcData ), static_cast< int >( texture->mWidth ), &width, &height, &channels, 4 );
                GUARANTEE_OR_DIE( decodedData, "Failed to decode embedded emissive texture" );

                Image image                 = Image( IntVec2( width, height ), decodedData, 4 );
                mat.m_specGlossEmissTexture = g_engine->m_render->CreateTextureFromImage( image );

                stbi_image_free( decodedData );
            }
        }

        materials.push_back( mat );
    }
}

//-----------------------------------------------------------------------------------------------
void ModelImporter::TraverseNode( std::vector< Node >& nodes, aiNode const* aiNode, Mat44 const& parentTransform )
{
    Mat44 localTransform = GetMatrixFromFile( aiNode->mTransformation );
    Mat44 worldTransform = parentTransform;
    worldTransform.Append( localTransform );

    Node node;
    node.m_name           = aiNode->mName.C_Str();
    node.m_localTransform = localTransform;
    node.m_worldTransform = worldTransform;

    unsigned int nodeIndex = static_cast< unsigned int >( nodes.size() );
    nodes.push_back( node );

    for ( unsigned int i = 0; i < aiNode->mNumMeshes; ++i )
    {
        unsigned int meshIndex = aiNode->mMeshes[ i ];
        nodes[ nodeIndex ].m_meshIndexes.push_back( meshIndex );
    }

    for ( unsigned int i = 0; i < aiNode->mNumChildren; ++i )
    {
        nodes[ nodeIndex ].m_childrenIndices.push_back( static_cast< unsigned int >( nodes.size() ) );
        TraverseNode( nodes, aiNode->mChildren[ i ], worldTransform );
    }
}

//-----------------------------------------------------------------------------------------------
void ModelImporter::CrearModels()
{
    for ( auto& pair : s_models )
    {
        delete pair.second;
        pair.second = nullptr;
    }

    s_models.clear();
}

//-----------------------------------------------------------------------------------------------
Vec3 ModelImporter::GetEngineDirection( Axis direction )
{
    switch ( direction )
    {
        case Axis::FORWARD: return Vec3( 1.f, 0.f, 0.f );
        case Axis::BACKWARD: return Vec3( -1.f, 0.f, 0.f );
        case Axis::LEFT: return Vec3( 0.f, 1.f, 0.f );
        case Axis::RIGHT: return Vec3( 0.f, -1.f, 0.f );
        case Axis::UP: return Vec3( 0.f, 0.f, 1.f );
        case Axis::DOWN: return Vec3( 0.f, 0.f, -1.f );
    }

    ERROR_AND_DIE( "Invalid AxisDirection" );
}

//-----------------------------------------------------------------------------------------------
Mat44 ModelImporter::MakeToEngineMatrix( Axis sourceX, Axis sourceY, Axis sourceZ )
{
    Mat44 matrix;
    matrix.SetIJK3D(
        GetEngineDirection( sourceX ),
        GetEngineDirection( sourceY ),
        GetEngineDirection( sourceZ ) );

    return matrix;
}