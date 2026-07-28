#pragma once

//----------------------------------------------------------------------------------------------
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Game/EngineBuildPreferences.hpp"

//----------------------------------------------------------------------------------------------
#include "dxgiformat.h"

//----------------------------------------------------------------------------------------------
#include <map>
#include <vector>
#include <array>

//----------------------------------------------------------------------------------------------
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RasterizerState;
struct ID3D11RenderTargetView;
struct ID3D11BlendState;
struct ID3D11SamplerState;
struct ID3D11DepthStencilView;
struct ID3D11DepthStencilState;
struct D3D11_DEPTH_STENCIL_DESC;
struct D3D11_TEXTURE2D_DESC;
struct Rgba8;
struct Vertex;
struct Viewport;
class Camera;
class Window;
class Shader;
class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;
class Image;
class TextureBase;
class RenderTexture;
class DepthRenderTexture;
class CubemapTexture;

//----------------------------------------------------------------------------------------------
#define DX_SAFE_RELEASE( dxObject )    \
    {                                  \
        if ( ( dxObject ) != nullptr ) \
        {                              \
            ( dxObject )->Release();   \
            ( dxObject ) = nullptr;    \
        }                              \
    }

//----------------------------------------------------------------------------------------------
#if defined( OPAQUE )
#undef OPAQUE
#endif

//----------------------------------------------------------------------------------------------
enum class VertexType
{
    VERTEX_PCUTBN,
    VERTEX_SKINNED,
};

//----------------------------------------------------------------------------------------------
enum class DefaultTexture
{
    WHITE,
    DIFFUSE,
    NORMAL,
    SPEC_GLOSS_EMIT
};

//----------------------------------------------------------------------------------------------
enum class BlendMode
{
    ALPHA,
    ADDITIVE,
    OPAQUE,
    COUNT,
};

//----------------------------------------------------------------------------------------------
enum class RasterizerMode
{
    SOLID_CULL_NONE,
    SOLID_CULL_BACK,
    WIREFRAME_CULL_NONE,
    WIREFRAME_CULL_BACK,
    COUNT
};

//----------------------------------------------------------------------------------------------
enum class DepthMode
{
    DISABLED,
    READ_ONLY_ALWAYS,
    READ_WRITE_LESS_EQUAL,
    READ_ONLY_LESS_EQUAL,
    COUNT
};

//----------------------------------------------------------------------------------------------
enum class SamplerMode
{
    POINT_CLAMP,
    BILINEAR_WRAP,
    BILINEAR_CLAMP,
    COUNT,
};

//----------------------------------------------------------------------------------------------
enum class ResourceSlot : int
{
    DIFFUSE = 0,
    NORMAL,
    SPEC_GLOSS_EMIT,
    ROUGHNESS,
    AMBIENT_OCCLUSION,
    METALLIC,
    HDR_SCENE,
    BRIGHT_PASS,
    HORIZONTAL_BLUR,
    VERTICAL_BLUR,
    SHADOWMAP,
    ENVIRONMENTCUBEMAP,
    IRRADIANCE,
    PREFILTERED,
    BRDF_LUT,
    COUNT
};

//----------------------------------------------------------------------------------------------
enum class ShaderType : int
{
    PBRLitStatic = 0,
    PBRLitSkinned,
    ShadowMap,
    COUNT
};

//----------------------------------------------------------------------------------------------
enum class VertexAttributeType
{
    POSITION,
    COLOR,
    UVTEXCOORDS,
    TANGENT,
    BITANGENT,
    NORMAL,
    JOINTINDICES,
    JOINTWEIGHTS
};

//----------------------------------------------------------------------------------------------
struct VertexDescriptorAttribute
{
    VertexAttributeType m_semanticName;
    unsigned int        m_alignedByteOffset;
    DXGI_FORMAT         m_format;

    VertexDescriptorAttribute( VertexAttributeType semanticName, unsigned int alignedByteOffset, DXGI_FORMAT format )
        : m_semanticName( semanticName )
        , m_alignedByteOffset( alignedByteOffset )
        , m_format( format )
    {
    }
};

//----------------------------------------------------------------------------------------------
struct VertexDescriptor
{
    std::vector< VertexDescriptorAttribute > m_attributes;
};

