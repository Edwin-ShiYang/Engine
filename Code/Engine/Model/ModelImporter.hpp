#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Model/StaticMesh.hpp"
#include "Engine/Model/SkeletonMesh.hpp"
#include "Engine/Math/Mat44.hpp"

//-----------------------------------------------------------------------------------------------
#include <map>
#include <string>

//-----------------------------------------------------------------------------------------------
class Texture;
class AnimationClip;
struct aiMesh;
struct aiScene;
struct aiNode;

//-----------------------------------------------------------------------------------------------
enum class Axis
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

//-----------------------------------------------------------------------------------------------
struct Material
{
    Texture const* m_diffuseTexture          = nullptr;
    Texture const* m_normalTexture           = nullptr;
    Texture const* m_roughnessTexture        = nullptr;
    Texture const* m_metallicTexture         = nullptr;
    Texture const* m_ambientOcclusionTexture = nullptr;
    Texture const* m_specGlossEmissTexture   = nullptr;
};

//-----------------------------------------------------------------------------------------------
struct Node
{
    std::vector< unsigned int > m_childrenIndices;
    std::vector< unsigned int > m_meshIndexes;
    Mat44                       m_localTransform;
    Mat44                       m_worldTransform;
    std::string                 m_name;
};

//-----------------------------------------------------------------------------------------------
struct Joint
{
    std::string m_name;
    Mat44       m_inverseBindMatrix;
    Mat44       m_worldTransform;
    Mat44       m_localTransform;
    Mat44       m_skinMatrix;
};

//-----------------------------------------------------------------------------------------------
struct Pose
{
public:
    std::vector< Mat44 > m_localTransforms;
    std::vector< Vec3 >  m_translations;
    std::vector< Vec4 >  m_rotations;
    std::vector< Vec3 >  m_scales;
};

//-----------------------------------------------------------------------------------------------
struct Skeleton
{
    std::vector< Joint > m_joints;
};

//-----------------------------------------------------------------------------------------------
struct Model
{
    std::vector< Material > m_materials;
    std::vector< Node >     m_nodes;
};

//-----------------------------------------------------------------------------------------------
struct StaticModel : public Model
{
    StaticMesh m_staticMesh;
};

struct SkeletonModel : public Model
{
    SkeletonMesh m_skeletonMesh;
    Skeleton     m_skeleton;
};

//-----------------------------------------------------------------------------------------------
struct ModelImportOptions
{
    char const* m_diffuseTexturePath          = nullptr;
    char const* m_normalTexturePath           = nullptr;
    char const* m_metallicTexturePath         = nullptr;
    char const* m_ambientOcclusionTexturePath = nullptr;
    char const* m_roughnessTexturePath        = nullptr;
    char const* m_specGlossEmissTexturePath   = nullptr;
};

//-----------------------------------------------------------------------------------------------
class ModelImporter
{
public:
    static StaticModel*                            CreateOrGetStaticModelFromFile( std::string const& filePath );
    static SkeletonModel*                          CreateOrGetSkeletonModelFromFile( std::string const& filePath );
    static AnimationClip*                          CreateOrGetAnimationFromFile( std::string const& filePath );

    static void                                    InitializeStaticMesh( StaticMesh& staticMesh, aiMesh const* const* meshes, unsigned int numMeshes );
    static void                                    InitializeSkeletonMesh( Skeleton const& skeleton, SkeletonMesh& skeletonMesh, aiMesh const* const* meshes, unsigned int numMeshes );
    static void                                    InitializeSkeleton( Skeleton& skeleton, aiMesh const* const* meshes, unsigned int numMeshes );
    static void                                    InitializeMaterials( std::vector< Material >& materials, aiScene const* scene );
    static void                                    TraverseNode( std::vector< Node >& nodes, aiNode const* aiNode, Mat44 const& parentTransform = Mat44() );
    static void                                    CrearModels();
    static Vec3                                    GetEngineDirection( Axis direction );
    static Mat44                                   MakeToEngineMatrix( Axis sourceX, Axis sourceY, Axis sourceZ );
    static int                                     GetBoneIndexByName( Skeleton const& skeleton, std::string const& name );

    static std::map< std::string, StaticModel* >   s_staticModels;
    static std::map< std::string, SkeletonModel* > s_skeletonModels;
};