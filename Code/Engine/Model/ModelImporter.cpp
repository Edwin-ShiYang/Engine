#include "Engine/Model/ModelImporter.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Vec4.hpp"

//-----------------------------------------------------------------------------------------------
#pragma comment( lib, "assimp-vc143-mt.lib" )
#include "../Core/VertexUtils.hpp"
#include "../AnimationClip.hpp"
#include "ThirdParty/assimp/Importer.hpp"
#include "ThirdParty/assimp/postprocess.h"
#include "ThirdParty/assimp/scene.h"
#include "ThirdParty/stb/stb_image.h"

//-----------------------------------------------------------------------------------------------
static Mat44 GetMatrixFromFile( aiMatrix4x4 const& transform )
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
StaticModel* ModelImporter::CreateOrGetStaticModelFromFile( std::string const& filePath )
{
    std::map< std::string, StaticModel* >::const_iterator found = s_staticModels.find( filePath );
    if ( found != s_staticModels.end() )
    {
        return found->second;
    }

    Assimp::Importer importer;
    importer.SetPropertyFloat( AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, 1.f );
    aiScene const* scene = importer.ReadFile( filePath, aiProcess_Triangulate | aiProcess_GlobalScale | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals );
    GUARANTEE_OR_DIE( scene, Stringf( "ModelImporter::CreateOrGetModelFromFile - Failed to load file: %s", filePath.c_str() ) );

    StaticModel* model = new StaticModel();

    InitializeStaticMesh( model->m_staticMesh, scene->mMeshes, scene->mNumMeshes );
    InitializeMaterials( model->m_materials, scene );
    TraverseNode( model->m_nodes, scene->mRootNode, Mat44() );

    s_staticModels[ filePath ] = model;
    return model;
}

//-----------------------------------------------------------------------------------------------
SkeletonModel* ModelImporter::CreateOrGetSkeletonModelFromFile( std::string const& filePath )
{
    std::map< std::string, SkeletonModel* >::const_iterator found = s_skeletonModels.find( filePath );
    if ( found != s_skeletonModels.end() )
    {
        return found->second;
    }

    Assimp::Importer importer;
    importer.SetPropertyFloat( AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, 1.f );
    aiScene const* scene = importer.ReadFile( filePath, aiProcess_Triangulate | aiProcess_GlobalScale | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals );
    GUARANTEE_OR_DIE( scene, Stringf( "ModelImporter::CreateOrGetSkeletonModelFromFile - Failed to load file: %s", filePath.c_str() ) );

    SkeletonModel* model = new SkeletonModel();

    InitializeSkeleton( model->m_skeleton, scene->mMeshes );
    InitializeSkeletonMesh( model->m_skeleton, model->m_skeletonMesh, scene->mMeshes, scene->mNumMeshes );
    InitializeMaterials( model->m_materials, scene );
    TraverseNode( model->m_nodes, scene->mRootNode, Mat44() );

    s_skeletonModels[ filePath ] = model;
    return model;
}