//----------------------------------------------------------------------------------------------
struct RenderConfig
{
    bool        m_isEnabled = true;
    std::string m_hdrTexture;
    std::string m_brightPass;
    std::string m_horizontalBlurPass;
    std::string m_verticalBlurPass;
    std::string m_toneMappingPass;
    std::string m_pbrLitStatic;
    std::string m_pbrLitSkinned;
    std::string m_shadowMap;
    std::string m_skybox;
    std::string m_equirectangularToCubemap;
    std::string m_irradianceConvolution;
    std::string m_prefilterEnvironment;
    std::string m_brdfIntegration;
};

//----------------------------------------------------------------------------------------------
class Renderer
{
public:
    Renderer( RenderConfig const& config );
    ~Renderer();

    void                Shutdown();
    void                Startup();

    void                BeginFrame();
    void                EndFrame();

    void                BeginCamera( Camera const& camera );
    void                BeginCamera( Camera const& camera, float width, float height );
    void                BeginCamera( Camera const& camera, Viewport const& viewport );
    void                EndCamera( Camera const& camera );

    void                SetRenderTarget( ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv = nullptr );
    void                SetMaterialConstants( float metallic = 0.f, float roughness = 0.5f, float ambientOcclusion = 1.0f, float emissiveIntensity = 0.f, Rgba8 emissiveColor = Rgba8::WHITE ) const;
    void                SetPostProcessConstants() const;
    void                SetSkinConstant( std::vector< Mat44 > const& skinMatrices ) const;

    void                ClearScreen( Rgba8 const& clearColor );
    void                ClearRenderTarget( ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv = nullptr, Rgba8 const& clearColor = Rgba8::BLACK );

    BitmapFont*         CreateOrGetBitmapFont( char const* fontFilePathNameWithNoExtension, std::string const& textureName );

    void                CreateDeviceAndSwapChain();
    void                CreateDefaultRenderTargetView();

    void                BindShader( Shader* shader );
    void                BindShader( ShaderType type );

    void                BindDefaultShader();

    void                BindTexture( Texture const* texture );
    void                BindTexture( TextureBase const* texture, ResourceSlot slot );
    void                UnbindTexture( ResourceSlot slot );
    void                UnbindTextures();

    void                SetBlendMode( BlendMode blendMode );
    void                SetSamplerMode( SamplerMode samplerMode, ResourceSlot slot );
    void                SetRasterizerMode( RasterizerMode rasterizerMode );
    void                SetDepthMode( DepthMode depthMode );
    void                ResetSamplerModes();

    Shader*             CreateShaderAndComplie( char const* shaderName, char const* shaderSource );
    Shader*             CreateOrGetShader( char const* shaderName, VertexType vertexType = VertexType::VERTEX_PCUTBN );
    Shader*             CreateShaderAndComplie( char const* shaderName, VertexType vertexType = VertexType::VERTEX_PCUTBN );

    RenderTexture*      CreateRenderTexture();
    RenderTexture*      CreateRenderTexture( unsigned int width, unsigned int height, DXGI_FORMAT format );

    DepthRenderTexture* CreateDepthTexture();

    Texture const*      GetTexture( DefaultTexture textureType );
    Texture*            CreateTextureFromImage( Image const& image );
    Texture*            CreateOrGetHDRTextureFromFile( char const* imageFilePath );
    Texture*            CreateOrGetTextureFromFile( char const* imageFilePath );
    Texture*            GetLoadedTextureByName( std::string const& textureName ) const;
    Texture*            GetTextureFromFileName( char const* imageFilePath );

    Image               CreateImageFromFile( char const* imageFilePath );

    void                CopyCPUToGPU( void const* data, unsigned int size, VertexBuffer* vbo );
    void                CopyCPUToGPU( void const* data, unsigned int size, IndexBuffer* ibo );
    void                CopyCPUToGPU( void const* data, unsigned int size, ConstantBuffer* cbo );

    void                DrawVertexArray( int numVertexes, Vertex const* vertexes );
    void                DrawVertexArray( std::vector< Vertex > const& verts );
    void                DrawIndexedVertexArray( std::vector< Vertex > const& vertexes, std::vector< unsigned int > const& indexes );

    void                DrawVertexBuffer( VertexBuffer* vbo, unsigned int vertexCount );
    void                DrawIndexedVertexBuffer( VertexBuffer* vbo, IndexBuffer* ibo, unsigned int indexCount );

    void                SetModelConstants( Mat44 const& modelToWorldTransform = Mat44(), Rgba8 const& modelColor = Rgba8::WHITE );
    void                BindConstantBuffer( int slot, ConstantBuffer* cbo );

