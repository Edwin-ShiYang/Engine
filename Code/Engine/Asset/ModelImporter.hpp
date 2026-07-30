#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Asset/StaticMesh.hpp"
#include "Engine/Math/Mat44.hpp"

//-----------------------------------------------------------------------------------------------
#include <map>
#include <string>

//-----------------------------------------------------------------------------------------------
class Texture;
struct aiMesh;
struct aiScene;
struct aiNode;

//-----------------------------------------------------------------------------------------------
enum class ModelFormat
{
    OBJ,
    FBX,
    GLTF,
    GLB,
    Unknown
};

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
class Model
{
public:
    Model()  = default;
    ~Model() = default;

    ModelFormat             m_format = ModelFormat::Unknown;
    StaticMesh              m_staticMesh;
    std::vector< Material > m_materials;
    std::vector< Node >     m_nodes;
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
    static Model*                          CreateOrGetModelFromFile( std::string const& filePath, ModelFormat const& format, ModelImportOptions const& options = {} );
    static Model const&                    GetLoadedModelByName( std::string name );
    static void                            InitializeStaticMesh( StaticMesh& staticMesh, aiMesh const* const* meshes, unsigned int numMeshes );
    static void                            InitializeMaterials( std::vector< Material >& materials, aiScene const* scene, char const* fileDirectory, ModelImportOptions const& options );
    static void                            TraverseNode( std::vector< Node >& nodes, aiNode const* aiNode, Mat44 const& parentTransform = Mat44() );
    static void                            CrearModels();
    static Vec3                            GetEngineDirection( Axis direction );
    static Mat44                           MakeToEngineMatrix( Axis sourceX, Axis sourceY, Axis sourceZ );

    static std::map< std::string, Model* > s_models;
};