//-----------------------------------------------------------------------------------------------
AnimationClip* ModelImporter::CreateOrGetAnimationFromFile( std::string const& filePath )
{
    Assimp::Importer animImporter;
    animImporter.SetPropertyBool( AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false );
    animImporter.SetPropertyFloat( AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, 1.f );

    aiScene const*     animScene     = animImporter.ReadFile( filePath, aiProcess_Triangulate | aiProcess_GlobalScale | aiProcess_MakeLeftHanded | aiProcess_TransformUVCoords | aiProcess_CalcTangentSpace );
    aiAnimation const* animation     = animScene->mAnimations[ 0 ];
    AnimationClip*     animationClip = new AnimationClip();
    animationClip->m_name            = animation->mName.C_Str();
    animationClip->m_duration        = static_cast< float >( animation->mDuration );
    animationClip->m_ticksPerSecond  = static_cast< float >( animation->mTicksPerSecond );
    animationClip->m_tracks.resize( animation->mNumChannels );

    for ( unsigned int i = 0; i < animation->mNumChannels; ++i )
    {
        aiNodeAnim* channel                     = animation->mChannels[ i ];
        animationClip->m_tracks[ i ].m_boneName = channel->mNodeName.C_Str();
        for ( unsigned int translationIndex = 0; translationIndex < channel->mNumPositionKeys; ++translationIndex )
        {
            aiVectorKey         key = channel->mPositionKeys[ translationIndex ];
            TranslationKeyFrame translationKeyFrame;
            translationKeyFrame.m_time  = static_cast< float >( key.mTime / animationClip->m_ticksPerSecond );
            translationKeyFrame.m_value = Vec3( key.mValue.x, key.mValue.y, key.mValue.z );
            animationClip->m_tracks[ i ].m_translationKeyFrames.push_back( translationKeyFrame );
        }

        for ( unsigned int rotationIndex = 0; rotationIndex < channel->mNumRotationKeys; ++rotationIndex )
        {
            aiQuatKey        key = channel->mRotationKeys[ rotationIndex ];
            RotationKeyFrame rotationKeyFrame;
            rotationKeyFrame.m_time  = static_cast< float >( key.mTime / animationClip->m_ticksPerSecond );
            rotationKeyFrame.m_value = Vec4( key.mValue.x, key.mValue.y, key.mValue.z, key.mValue.w );
            animationClip->m_tracks[ i ].m_rotationKeyFrames.push_back( rotationKeyFrame );
        }

        for ( unsigned int scaleIndex = 0; scaleIndex < channel->mNumScalingKeys; ++scaleIndex )
        {
            aiVectorKey   key = channel->mScalingKeys[ scaleIndex ];
            ScaleKeyFrame scaleKeyFrame;
            scaleKeyFrame.m_time  = static_cast< float >( key.mTime / animationClip->m_ticksPerSecond );
            scaleKeyFrame.m_value = Vec3( key.mValue.x, key.mValue.y, key.mValue.z );
            animationClip->m_tracks[ i ].m_scaleKeyFrames.push_back( scaleKeyFrame );
        }
    }
    return animationClip;
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
            staticMesh.m_sections[ i ].m_vertices[ vertexIndex ].m_uvTexCoords = Vec2( mesh->mTextureCoords[ 0 ][ vertexIndex ].x, mesh->mTextureCoords[ 0 ][ vertexIndex ].y );
            staticMesh.m_sections[ i ].m_vertices[ vertexIndex ].m_position    = Vec3( mesh->mVertices[ vertexIndex ].x, mesh->mVertices[ vertexIndex ].y, mesh->mVertices[ vertexIndex ].z );
            staticMesh.m_sections[ i ].m_vertices[ vertexIndex ].m_normal      = Vec3( mesh->mNormals[ vertexIndex ].x, mesh->mNormals[ vertexIndex ].y, mesh->mNormals[ vertexIndex ].z );
            staticMesh.m_sections[ i ].m_vertices[ vertexIndex ].m_tangent     = Vec3( mesh->mTangents[ vertexIndex ].x, mesh->mTangents[ vertexIndex ].y, mesh->mTangents[ vertexIndex ].z );
            staticMesh.m_sections[ i ].m_vertices[ vertexIndex ].m_bitangent   = Vec3( mesh->mBitangents[ vertexIndex ].x, mesh->mBitangents[ vertexIndex ].y, mesh->mBitangents[ vertexIndex ].z );
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
void ModelImporter::InitializeSkeletonMesh( Skeleton const& skeleton, SkeletonMesh& skeletonMesh, aiMesh const* const* meshes, unsigned int numMeshes )
{
    skeletonMesh.m_sections.resize( numMeshes );

    for ( unsigned int i = 0; i < numMeshes; ++i )
    {
        aiMesh const* mesh = meshes[ i ];
        skeletonMesh.m_sections[ i ].m_vertices.resize( mesh->mNumVertices );

        for ( unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex )
        {
            Vec3 position  = Vec3( mesh->mVertices[ vertexIndex ].x, mesh->mVertices[ vertexIndex ].y, mesh->mVertices[ vertexIndex ].z );
            Vec2 uv        = Vec2( mesh->mTextureCoords[ 0 ][ vertexIndex ].x, mesh->mTextureCoords[ 0 ][ vertexIndex ].y );
            Vec3 normal    = Vec3( mesh->mNormals[ vertexIndex ].x, mesh->mNormals[ vertexIndex ].y, mesh->mNormals[ vertexIndex ].z );
            Vec3 tangent   = Vec3( mesh->mTangents[ vertexIndex ].x, mesh->mTangents[ vertexIndex ].y, mesh->mTangents[ vertexIndex ].z );
            Vec3 bitangent = Vec3( mesh->mBitangents[ vertexIndex ].x, mesh->mBitangents[ vertexIndex ].y, mesh->mBitangents[ vertexIndex ].z );

            skeletonMesh.m_sections[ i ].m_vertices[ vertexIndex ].m_position    = position;
            skeletonMesh.m_sections[ i ].m_vertices[ vertexIndex ].m_uvTexCoords = uv;
            skeletonMesh.m_sections[ i ].m_vertices[ vertexIndex ].m_normal      = normal;
            skeletonMesh.m_sections[ i ].m_vertices[ vertexIndex ].m_tangent     = tangent;
            skeletonMesh.m_sections[ i ].m_vertices[ vertexIndex ].m_bitangent   = bitangent;
        }

        for ( unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex )
        {
            aiFace const& face = mesh->mFaces[ faceIndex ];
            skeletonMesh.m_sections[ i ].m_indices.push_back( face.mIndices[ 0 ] );
            skeletonMesh.m_sections[ i ].m_indices.push_back( face.mIndices[ 1 ] );
            skeletonMesh.m_sections[ i ].m_indices.push_back( face.mIndices[ 2 ] );
        }

        for ( unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex )
        {
            aiBone const* bone    = mesh->mBones[ boneIndex ];
            int           boneIdx = GetBoneIndexByName( skeleton, bone->mName.C_Str() );
            if ( boneIdx == -1 )
            {
                continue;
            }

            for ( unsigned int weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex )
            {
                unsigned int    vertexId = bone->mWeights[ weightIndex ].mVertexId;
                Vertex_Skinned& vertex   = skeletonMesh.m_sections[ i ].m_vertices[ vertexId ];
                float           weight   = bone->mWeights[ weightIndex ].mWeight;

                if ( weight <= 0.f )
                {
                    continue;
                }

                if ( vertex.m_jointWeights.x == 0.f )
                {
                    vertex.m_jointIndices.x = boneIdx;
                    vertex.m_jointWeights.x = weight;
                    continue;
                }

                if ( vertex.m_jointWeights.y == 0.f )
                {
                    vertex.m_jointIndices.y = boneIdx;
                    vertex.m_jointWeights.y = weight;
                    continue;
                }

                if ( vertex.m_jointWeights.z == 0.f )
                {
                    vertex.m_jointIndices.z = boneIdx;
                    vertex.m_jointWeights.z = weight;
                    continue;
                }

                if ( vertex.m_jointWeights.w == 0.f )
                {
                    vertex.m_jointIndices.w = boneIdx;
                    vertex.m_jointWeights.w = weight;
                    continue;
                }
            }
        }

        skeletonMesh.m_sections[ i ].CreateVertexBuffer();
        skeletonMesh.m_sections[ i ].CreateIndexBuffer();
        skeletonMesh.m_sections[ i ].m_materialIndex = mesh->mMaterialIndex;
    }
}

//-----------------------------------------------------------------------------------------------
void ModelImporter::InitializeSkeleton( Skeleton& skeleton, aiMesh const* const* meshes )
{
    aiMesh const* mesh = meshes[ 0 ];
    for ( unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex )
    {
        aiBone const* aiBone = mesh->mBones[ boneIndex ];
        Joint         joint;
        joint.m_name              = aiBone->mName.C_Str();
        joint.m_inverseBindMatrix = GetMatrixFromFile( aiBone->mOffsetMatrix );
        skeleton.m_joints.push_back( joint );
    }
}

//-----------------------------------------------------------------------------------------------
void ModelImporter::InitializeMaterials( std::vector< Material >& materials, aiScene const* scene )
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
    for ( auto& pair : s_staticModels )
    {
        delete pair.second;
        pair.second = nullptr;
    }

    for ( auto& pair : s_skeletonModels )
    {
        delete pair.second;
        pair.second = nullptr;
    }

    s_staticModels.clear();
    s_skeletonModels.clear();
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

//-----------------------------------------------------------------------------------------------
int ModelImporter::GetBoneIndexByName( Skeleton const& skeleton, std::string const& name )
{
    for ( int boneIndex = 0; boneIndex < static_cast< int >( skeleton.m_joints.size() ); ++boneIndex )
    {
        if ( skeleton.m_joints[ boneIndex ].m_name == name )
        {
            return boneIndex;
        }
    }
    return -1;
}

//-----------------------------------------------------------------------------------------------
std::map< std::string, StaticModel* >   ModelImporter::s_staticModels;
std::map< std::string, SkeletonModel* > ModelImporter::s_skeletonModels;