    VertexBuffer*       CreateVertexBuffer( unsigned int size, unsigned int stride );
    IndexBuffer*        CreateIndexBuffer( unsigned int size, unsigned int stride );
    ConstantBuffer*     CreateConstantBuffer( unsigned int size );

    void                DrawFullQuad();
    void                DrawSkyCube( Camera* camera, float scale = 50.f );
    void                ConvertEquirectangularToCubemap( Texture* texture );
    void                GenerateIrradianceCubemap();

    void                BeginShadowPass();
    void                EndShadowPass();

    void                BeginHDRPass();
    void                EndHDRPass();

    void                BeginBrightPass();
    void                EndBrightPass();

    void                BeginHorizontalBlurPass();
    void                EndHorizontalBlurPass();

    void                BeginVerticalBlurPass();
    void                EndVerticalBlurPass();

    void                BeginToneMappingPass();
    void                EndToneMappingPass();

    void                GeneratePrefilteredCubemap();
    void                GenerateBRDFLUT();

private:
    BitmapFont*      CreateBitmapFont( char const* fontFilePathNameWithNoExtension, Texture& fontTexture );
    void             CreateDefaultBuffers();
    void             CreateDefaultTextures();

    CubemapTexture*  CreateCubemapTexture( int width, int height, int mipLevels, unsigned int miscFlags );

    bool             CompileShaderToByteCode( std::vector< unsigned char >& outByteCode, char const* name, char const* source, char const* entryPoint, char const* target );

    void             CreateDefaultShaders();

    void             BindVertexBuffer( VertexBuffer* vbo );
    void             BindIndexBuffer( IndexBuffer* ibo );

    void             SetStatesIfChanged();

    void             DestroyTextures();
    void             DestroyShaders();

    VertexDescriptor GetVertexDescriptor( VertexType vertexType = VertexType::VERTEX_PCUTBN );
    char const*      GetSemanticName( VertexAttributeType type );

    void             InitializeBlendModes();
    void             InitializeBlendStates();
    void             CreateBlendState( BlendMode blendMode );
    void             ReleaseBlendResources();

    void             InitSamplerModes();
    void             InitializeSamplerStates();
    void             CreateSamplerState( SamplerMode samplerMode );
    void             ReleaseSamplerResources();

    void             InitRasterizerModes();
    void             InitializeRasterizerStates();
    void             CreateRasterizerState( RasterizerMode rasterizerMode );
    void             ReleaseRasterizerResources();

    void             InitDepthStencilModes();
    void             InitializeDepthStencilStates();
    void             CreateDepthStencilState( DepthMode depthMode );
    void             CreateDepthStencilTexture();
    void             CreateDepthStencilDSV();
    void             ReleaseDepthResources();

    void             InitDebugLayer();
    void             ShutdownDebugLayer();

    void             InitializeFullQuad();
    void             InitializeCube();

public:
    RenderConfig                                                                 m_config;

    std::vector< Texture* >                                                      m_loadedTextures;
    std::map< std::string, Texture* >                                            m_loadedTexturesByName;
    std::map< std::string, BitmapFont* >                                         m_loadedFontsByName;

    Shader*                                                                      m_toneMapping              = nullptr;
    Shader*                                                                      m_brightPass               = nullptr;
    Shader*                                                                      m_horizontalBlur           = nullptr;
    Shader*                                                                      m_verticalBlur             = nullptr;
    Shader*                                                                      m_defaultShader            = nullptr;
    Shader*                                                                      m_pbrLitStatic             = nullptr;
    Shader*                                                                      m_pbrLitSkinned            = nullptr;
    Shader*                                                                      m_shadowMap                = nullptr;
    Shader*                                                                      m_skybox                   = nullptr;
    Shader*                                                                      m_equirectangularToCubemap = nullptr;
    Shader*                                                                      m_irradianceConvolution    = nullptr;
    Shader*                                                                      m_prefilterEnvironment     = nullptr;
    Shader*                                                                      m_brdfIntegration          = nullptr;

    Texture const*                                                               m_defaultWhiteTexture            = nullptr;
    Texture const*                                                               m_defaultDiffuseTexture          = nullptr;
    Texture const*                                                               m_defaultNormalTexture           = nullptr;
    Texture const*                                                               m_defaultSGETexture              = nullptr;
    Texture const*                                                               m_defaultMetallicTexture         = nullptr;
    Texture const*                                                               m_defaultRoughnessTexture        = nullptr;
    Texture const*                                                               m_defaultAmbientOcclusionTexture = nullptr;
    Texture*                                                                     m_hdrTexture                     = nullptr;

    ID3D11Device*                                                                m_device           = nullptr;
    ID3D11DeviceContext*                                                         m_deviceContext    = nullptr;
    IDXGISwapChain*                                                              m_swapChain        = nullptr;
    ID3D11RenderTargetView*                                                      m_renderTargetView = nullptr;

    VertexBuffer*                                                                m_immediateVBO   = nullptr;
    IndexBuffer*                                                                 m_immediateIBO   = nullptr;
    ConstantBuffer*                                                              m_cameraCBO      = nullptr;
    ConstantBuffer*                                                              m_modelCBO       = nullptr;
    ConstantBuffer*                                                              m_matCBO         = nullptr;
    ConstantBuffer*                                                              m_postProcessCBO = nullptr;
    ConstantBuffer*                                                              m_prefilterCBO   = nullptr;
    ConstantBuffer*                                                              m_skinCBO        = nullptr;

    std::array< SamplerMode, static_cast< int >( ResourceSlot::COUNT ) >         m_desiredSamplerModes;
    std::array< ID3D11SamplerState*, static_cast< int >( ResourceSlot::COUNT ) > m_currentSamplerStates = {};

    ID3D11SamplerState*                                                          m_hdrSceneSampler                                           = nullptr;
    ID3D11SamplerState*                                                          m_samplerState                                              = nullptr;
    ID3D11SamplerState*                                                          m_samplerStates[ static_cast< int >( SamplerMode::COUNT ) ] = {};

    BlendMode                                                                    m_desiredBlendMode                                      = BlendMode::ALPHA;
    ID3D11BlendState*                                                            m_blendState                                            = nullptr;
    ID3D11BlendState*                                                            m_blendStates[ static_cast< int >( BlendMode::COUNT ) ] = {};

    RasterizerMode                                                               m_desiredRasterizerMode                                           = RasterizerMode::SOLID_CULL_BACK;
    ID3D11RasterizerState*                                                       m_rasterizerState                                                 = nullptr;
    ID3D11RasterizerState*                                                       m_rasterizerStates[ static_cast< int >( RasterizerMode::COUNT ) ] = {};

    DepthMode                                                                    m_desiredDepthMode                                             = DepthMode::READ_WRITE_LESS_EQUAL;
    ID3D11Texture2D*                                                             m_depthStencilTexture                                          = nullptr;
    ID3D11DepthStencilView*                                                      m_depthStencilDSV                                              = nullptr;
    ID3D11DepthStencilView*                                                      m_shadowDSV                                                    = nullptr;
    ID3D11DepthStencilState*                                                     m_depthStencilState                                            = nullptr;
    ID3D11DepthStencilState*                                                     m_depthStencilStates[ static_cast< int >( DepthMode::COUNT ) ] = {};

    RenderTexture*                                                               m_hdrRenderTexture      = nullptr;
    RenderTexture*                                                               m_brightPassTexture     = nullptr;
    RenderTexture*                                                               m_horizontalBlurTexture = nullptr;
    RenderTexture*                                                               m_verticalBlurTexture   = nullptr;
    RenderTexture*                                                               m_brdfLUTTexture        = nullptr;

    CubemapTexture*                                                              m_environmentTexture = nullptr;
    CubemapTexture*                                                              m_irradianceTexture  = nullptr;
    CubemapTexture*                                                              m_prefilteredTexture = nullptr;

    DepthRenderTexture*                                                          m_shadowMapTexture        = nullptr;
    ID3D11SamplerState*                                                          m_shadowComparisonSampler = nullptr;

private:
    VertexBuffer*               m_fullQuadVertexBuffer = nullptr;
    IndexBuffer*                m_fullQuadIndexBuffer  = nullptr;
    std::vector< Vertex >       m_fullQuadVertices;
    std::vector< unsigned int > m_fullQuadIndices;

private:
    VertexBuffer*               m_cubeVertexBuffer = nullptr;
    IndexBuffer*                m_cubeIndexBuffer  = nullptr;
    std::vector< Vertex >       m_cubeVertices;
    std::vector< unsigned int > m_cubeIndices;

protected:
    std::vector< Shader* > m_loadedShaders;

private:
#if defined( ENGINE_DEBUG_RENDER )
    void* m_dxgiDebug       = nullptr;
    void* m_dxgiDebugModule = nullptr;
#endif
};