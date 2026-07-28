#include "Engine/Renderer/Renderer.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "d3dcompiler.lib" )

#if defined( ENGINE_DEBUG_RENDER )
#include <dxgidebug.h>
#pragma comment( lib, "dxguid.lib" )
#endif

#if defined( OPAQUE )
#undef OPAQUE
#endif

#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "RendererTexture.hpp"
#include "DepthRenderTexture.hpp"
#include "CubemapTexture.hpp"

//----------------------------------------------------------------------------------------------
struct TextureDescriptionConfig
{
    unsigned int width          = 0;
    unsigned int height         = 0;
    unsigned int mipLevels      = 1;
    unsigned int arraySize      = 1;
    DXGI_FORMAT  format         = DXGI_FORMAT_R8G8B8A8_UNORM;
    unsigned int sampleCount    = 1;
    unsigned int sampleQuality  = 0;
    D3D11_USAGE  usage          = D3D11_USAGE_DEFAULT;
    unsigned int bindFlags      = 0;
    unsigned int cpuAccessFlags = 0;
    unsigned int miscFlags      = 0;
};

//------------------------------------------------------------------------------------------------
D3D11_TEXTURE2D_DESC CreateTextureDescription( TextureDescriptionConfig const& config )
{
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width                = config.width;
    desc.Height               = config.height;
    desc.MipLevels            = config.mipLevels;
    desc.ArraySize            = config.arraySize;
    desc.Format               = config.format;
    desc.SampleDesc.Count     = config.sampleCount;
    desc.SampleDesc.Quality   = config.sampleQuality;
    desc.Usage                = config.usage;
    desc.BindFlags            = config.bindFlags;
    desc.CPUAccessFlags       = config.cpuAccessFlags;
    desc.MiscFlags            = config.miscFlags;

    return desc;
}

//----------------------------------------------------------------------------------------------
Renderer::Renderer( RenderConfig const& config )
    : m_config( config )
{
}

//----------------------------------------------------------------------------------------------
Renderer::~Renderer()
{
}

//----------------------------------------------------------------------------------------------
void Renderer::Startup()
{
    m_desiredSamplerModes.fill( SamplerMode::POINT_CLAMP );

    InitDebugLayer();

    CreateDeviceAndSwapChain();
    CreateDefaultRenderTargetView();
    CreateDefaultBuffers();
    CreateDefaultShaders();
    CreateDefaultTextures();

    InitializeBlendModes();
    InitRasterizerModes();
    InitSamplerModes();
    InitDepthStencilModes();

    m_hdrTexture = CreateOrGetHDRTextureFromFile( m_config.m_hdrTexture.c_str() );
    ConvertEquirectangularToCubemap( m_hdrTexture );

    GenerateIrradianceCubemap();
    GeneratePrefilteredCubemap();
    GenerateBRDFLUT();

    BindDefaultShader();
    BindTexture( nullptr );
}

//----------------------------------------------------------------------------------------------
void Renderer::Shutdown()
{
    DestroyShaders();
    DestroyTextures();

    delete m_immediateVBO;
    m_immediateVBO = nullptr;

    delete m_immediateIBO;
    m_immediateIBO = nullptr;

    delete m_cameraCBO;
    m_cameraCBO = nullptr;

    delete m_modelCBO;
    m_modelCBO = nullptr;

    delete m_matCBO;
    m_matCBO = nullptr;

    delete m_prefilterCBO;
    m_prefilterCBO = nullptr;

    delete m_postProcessCBO;
    m_postProcessCBO = nullptr;

    ReleaseBlendResources();
    ReleaseSamplerResources();
    ReleaseRasterizerResources();
    ReleaseDepthResources();

    m_defaultShader = nullptr;

    DX_SAFE_RELEASE( m_renderTargetView );
    DX_SAFE_RELEASE( m_swapChain );
    DX_SAFE_RELEASE( m_deviceContext );
    DX_SAFE_RELEASE( m_device );

    DX_SAFE_RELEASE( m_shadowComparisonSampler );

    delete m_fullQuadVertexBuffer;
    m_fullQuadVertexBuffer = nullptr;

    delete m_fullQuadIndexBuffer;
    m_fullQuadIndexBuffer = nullptr;

    delete m_cubeVertexBuffer;
    m_cubeVertexBuffer = nullptr;

    delete m_cubeIndexBuffer;
    m_cubeIndexBuffer = nullptr;

    delete m_hdrRenderTexture;
    m_hdrRenderTexture = nullptr;

    delete m_brightPassTexture;
    m_brightPassTexture = nullptr;

    delete m_horizontalBlurTexture;
    m_horizontalBlurTexture = nullptr;

    delete m_verticalBlurTexture;
    m_verticalBlurTexture = nullptr;

    delete m_shadowMapTexture;
    m_shadowMapTexture = nullptr;

    delete m_environmentTexture;
    m_environmentTexture = nullptr;

    delete m_irradianceTexture;
    m_irradianceTexture = nullptr;

    delete m_prefilteredTexture;
    m_prefilteredTexture = nullptr;

    delete m_brdfLUTTexture;
    m_brdfLUTTexture = nullptr;

    ShutdownDebugLayer();
}

//----------------------------------------------------------------------------------------------
void Renderer::BeginFrame()
{
    m_deviceContext->OMSetRenderTargets( 1, &m_renderTargetView, m_depthStencilDSV );
}

//----------------------------------------------------------------------------------------------
void Renderer::EndFrame()
{
    // Present
    HRESULT hr;
    hr = m_swapChain->Present( 0, 0 );
    if ( hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET )
    {
        ERROR_AND_DIE( "Device has been lost, application will now terminate." );
    }
}

//----------------------------------------------------------------------------------------------
void Renderer::BeginCamera( Camera const& camera )
{
    // viewport
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX       = 0.f;
    viewport.TopLeftY       = 0.f;
    viewport.Width          = static_cast< float >( g_engine->m_window->GetClientDimensions().x );
    viewport.Height         = static_cast< float >( g_engine->m_window->GetClientDimensions().y );
    viewport.MinDepth       = 0.f;
    viewport.MaxDepth       = 1.f;

    m_deviceContext->RSSetViewports( 1, &viewport );

    CameraConstants cameraConstants  = CameraConstants();
    cameraConstants.c_renderToClip   = camera.GetRenderToClipTransform();
    cameraConstants.c_cameraToRender = camera.GetCameraToRenderTransform();
    cameraConstants.c_worldToCamera  = camera.GetWorldToCameraTransform();
    cameraConstants.c_cameraWorldPos = camera.GetPosition();

    CopyCPUToGPU( &cameraConstants, sizeof( CameraConstants ), m_cameraCBO );
    BindConstantBuffer( k_cameraConstantsSlot, m_cameraCBO );

    SetModelConstants();
}

//----------------------------------------------------------------------------------------------
void Renderer::BeginCamera( Camera const& camera, Viewport const& viewport )
{
    float          width  = static_cast< float >( g_engine->m_window->GetClientDimensions().x );
    float          height = static_cast< float >( g_engine->m_window->GetClientDimensions().y );

    // viewport
    D3D11_VIEWPORT dxViewport = {};
    dxViewport.TopLeftX       = viewport.x * width;
    dxViewport.TopLeftY       = viewport.y * height;
    dxViewport.Width          = viewport.width * width;
    dxViewport.Height         = viewport.height * height;
    dxViewport.MinDepth       = 0.f;
    dxViewport.MaxDepth       = 1.f;

    m_deviceContext->RSSetViewports( 1, &dxViewport );

    // cameraConstants
    CameraConstants cameraConstants  = CameraConstants();
    cameraConstants.c_renderToClip   = camera.GetRenderToClipTransform();
    cameraConstants.c_cameraToRender = camera.GetCameraToRenderTransform();
    cameraConstants.c_worldToCamera  = camera.GetWorldToCameraTransform();

    CopyCPUToGPU( &cameraConstants, sizeof( CameraConstants ), m_cameraCBO );
    BindConstantBuffer( k_cameraConstantsSlot, m_cameraCBO );

    SetModelConstants();
}

//----------------------------------------------------------------------------------------------
void Renderer::BeginCamera( Camera const& camera, float width, float height )
{
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX       = 0.f;
    viewport.TopLeftY       = 0.f;
    viewport.Width          = width;
    viewport.Height         = height;
    viewport.MinDepth       = 0.f;
    viewport.MaxDepth       = 1.f;

    m_deviceContext->RSSetViewports( 1, &viewport );

    CameraConstants cameraConstants  = CameraConstants();
    cameraConstants.c_renderToClip   = camera.GetRenderToClipTransform();
    cameraConstants.c_cameraToRender = camera.GetCameraToRenderTransform();
    cameraConstants.c_worldToCamera  = camera.GetWorldToCameraTransform();
    cameraConstants.c_cameraWorldPos = camera.GetPosition();

    CopyCPUToGPU( &cameraConstants, sizeof( CameraConstants ), m_cameraCBO );
    BindConstantBuffer( k_cameraConstantsSlot, m_cameraCBO );

    SetModelConstants();
}

//----------------------------------------------------------------------------------------------
void Renderer::EndCamera( [[maybe_unused]] Camera const& camera )
{
}

//----------------------------------------------------------------------------------------------
void Renderer::SetRenderTarget( ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv )
{
    m_deviceContext->OMSetRenderTargets( 1, &rtv, dsv );
}

//----------------------------------------------------------------------------------------------
void Renderer::SetMaterialConstants( float metallic /*= 0.f*/, float roughness /*= 0.5f*/, float ambientOcclusion /*= 1.0f*/, float emissiveIntensity /*= 0.f*/, Rgba8 emissiveColor /*= Rgba8::WHITE */ ) const
{
    MaterialConstants materialConstants;

    materialConstants.c_metallic           = metallic;
    materialConstants.c_roughness          = roughness;
    materialConstants.c_ambientOcclusion   = ambientOcclusion;
    materialConstants.c_emissiveColor[ 0 ] = NormalizeByte( static_cast< unsigned char >( emissiveColor.r ) );
    materialConstants.c_emissiveColor[ 1 ] = NormalizeByte( static_cast< unsigned char >( emissiveColor.g ) );
    materialConstants.c_emissiveColor[ 2 ] = NormalizeByte( static_cast< unsigned char >( emissiveColor.b ) );

    materialConstants.c_emissiveIntensity = emissiveIntensity;

    g_engine->m_render->CopyCPUToGPU( &materialConstants, sizeof( MaterialConstants ), m_matCBO );
    g_engine->m_render->BindConstantBuffer( k_materialConstantsSlot, m_matCBO );
}

//----------------------------------------------------------------------------------------------
void Renderer::SetPostProcessConstants() const
{
    PostProcessConstants postProcessConstants;
    postProcessConstants.c_width  = static_cast< float >( g_engine->m_window->GetClientDimensions().x );
    postProcessConstants.c_height = static_cast< float >( g_engine->m_window->GetClientDimensions().y );
    g_engine->m_render->CopyCPUToGPU( &postProcessConstants, sizeof( PostProcessConstants ), m_postProcessCBO );
    g_engine->m_render->BindConstantBuffer( k_postProcessConstantsSlot, m_postProcessCBO );
}

//------------------------------------------------------------------------------------------------
void Renderer::BeginHDRPass()
{
    ClearRenderTarget( m_hdrRenderTexture->m_renderTargetView, m_depthStencilDSV, Rgba8( 0, 0, 0 ) );
    SetRenderTarget( m_hdrRenderTexture->m_renderTargetView, m_depthStencilDSV );

    SetSamplerMode( SamplerMode::BILINEAR_CLAMP, ResourceSlot::SHADOWMAP );
    BindTexture( m_shadowMapTexture, ResourceSlot::SHADOWMAP );

    SetSamplerMode( SamplerMode::BILINEAR_CLAMP, ResourceSlot::IRRADIANCE );
    BindTexture( m_irradianceTexture, ResourceSlot::IRRADIANCE );

    BindTexture( m_prefilteredTexture, ResourceSlot::PREFILTERED );
    SetSamplerMode( SamplerMode::BILINEAR_CLAMP, ResourceSlot::PREFILTERED );

    BindTexture( m_brdfLUTTexture, ResourceSlot::BRDF_LUT );
    SetSamplerMode( SamplerMode::BILINEAR_CLAMP, ResourceSlot::BRDF_LUT );
}

//------------------------------------------------------------------------------------------------
void Renderer::EndHDRPass()
{
    UnbindTextures();
    ResetSamplerModes();

    m_deviceContext->OMSetRenderTargets( 0, nullptr, nullptr );
}

//----------------------------------------------------------------------------------------------
void Renderer::BeginBrightPass()
{
    SetRenderTarget( m_brightPassTexture->m_renderTargetView );
    ClearRenderTarget( m_brightPassTexture->m_renderTargetView, nullptr );

    BindShader( m_brightPass );
    BindTexture( m_hdrRenderTexture, ResourceSlot::HDR_SCENE );
}

//----------------------------------------------------------------------------------------------
void Renderer::EndBrightPass()
{
    UnbindTexture( ResourceSlot::HDR_SCENE );
    m_deviceContext->OMSetRenderTargets( 0, nullptr, nullptr );
}

//----------------------------------------------------------------------------------------------
void Renderer::BeginHorizontalBlurPass()
{
    SetRenderTarget( m_horizontalBlurTexture->m_renderTargetView );
    ClearRenderTarget( m_horizontalBlurTexture->m_renderTargetView, nullptr );

    SetDepthMode( DepthMode::DISABLED );
    SetSamplerMode( SamplerMode::BILINEAR_CLAMP, ResourceSlot::BRIGHT_PASS );
    SetPostProcessConstants();

    BindShader( m_horizontalBlur );
    BindTexture( m_brightPassTexture, ResourceSlot::BRIGHT_PASS );
}

//----------------------------------------------------------------------------------------------
void Renderer::EndHorizontalBlurPass()
{
    UnbindTexture( ResourceSlot::BRIGHT_PASS );
    m_deviceContext->OMSetRenderTargets( 0, nullptr, nullptr );
}

//----------------------------------------------------------------------------------------------
void Renderer::BeginVerticalBlurPass()
{
    SetRenderTarget( m_verticalBlurTexture->m_renderTargetView );
    ClearRenderTarget( m_verticalBlurTexture->m_renderTargetView, nullptr );

    SetDepthMode( DepthMode::DISABLED );
    SetSamplerMode( SamplerMode::BILINEAR_CLAMP, ResourceSlot::HORIZONTAL_BLUR );
    SetPostProcessConstants();

    BindShader( m_verticalBlur );
    BindTexture( m_horizontalBlurTexture, ResourceSlot::HORIZONTAL_BLUR );
}

//----------------------------------------------------------------------------------------------
void Renderer::EndVerticalBlurPass()
{
    UnbindTexture( ResourceSlot::HORIZONTAL_BLUR );
    m_deviceContext->OMSetRenderTargets( 0, nullptr, nullptr );
}

//----------------------------------------------------------------------------------------------
void Renderer::BeginToneMappingPass()
{
    SetRenderTarget( m_renderTargetView, m_depthStencilDSV );
    ClearRenderTarget( m_renderTargetView );

    SetDepthMode( DepthMode::DISABLED );

    SetSamplerMode( SamplerMode::BILINEAR_CLAMP, ResourceSlot::HDR_SCENE );
    BindTexture( m_hdrRenderTexture, ResourceSlot::HDR_SCENE );

    SetSamplerMode( SamplerMode::BILINEAR_CLAMP, ResourceSlot::VERTICAL_BLUR );
    BindTexture( m_verticalBlurTexture, ResourceSlot::VERTICAL_BLUR );

    BindShader( m_toneMapping );
}

//----------------------------------------------------------------------------------------------
void Renderer::EndToneMappingPass()
{
    g_engine->m_render->UnbindTexture( ResourceSlot::HDR_SCENE );
    g_engine->m_render->UnbindTexture( ResourceSlot::VERTICAL_BLUR );
}

//----------------------------------------------------------------------------------------------
void Renderer::ClearScreen( Rgba8 const& clearColor )
{
    float colorAsFloats[ 4 ];
    clearColor.GetAsFloats( colorAsFloats );
    m_deviceContext->ClearRenderTargetView( m_renderTargetView, colorAsFloats );
    m_deviceContext->ClearDepthStencilView( m_depthStencilDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
}

//----------------------------------------------------------------------------------------------
void Renderer::ClearRenderTarget( ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv /*= nullptr*/, Rgba8 const& clearColor /*= Rgba8::BLACK */ )
{
    float colorAsFloats[ 4 ];
    clearColor.GetAsFloats( colorAsFloats );
    m_deviceContext->ClearRenderTargetView( rtv, colorAsFloats );
    if ( dsv )
    {
        m_deviceContext->ClearDepthStencilView( dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
    }
}

//-----------------------------------------------------------------------------------------------
Texture* Renderer::GetLoadedTextureByName( std::string const& textureName ) const
{
    std::map< std::string, Texture* >::const_iterator iterator = m_loadedTexturesByName.find( textureName );
    GUARANTEE_OR_DIE( iterator != m_loadedTexturesByName.end(), "Texture not found: " + textureName );
    return iterator->second;
}

//------------------------------------------------------------------------------------------------
Texture* Renderer::GetTextureFromFileName( char const* imageFilePath )
{
    for ( int textureIndex = 0; textureIndex < static_cast< int >( m_loadedTextures.size() ); ++textureIndex )
    {
        if ( m_loadedTextures[ textureIndex ] && m_loadedTextures[ textureIndex ]->m_name == imageFilePath )
        {
            return m_loadedTextures[ textureIndex ];
        }
    }
    return nullptr;
}

//----------------------------------------------------------------------------------------------
DepthRenderTexture* Renderer::CreateDepthTexture()
{
    DepthRenderTexture*      newDepthRenderTexture = new DepthRenderTexture();
    HRESULT                  hr;

    TextureDescriptionConfig config;
    config.width              = 2048;
    config.height             = 2048;
    config.format             = DXGI_FORMAT_R24G8_TYPELESS;
    config.bindFlags          = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    D3D11_TEXTURE2D_DESC desc = CreateTextureDescription( config );
    hr                        = m_device->CreateTexture2D( &desc, nullptr, &newDepthRenderTexture->m_texture );
    GUARANTEE_OR_DIE( SUCCEEDED( hr ), "Failed to create Texture" );

    // DSV
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format                        = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension                 = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice            = 0;

    // write texture
    hr = m_device->CreateDepthStencilView( newDepthRenderTexture->m_texture, &dsvDesc, &newDepthRenderTexture->m_depthStencilView );
    GUARANTEE_OR_DIE( SUCCEEDED( hr ), "Failed to create shadow map DSV." );

    // SRV
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format                          = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.ViewDimension                   = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip       = 0;
    srvDesc.Texture2D.MipLevels             = 1;

    // read texture
    hr = m_device->CreateShaderResourceView( newDepthRenderTexture->m_texture, &srvDesc, &newDepthRenderTexture->m_shaderResourceView );
    GUARANTEE_OR_DIE( SUCCEEDED( hr ), "Failed to create SRV" );

    return newDepthRenderTexture;
}

//----------------------------------------------------------------------------------------------
void Renderer::DrawVertexArray( int numVertexes, Vertex const* vertexes )
{
    CopyCPUToGPU( vertexes, numVertexes * sizeof( Vertex ), m_immediateVBO );
    DrawVertexBuffer( m_immediateVBO, numVertexes );
}

//------------------------------------------------------------------------------------------------
void Renderer::DrawVertexArray( std::vector< Vertex > const& verts )
{
    CopyCPUToGPU( verts.data(), static_cast< unsigned int >( verts.size() ) * sizeof( Vertex ), m_immediateVBO );
    DrawVertexBuffer( m_immediateVBO, static_cast< unsigned int >( verts.size() ) );
}

//------------------------------------------------------------------------------------------------
void Renderer::DrawIndexedVertexArray( std::vector< Vertex > const& vertexes, std::vector< unsigned int > const& indexes )
{
    CopyCPUToGPU( vertexes.data(), static_cast< unsigned int >( vertexes.size() ) * sizeof( Vertex ), m_immediateVBO );
    CopyCPUToGPU( indexes.data(), static_cast< unsigned int >( indexes.size() ) * sizeof( unsigned int ), m_immediateIBO );
    DrawIndexedVertexBuffer( m_immediateVBO, m_immediateIBO, static_cast< unsigned int >( indexes.size() ) );
}

//------------------------------------------------------------------------------------------------
void Renderer::DrawVertexBuffer( VertexBuffer* vbo, unsigned int vertexCount )
{
    BindVertexBuffer( vbo );
    SetStatesIfChanged();
    m_deviceContext->Draw( vertexCount, 0 );
}

//------------------------------------------------------------------------------------------------
void Renderer::DrawIndexedVertexBuffer( VertexBuffer* vbo, IndexBuffer* ibo, unsigned int indexCount )
{
    BindVertexBuffer( vbo );
    BindIndexBuffer( ibo );
    SetStatesIfChanged();
    m_deviceContext->DrawIndexed( indexCount, 0, 0 );
}

//-----------------------------------------------------------------------------------------------
void Renderer::SetBlendMode( BlendMode blendMode )
{
    m_desiredBlendMode = blendMode;
}

//------------------------------------------------------------------------------------------------
void Renderer::SetSamplerMode( SamplerMode samplerMode, ResourceSlot slot )
{
    m_desiredSamplerModes[ static_cast< int >( slot ) ] = samplerMode;
}

//------------------------------------------------------------------------------------------------
void Renderer::SetRasterizerMode( RasterizerMode rasterizerMode )
{
    m_desiredRasterizerMode = rasterizerMode;
}

//------------------------------------------------------------------------------------------------
void Renderer::SetModelConstants( Mat44 const& modelToWorldTransform /*= Mat44()*/, Rgba8 const& modelColor /*= Rgba8::WHITE */ )
{
    ModelConstants modelConstants = ModelConstants();
    modelConstants.u_modelToWorld = modelToWorldTransform;

    modelConstants.u_modelTint[ 0 ] = NormalizeByte( modelColor.r );
    modelConstants.u_modelTint[ 1 ] = NormalizeByte( modelColor.g );
    modelConstants.u_modelTint[ 2 ] = NormalizeByte( modelColor.b );
    modelConstants.u_modelTint[ 3 ] = NormalizeByte( modelColor.a );

    CopyCPUToGPU( &modelConstants, sizeof( ModelConstants ), m_modelCBO );
    BindConstantBuffer( k_modelConstantsSlot, m_modelCBO );
}

//------------------------------------------------------------------------------------------------
void Renderer::SetStatesIfChanged()
{
    // Blend State
    ID3D11BlendState* desiredBlendState = m_blendStates[ static_cast< int >( m_desiredBlendMode ) ];
    if ( m_blendState != desiredBlendState )
    {
        m_blendState = desiredBlendState;

        float blendFactor[ 4 ] = { 0.0f, 0.0f, 0.0f, 0.0f };
        UINT  sampleMask       = 0xffffffff;
        m_deviceContext->OMSetBlendState( m_blendState, blendFactor, sampleMask );
    }

    // Sampler State
    for ( int slot = 0; slot < static_cast< int >( ResourceSlot::COUNT ); ++slot )
    {
        ID3D11SamplerState* desiredSamplerState = ( slot == static_cast< int >( ResourceSlot::SHADOWMAP ) ) ? m_shadowComparisonSampler : m_samplerStates[ static_cast< int >( m_desiredSamplerModes[ slot ] ) ];
        if ( m_currentSamplerStates[ slot ] != desiredSamplerState )
        {
            m_currentSamplerStates[ slot ] = desiredSamplerState;
            m_deviceContext->PSSetSamplers( slot, 1, &desiredSamplerState );
        }
    }

    // Rasterizer
    ID3D11RasterizerState* desiredRasterizerState = m_rasterizerStates[ static_cast< int >( m_desiredRasterizerMode ) ];
    if ( m_rasterizerState != desiredRasterizerState )
    {
        m_rasterizerState = desiredRasterizerState;
        m_deviceContext->RSSetState( m_rasterizerState );
    }

    // DepthStencilState
    ID3D11DepthStencilState* desiredDepthStencilState = m_depthStencilStates[ static_cast< int >( m_desiredDepthMode ) ];
    if ( m_depthStencilState != desiredDepthStencilState )
    {
        m_depthStencilState = desiredDepthStencilState;
        m_deviceContext->OMSetDepthStencilState( m_depthStencilState, 0 );
    }
}

//------------------------------------------------------------------------------------------------
BitmapFont* Renderer::CreateOrGetBitmapFont( char const* fontFilePathNameWithNoExtension, std::string const& textureName )
{
    std::string fontFilePathNameWithExtension = std::string( fontFilePathNameWithNoExtension ) + ".png";
    Texture*    fontTexture                   = GetTextureFromFileName( fontFilePathNameWithExtension.c_str() );
    GUARANTEE_OR_DIE( fontTexture, "Fail to load font texture !" );

    BitmapFont* bitmapFont             = CreateBitmapFont( fontFilePathNameWithNoExtension, *fontTexture );
    m_loadedFontsByName[ textureName ] = bitmapFont;

    return bitmapFont;
}

//------------------------------------------------------------------------------------------------
Image Renderer::CreateImageFromFile( char const* imageFilePath )
{
    GUARANTEE_OR_DIE( imageFilePath, Stringf( "Renderer::CreateImageFromFile - imageFilePath is empty" ) );
    Image image = Image( imageFilePath );
    return image;
}

//------------------------------------------------------------------------------------------------
Texture* Renderer::CreateTextureFromImage( Image const& image )
{
    Texture* newTexture      = new Texture();
    newTexture->m_name       = image.m_imageFilePath;
    newTexture->m_dimensions = image.GetDimensions();

    TextureDescriptionConfig config;
    config.width              = image.GetDimensions().x;
    config.height             = image.GetDimensions().y;
    config.mipLevels          = 0;
    config.miscFlags          = D3D11_RESOURCE_MISC_GENERATE_MIPS;
    config.format             = image.m_isHDR ? DXGI_FORMAT_R32G32B32A32_FLOAT : DXGI_FORMAT_R8G8B8A8_UNORM;
    config.bindFlags          = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    D3D11_TEXTURE2D_DESC desc = CreateTextureDescription( config );

    HRESULT              hr = m_device->CreateTexture2D( &desc, nullptr, &newTexture->m_texture );
    GUARANTEE_OR_DIE( SUCCEEDED( hr ), "Renderer::CreateTextureFromImage - CreateTexture2D failed." );

    unsigned int bytesPerRow = ( image.m_isHDR ? sizeof( Vec4 ) : sizeof( Rgba8 ) ) * image.GetDimensions().x;
    /*
        pDstResource   - target GPU texture
        DstSubresource - mip level 0
        pDstBox        - whole subresource
        pSrcData       - source CPU pixel data
        SrcRowPitch    - bytes per source row
        SrcDepthPitch  - source depth pitch (3D Texture Only)

        Uploads image pixel data from CPU memory to a GPU texture
    */
    m_deviceContext->UpdateSubresource( newTexture->m_texture, 0, nullptr, image.GetRawData(), bytesPerRow, 0 );

    hr = m_device->CreateShaderResourceView( newTexture->m_texture, nullptr, &newTexture->m_shaderResourceView );
    GUARANTEE_OR_DIE( SUCCEEDED( hr ), "CreateShaderResourceView failed." );

    // GenerateMips generates mipmap levels from mip level 0
    m_deviceContext->GenerateMips( newTexture->m_shaderResourceView );

    m_loadedTextures.push_back( newTexture );
    return newTexture;
}

//------------------------------------------------------------------------------------------------
Texture* Renderer::CreateOrGetHDRTextureFromFile( char const* imageFilePath )
{
    Image    image           = Image( imageFilePath );
    Texture* newTexture      = new Texture();
    newTexture->m_name       = image.m_imageFilePath;
    newTexture->m_dimensions = image.GetDimensions();

    TextureDescriptionConfig config;
    config.width              = image.GetDimensions().x;
    config.height             = image.GetDimensions().y;
    config.mipLevels          = 0;
    config.miscFlags          = D3D11_RESOURCE_MISC_GENERATE_MIPS;
    config.format             = DXGI_FORMAT_R32G32B32A32_FLOAT;
    config.bindFlags          = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    D3D11_TEXTURE2D_DESC desc = CreateTextureDescription( config );

    HRESULT              hr = m_device->CreateTexture2D( &desc, nullptr, &newTexture->m_texture );
    GUARANTEE_OR_DIE( SUCCEEDED( hr ), "Renderer::CreateTextureFromImage - CreateTexture2D failed." );

    unsigned int bytesPerRow = ( image.m_isHDR ? sizeof( Vec4 ) : sizeof( Rgba8 ) ) * image.GetDimensions().x;
    m_deviceContext->UpdateSubresource( newTexture->m_texture, 0, nullptr, image.GetRawData(), bytesPerRow, 0 );

    hr = m_device->CreateShaderResourceView( newTexture->m_texture, nullptr, &newTexture->m_shaderResourceView );
    GUARANTEE_OR_DIE( SUCCEEDED( hr ), "CreateShaderResourceView failed." );

    m_deviceContext->GenerateMips( newTexture->m_shaderResourceView );

    m_loadedTextures.push_back( newTexture );
    return newTexture;
}

//------------------------------------------------------------------------------------------------
Texture* Renderer::CreateOrGetTextureFromFile( char const* imageFilePath )
{
    // See if we already have this texture previously loaded
    Texture* existingTexture = GetTextureFromFileName( imageFilePath );  // You need to write this
    if ( existingTexture )
    {
        return existingTexture;
    }

    // Never seen this texture before!  Let's load it.
    Texture* newTexture = CreateTextureFromImage( imageFilePath );
    return newTexture;
}

//------------------------------------------------------------------------------------------------
void Renderer::CreateDeviceAndSwapChain()
{
    unsigned int deviceFlags = 0;
#if defined( ENGINE_DEBUG_RENDER )
    deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferDesc.Width     = g_engine->m_window->GetClientDimensions().x;
    swapChainDesc.BufferDesc.Height    = g_engine->m_window->GetClientDimensions().y;
    swapChainDesc.BufferDesc.Format    = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.SampleDesc.Count     = 1;
    swapChainDesc.BufferUsage          = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount          = 2;
    swapChainDesc.OutputWindow         = static_cast< HWND >( g_engine->m_window->GetHwnd() );
    swapChainDesc.Windowed             = true;
    swapChainDesc.SwapEffect           = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    HRESULT hr;
    hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        deviceFlags,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &m_swapChain,
        &m_device,
        nullptr,
        &m_deviceContext );

    if ( !SUCCEEDED( hr ) )
    {
        ERROR_AND_DIE( "Could not create D3D 11 device and swap chain" );
    }
}

//------------------------------------------------------------------------------------------------
void Renderer::CreateDefaultRenderTargetView()
{
    HRESULT          hr;
    ID3D11Texture2D* backBuffer;
    hr = m_swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**)&backBuffer );
    if ( !SUCCEEDED( hr ) )
    {
        ERROR_AND_DIE( "Could not get swap chain buffer." );
    }

    hr = m_device->CreateRenderTargetView( backBuffer, nullptr, &m_renderTargetView );
    if ( !SUCCEEDED( hr ) )
    {
        ERROR_AND_DIE( "Could not create render target view for swap chain buffer." );
    }

    DX_SAFE_RELEASE( backBuffer );
}

//------------------------------------------------------------------------------------------------
void Renderer::CreateDefaultBuffers()
{
    InitializeFullQuad();
    InitializeCube();

    m_immediateVBO = CreateVertexBuffer( sizeof( Vertex ), sizeof( Vertex ) );
    m_immediateIBO = CreateIndexBuffer( sizeof( unsigned int ), sizeof( unsigned int ) );

    m_cameraCBO      = CreateConstantBuffer( sizeof( CameraConstants ) );
    m_modelCBO       = CreateConstantBuffer( sizeof( ModelConstants ) );
    m_matCBO         = CreateConstantBuffer( sizeof( MaterialConstants ) );
    m_postProcessCBO = CreateConstantBuffer( sizeof( PostProcessConstants ) );
    m_prefilterCBO   = CreateConstantBuffer( sizeof( PrefilterConstants ) );
}

//------------------------------------------------------------------------------------------------
BitmapFont* Renderer::CreateBitmapFont( char const* fontFilePathNameWithNoExtension, Texture& fontTexture )
{
    BitmapFont* bitmapFont = new BitmapFont( fontFilePathNameWithNoExtension, fontTexture );
    return bitmapFont;
}

//------------------------------------------------------------------------------------------------
char const* Renderer::GetSemanticName( VertexAttributeType type )
{
    switch ( type )
    {
        case VertexAttributeType::POSITION:
            {
                return "VERTEX_POSITION";
            }

        case VertexAttributeType::COLOR:
            {
                return "VERTEX_COLOR";
            }

        case VertexAttributeType::UVTEXCOORDS:
            {
                return "VERTEX_UVTEXCOORDS";
            }

        case VertexAttributeType::TANGENT:
            {
                return "VERTEX_TANGENT";
            }

        case VertexAttributeType::BITANGENT:
            {
                return "VERTEX_BITANGENT";
            }

        case VertexAttributeType::NORMAL:
            {
                return "VERTEX_NORMAL";
            }

        case VertexAttributeType::JOINTINDICES:
            {
                return "VERTEX_JOINTINDICES";
            }

        case VertexAttributeType::JOINTWEIGHTS:
            {
                return "VERTEX_JOINTWEIGHTS";
            }
        default:
            ERROR_AND_DIE( Stringf( "Could not find semantic name for vertex attribute type" ) );
    }
}

//------------------------------------------------------------------------------------------------
Shader* Renderer::CreateShaderAndComplie( char const* shaderName, char const* shaderSource )
{
    HRESULT      hr;
    ShaderConfig shaderConfig;
    shaderConfig.m_name                 = shaderName;
    Shader*                      shader = new Shader( shaderConfig );

    // Create Vertex Shader
    std::vector< unsigned char > vertexShaderByteCode;
    CompileShaderToByteCode( vertexShaderByteCode, "VertexShader", shaderSource, shaderConfig.m_vertexEntryPoint.c_str(), "vs_5_0" );
    hr = m_device->CreateVertexShader(
        vertexShaderByteCode.data(),
        vertexShaderByteCode.size(),
        nullptr,
        &shader->m_vertexShader );

    if ( !SUCCEEDED( hr ) )
    {
        ERROR_AND_DIE( Stringf( "Could not create vertex shader." ) );
    }

    // Create Pixel Shader
    std::vector< unsigned char > pixelShaderByteCode;
    CompileShaderToByteCode( pixelShaderByteCode, "PixelShader", shaderSource, shaderConfig.m_pixelEntryPoint.c_str(), "ps_5_0" );
    hr = m_device->CreatePixelShader(
        pixelShaderByteCode.data(),
        pixelShaderByteCode.size(),
        nullptr,
        &shader->m_pixelShader );

    if ( !SUCCEEDED( hr ) )
    {
        ERROR_AND_DIE( Stringf( "Could not create pixel shader." ) );
    }

    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
        { "VERTEX_POSITION",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "VERTEX_COLOR",       0, DXGI_FORMAT_R8G8B8A8_UNORM,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "VERTEX_UVTEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "VERTEX_TANGENT",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "VERTEX_BITANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "VERTEX_NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    UINT numElements = ARRAYSIZE( inputElementDesc );
    hr               = m_device->CreateInputLayout( inputElementDesc, numElements, vertexShaderByteCode.data(), vertexShaderByteCode.size(), &shader->m_inputLayout );
    if ( !SUCCEEDED( hr ) )
    {
        ERROR_AND_DIE( "Could not create vertex layout" );
    }

    m_loadedShaders.push_back( shader );
    return shader;
}

//------------------------------------------------------------------------------------------------
Shader* Renderer::CreateShaderAndComplie( char const* shaderName, VertexType vertexType )
{
    std::string shaderFileName = std::string( shaderName ) + ".hlsl";
    std::string shaderSource;

    int         size = FileReadToString( shaderSource, shaderFileName );
    GUARANTEE_OR_DIE( size != 0, Stringf( "Shader is empty!" ) );

    HRESULT      hr;
    ShaderConfig shaderConfig;
    shaderConfig.m_name                 = shaderName;
    Shader*                      shader = new Shader( shaderConfig );

    // Create Vertex Shader
    std::vector< unsigned char > vertexShaderByteCode;
    CompileShaderToByteCode( vertexShaderByteCode, "VertexShader", shaderSource.c_str(), shaderConfig.m_vertexEntryPoint.c_str(), "vs_5_0" );
    hr = m_device->CreateVertexShader(
        vertexShaderByteCode.data(),
        vertexShaderByteCode.size(),
        nullptr,
        &shader->m_vertexShader );

    if ( !SUCCEEDED( hr ) )
    {
        ERROR_AND_DIE( Stringf( "Could not create vertex shader." ) );
    }

    // Create Pixel Shader
    std::vector< unsigned char > pixelShaderByteCode;
    CompileShaderToByteCode( pixelShaderByteCode, "PixelShader", shaderSource.c_str(), shaderConfig.m_pixelEntryPoint.c_str(), "ps_5_0" );
    hr = m_device->CreatePixelShader(
        pixelShaderByteCode.data(),
        pixelShaderByteCode.size(),
        nullptr,
        &shader->m_pixelShader );

    if ( !SUCCEEDED( hr ) )
    {
        ERROR_AND_DIE( Stringf( "Could not create pixel shader." ) );
    }

    std::vector< D3D11_INPUT_ELEMENT_DESC > inputElementDescs;
    VertexDescriptor                        vertexDescriptor = GetVertexDescriptor( vertexType );
    for ( int attributeIndex = 0; attributeIndex < static_cast< int >( vertexDescriptor.m_attributes.size() ); ++attributeIndex )
    {
        VertexDescriptorAttribute const& attribute    = vertexDescriptor.m_attributes[ attributeIndex ];
        char const*                      semanticName = GetSemanticName( attribute.m_semanticName );
        inputElementDescs.push_back( { semanticName, 0, attribute.m_format, 0, attribute.m_alignedByteOffset, D3D11_INPUT_PER_VERTEX_DATA, 0 } );
    }

    UINT numElements = static_cast< UINT >( inputElementDescs.size() );
    hr               = m_device->CreateInputLayout(
        inputElementDescs.data(),
        numElements,
        vertexShaderByteCode.data(),
        vertexShaderByteCode.size(),
        &shader->m_inputLayout );

    if ( !SUCCEEDED( hr ) )
    {
        ERROR_AND_DIE( "Could not create vertex layout" );
    }

    m_loadedShaders.push_back( shader );
    return shader;
}

//------------------------------------------------------------------------------------------------
Shader* Renderer::CreateOrGetShader( char const* shaderName, VertexType vertexType )
{
    for ( int shaderIndex = 0; shaderIndex < static_cast< int >( m_loadedShaders.size() ); ++shaderIndex )
    {
        Shader* shader = m_loadedShaders[ shaderIndex ];
        if ( shader->GetName() == shaderName )
        {
            return shader;
        }
    }

    Shader* shader = CreateShaderAndComplie( shaderName, vertexType );

    return shader;
}

//------------------------------------------------------------------------------------------------
void Renderer::CreateDefaultTextures()
{
    // clang-format off
	unsigned char defaultDiffuseTextureData [] = {
		255, 0, 255, 255,  0,   0, 0,    255, 255, 0,  255,  255,  0,   0, 0,   255,
		0,   0, 0,   255,  255, 0, 255,  255, 0,   0,  0,    255,  255, 0, 255, 255,
		255, 0, 255, 255,  0,   0, 0,    255, 255, 0,  255,  255,  0,   0, 0,   255,
		0,   0, 0,   255,  255, 0, 255,  255, 0,   0,  0,    255,  255, 0, 255, 255
	};
    // clang-format on

    m_defaultDiffuseTexture          = CreateTextureFromImage( Image( IntVec2( 4, 4 ), defaultDiffuseTextureData, 4 ) );
    m_defaultNormalTexture           = CreateTextureFromImage( Image( IntVec2( 4, 4 ), Rgba8( 127, 127, 255, 255 ) ) );
    m_defaultSGETexture              = CreateTextureFromImage( Image( IntVec2( 4, 4 ), Rgba8( 127, 127, 0, 255 ) ) );
    m_defaultWhiteTexture            = CreateTextureFromImage( Image( IntVec2( 2, 2 ), Rgba8::WHITE ) );
    m_defaultMetallicTexture         = m_defaultWhiteTexture;
    m_defaultRoughnessTexture        = m_defaultWhiteTexture;
    m_defaultAmbientOcclusionTexture = m_defaultWhiteTexture;

    m_hdrRenderTexture      = CreateRenderTexture();
    m_brightPassTexture     = CreateRenderTexture();
    m_horizontalBlurTexture = CreateRenderTexture();
    m_verticalBlurTexture   = CreateRenderTexture();
    m_shadowMapTexture      = CreateDepthTexture();
    m_environmentTexture    = CreateCubemapTexture( 512, 512, 0, D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS );
    m_irradianceTexture     = CreateCubemapTexture( 32, 32, 1, D3D11_RESOURCE_MISC_TEXTURECUBE );
    m_prefilteredTexture    = CreateCubemapTexture( 128, 128, 5, D3D11_RESOURCE_MISC_TEXTURECUBE );
    m_brdfLUTTexture        = CreateRenderTexture( 512, 512, DXGI_FORMAT_R16G16_FLOAT );
}

//------------------------------------------------------------------------------------------------
CubemapTexture* Renderer::CreateCubemapTexture( int width, int height, int mipLevels, unsigned int miscFlags )
{
    CubemapTexture*          newCubemapTexture = new CubemapTexture();
    HRESULT                  hr;

    TextureDescriptionConfig config;
    config.width     = width;
    config.height    = height;
    config.mipLevels = mipLevels;
    config.arraySize = 6;
    config.format    = DXGI_FORMAT_R16G16B16A16_FLOAT;
    config.bindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    config.miscFlags = miscFlags;

    D3D11_TEXTURE2D_DESC desc = CreateTextureDescription( config );
    hr                        = m_device->CreateTexture2D( &desc, nullptr, &newCubemapTexture->m_texture );
    GUARANTEE_OR_DIE( SUCCEEDED( hr ), "Failed to create cubemap texture." );

    D3D11_TEXTURE2D_DESC actualTextureDesc = {};
    newCubemapTexture->m_texture->GetDesc( &actualTextureDesc );

    newCubemapTexture->m_mipLevelCount = actualTextureDesc.MipLevels;
    newCubemapTexture->m_renderTargetViews.resize( actualTextureDesc.MipLevels * 6, nullptr );

    for ( unsigned int mipIndex = 0; mipIndex < actualTextureDesc.MipLevels; ++mipIndex )
    {
        for ( unsigned int faceIndex = 0; faceIndex < 6; ++faceIndex )
        {
            unsigned int                  rtvIndex = mipIndex * 6 + faceIndex;

            D3D11_RENDER_TARGET_VIEW_DESC rtvDesc  = {};
            rtvDesc.Format                         = actualTextureDesc.Format;
            rtvDesc.ViewDimension                  = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
            rtvDesc.Texture2DArray.MipSlice        = mipIndex;
            rtvDesc.Texture2DArray.FirstArraySlice = faceIndex;
            rtvDesc.Texture2DArray.ArraySize       = 1;

            hr = m_device->CreateRenderTargetView( newCubemapTexture->m_texture, &rtvDesc, &newCubemapTexture->m_renderTargetViews[ rtvIndex ] );
            GUARANTEE_OR_DIE( SUCCEEDED( hr ), "Failed to create cubemap face mip RTV." );
        }
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format                          = actualTextureDesc.Format;
    srvDesc.ViewDimension                   = D3D11_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.TextureCube.MostDetailedMip     = 0;
    srvDesc.TextureCube.MipLevels           = actualTextureDesc.MipLevels;

    hr = m_device->CreateShaderResourceView( newCubemapTexture->m_texture, &srvDesc, &newCubemapTexture->m_shaderResourceView );
    GUARANTEE_OR_DIE( SUCCEEDED( hr ), "Failed to create cubemap SRV." );

    return newCubemapTexture;
}

//------------------------------------------------------------------------------------------------
VertexDescriptor Renderer::GetVertexDescriptor( VertexType vertexType )
{
    VertexDescriptor vertexDescriptor;
    vertexDescriptor.m_attributes.emplace_back( VertexAttributeType::POSITION, 0, DXGI_FORMAT_R32G32B32_FLOAT );
    vertexDescriptor.m_attributes.emplace_back( VertexAttributeType::COLOR, D3D11_APPEND_ALIGNED_ELEMENT, DXGI_FORMAT_R8G8B8A8_UNORM );
    vertexDescriptor.m_attributes.emplace_back( VertexAttributeType::UVTEXCOORDS, D3D11_APPEND_ALIGNED_ELEMENT, DXGI_FORMAT_R32G32_FLOAT );
    vertexDescriptor.m_attributes.emplace_back( VertexAttributeType::TANGENT, D3D11_APPEND_ALIGNED_ELEMENT, DXGI_FORMAT_R32G32B32_FLOAT );
    vertexDescriptor.m_attributes.emplace_back( VertexAttributeType::BITANGENT, D3D11_APPEND_ALIGNED_ELEMENT, DXGI_FORMAT_R32G32B32_FLOAT );
    vertexDescriptor.m_attributes.emplace_back( VertexAttributeType::NORMAL, D3D11_APPEND_ALIGNED_ELEMENT, DXGI_FORMAT_R32G32B32_FLOAT );

    switch ( vertexType )
    {
        case VertexType::VERTEX_PCUTBN:
            {
                return vertexDescriptor;
            }

        case VertexType::VERTEX_SKINNED:
            {
                vertexDescriptor.m_attributes.emplace_back( VertexAttributeType::JOINTINDICES, D3D11_APPEND_ALIGNED_ELEMENT, DXGI_FORMAT_R32G32B32A32_SINT );
                vertexDescriptor.m_attributes.emplace_back( VertexAttributeType::JOINTWEIGHTS, D3D11_APPEND_ALIGNED_ELEMENT, DXGI_FORMAT_R32G32B32A32_FLOAT );

                return vertexDescriptor;
            }

        default:
            ERROR_AND_DIE( Stringf( "Could not find vertex type" ) );
    }
}

//------------------------------------------------------------------------------------------------
void Renderer::CreateDefaultShaders()
{
    m_defaultShader            = CreateShaderAndComplie( "Data/Shaders/Default", VertexType::VERTEX_PCUTBN );
    m_pbrLitStatic             = CreateShaderAndComplie( m_config.m_pbrLitStatic.c_str(), VertexType::VERTEX_PCUTBN );
    m_pbrLitSkinned            = CreateShaderAndComplie( m_config.m_pbrLitSkinned.c_str(), VertexType::VERTEX_SKINNED );
    m_brightPass               = CreateShaderAndComplie( m_config.m_brightPass.c_str(), VertexType::VERTEX_PCUTBN );
    m_horizontalBlur           = CreateShaderAndComplie( m_config.m_horizontalBlurPass.c_str(), VertexType::VERTEX_PCUTBN );
    m_verticalBlur             = CreateShaderAndComplie( m_config.m_verticalBlurPass.c_str(), VertexType::VERTEX_PCUTBN );
    m_toneMapping              = CreateShaderAndComplie( m_config.m_toneMappingPass.c_str(), VertexType::VERTEX_PCUTBN );
    m_shadowMap                = CreateShaderAndComplie( m_config.m_shadowMap.c_str(), VertexType::VERTEX_PCUTBN );
    m_skybox                   = CreateShaderAndComplie( m_config.m_skybox.c_str(), VertexType::VERTEX_PCUTBN );
    m_equirectangularToCubemap = CreateShaderAndComplie( m_config.m_equirectangularToCubemap.c_str(), VertexType::VERTEX_PCUTBN );
    m_irradianceConvolution    = CreateShaderAndComplie( m_config.m_irradianceConvolution.c_str(), VertexType::VERTEX_PCUTBN );
    m_prefilterEnvironment     = CreateShaderAndComplie( m_config.m_prefilterEnvironment.c_str(), VertexType::VERTEX_PCUTBN );
    m_brdfIntegration          = CreateShaderAndComplie( m_config.m_brdfIntegration.c_str(), VertexType::VERTEX_PCUTBN );

    BindShader( m_defaultShader );
}

//------------------------------------------------------------------------------------------------
VertexBuffer* Renderer::CreateVertexBuffer( unsigned int const size, unsigned int stride )
{
    GUARANTEE_OR_DIE( size > 0 && stride > 0, Stringf( "Renderer::CreateVertexBuffer - Size is Zero or	 Stride is Zero !" ) );
    return new VertexBuffer( size, stride );
}

//------------------------------------------------------------------------------------------------
IndexBuffer* Renderer::CreateIndexBuffer( unsigned int const size, unsigned int stride )
{
    GUARANTEE_OR_DIE( size > 0 && stride > 0, Stringf( "Renderer::CreateIndexBuffer - Size is Zero or Stride is Zero !" ) );
    return new IndexBuffer( size, stride );
}

//------------------------------------------------------------------------------------------------
ConstantBuffer* Renderer::CreateConstantBuffer( unsigned int const size )
{
    GUARANTEE_OR_DIE( size > 0, Stringf( "Renderer::CreateConstantBuffer - Size is Zero !" ) );
    return new ConstantBuffer( size );
}

//------------------------------------------------------------------------------------------------
void Renderer::DrawFullQuad()
{
    DrawIndexedVertexBuffer( m_fullQuadVertexBuffer, m_fullQuadIndexBuffer, static_cast< unsigned int >( m_fullQuadIndices.size() ) );
}

//------------------------------------------------------------------------------------------------
void Renderer::DrawSkyCube( Camera* camera, float scale )
{
    BindShader( m_skybox );
    BindTexture( m_environmentTexture, ResourceSlot::ENVIRONMENTCUBEMAP );
    SetSamplerMode( SamplerMode::BILINEAR_CLAMP, ResourceSlot::ENVIRONMENTCUBEMAP );
    SetDepthMode( DepthMode::READ_ONLY_LESS_EQUAL );
    SetRasterizerMode( RasterizerMode::SOLID_CULL_NONE );

    Mat44 skyboxTransform;
    skyboxTransform.AppendTranslation3D( camera->GetPosition() );
    skyboxTransform.AppendScaleUniform3D( scale );
    SetModelConstants( skyboxTransform );
    DrawVertexArray( m_cubeVertices );

    SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
    SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
}

//------------------------------------------------------------------------------------------------
void Renderer::CopyCPUToGPU( void const* data, unsigned int size, VertexBuffer* vbo )
{
    if ( size > vbo->m_size )
    {
        vbo->Resize( size );
    }

    D3D11_MAPPED_SUBRESOURCE resource;
    m_deviceContext->Map( vbo->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource );
    memcpy( resource.pData, data, size );
    m_deviceContext->Unmap( vbo->m_buffer, 0 );
}

//------------------------------------------------------------------------------------------------
void Renderer::CopyCPUToGPU( void const* data, unsigned int size, ConstantBuffer* cbo )
{
    D3D11_MAPPED_SUBRESOURCE resource;
    m_deviceContext->Map( cbo->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource );
    memcpy( resource.pData, data, size );
    m_deviceContext->Unmap( cbo->m_buffer, 0 );
}

//------------------------------------------------------------------------------------------------
void Renderer::CopyCPUToGPU( void const* data, unsigned int size, IndexBuffer* ibo )
{
    if ( size > ibo->m_size )
    {
        ibo->Resize( size );
    }

    D3D11_MAPPED_SUBRESOURCE resource;
    m_deviceContext->Map( ibo->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource );
    memcpy( resource.pData, data, size );
    m_deviceContext->Unmap( ibo->m_buffer, 0 );
}

//------------------------------------------------------------------------------------------------
bool Renderer::CompileShaderToByteCode( std::vector< unsigned char >& outByteCode, char const* name, char const* source, char const* entryPoint, char const* target )
{
    DWORD shaderFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#if defined( ENGINE_DEBUG_RENDER )
    shaderFlags = D3DCOMPILE_DEBUG;
    shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
    shaderFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif
    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob  = nullptr;
    HRESULT   hr;

    hr = D3DCompile(
        source,
        strlen( source ),
        name,
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entryPoint,
        target,
        shaderFlags,
        0,
        &shaderBlob,
        &errorBlob );

    if ( SUCCEEDED( hr ) )
    {
        outByteCode.resize( shaderBlob->GetBufferSize() );
        memcpy(
            outByteCode.data(),
            shaderBlob->GetBufferPointer(),
            shaderBlob->GetBufferSize() );
    }
    else
    {
        if ( errorBlob != nullptr )
        {
            DebuggerPrintf( static_cast< char* >( errorBlob->GetBufferPointer() ) );
        }
        ERROR_AND_DIE( Stringf( "Could not compile shader." ) );
    }

    shaderBlob->Release();

    if ( errorBlob != nullptr )
    {
        errorBlob->Release();
    }

    return true;
}

//------------------------------------------------------------------------------------------------
void Renderer::BindVertexBuffer( VertexBuffer* vbo )
{
    UINT startOffset = 0;
    m_deviceContext->IASetVertexBuffers( 0, 1, &vbo->m_buffer, &vbo->m_stride, &startOffset );
    m_deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}

//-------------------------------------------------------------------------	-----------------------
void Renderer::BindConstantBuffer( int slot, ConstantBuffer* cbo )
{
    m_deviceContext->VSSetConstantBuffers( slot, 1, &cbo->m_buffer );
    m_deviceContext->PSSetConstantBuffers( slot, 1, &cbo->m_buffer );
}

//------------------------------------------------------------------------------------------------
void Renderer::BindIndexBuffer( IndexBuffer* ibo )
{
    m_deviceContext->IASetIndexBuffer( ibo->m_buffer, DXGI_FORMAT_R32_UINT, 0 );
}

//------------------------------------------------------------------------------------------------
void Renderer::BindShader( Shader* shader )
{
    if ( !shader )
    {
        shader = m_defaultShader;
    }

    GUARANTEE_OR_DIE( shader, Stringf( "Renderer::BindShader() - Shader is null" ) );
    m_deviceContext->VSSetShader( shader->m_vertexShader, nullptr, 0 );
    m_deviceContext->PSSetShader( shader->m_pixelShader, nullptr, 0 );
    m_deviceContext->IASetInputLayout( shader->m_inputLayout );
}

//------------------------------------------------------------------------------------------------
void Renderer::BindShader( ShaderType type )
{
    Shader* shader = nullptr;
    switch ( type )
    {
        case ShaderType::PBRLitStatic:
            shader = m_pbrLitStatic;
            break;

        case ShaderType::PBRLitSkinned:
            shader = m_pbrLitSkinned;
            break;

        case ShaderType::ShadowMap:
            shader = m_shadowMap;
            break;

        default:
            ERROR_AND_DIE( "unknown shader" );
    }

    GUARANTEE_OR_DIE( shader, Stringf( "Renderer::BindShader - Shader is null" ) );
    m_deviceContext->VSSetShader( shader->m_vertexShader, nullptr, 0 );
    m_deviceContext->PSSetShader( shader->m_pixelShader, nullptr, 0 );
    m_deviceContext->IASetInputLayout( shader->m_inputLayout );
}

//---------------------------------------------------------------------------------------------
void Renderer::BindDefaultShader()
{
    BindShader( m_defaultShader );
}

//------------------------------------------------------------------------------------------------
void Renderer::BindTexture( Texture const* texture )
{
    if ( !texture )
    {
        texture = m_defaultWhiteTexture;
    }

    m_deviceContext->PSSetShaderResources( 0, 1, &texture->m_shaderResourceView );
}

//------------------------------------------------------------------------------------------------
void Renderer::BindTexture( TextureBase const* texture, ResourceSlot slot )
{
    m_deviceContext->PSSetShaderResources( static_cast< unsigned int >( slot ), 1, &texture->m_shaderResourceView );
}

//------------------------------------------------------------------------------------------------
void Renderer::UnbindTexture( ResourceSlot slot )
{
    ID3D11ShaderResourceView* nullShaderResourceView = nullptr;
    m_deviceContext->PSSetShaderResources( static_cast< unsigned int >( slot ), 1, &nullShaderResourceView );
}

//------------------------------------------------------------------------------------------------
void Renderer::UnbindTextures()
{
    g_engine->m_render->UnbindTexture( ResourceSlot::DIFFUSE );
    g_engine->m_render->UnbindTexture( ResourceSlot::METALLIC );
    g_engine->m_render->UnbindTexture( ResourceSlot::NORMAL );
    g_engine->m_render->UnbindTexture( ResourceSlot::ROUGHNESS );
    g_engine->m_render->UnbindTexture( ResourceSlot::AMBIENT_OCCLUSION );
}

//------------------------------------------------------------------------------------------------
Texture const* Renderer::GetTexture( DefaultTexture textureType )
{
    switch ( textureType )
    {
        case DefaultTexture::DIFFUSE:
            return m_defaultDiffuseTexture;

        case DefaultTexture::NORMAL:
            return m_defaultNormalTexture;

        case DefaultTexture::SPEC_GLOSS_EMIT:
            return m_defaultSGETexture;
    }

    return m_defaultWhiteTexture;
}

//------------------------------------------------------------------------------------------------
void Renderer::DestroyTextures()
{
    for ( int i = 0; i < static_cast< int >( m_loadedTextures.size() ); ++i )
    {
        delete m_loadedTextures[ i ];
    }

    for ( std::map< std::string, BitmapFont* >::iterator it = m_loadedFontsByName.begin(); it != m_loadedFontsByName.end(); ++it )
    {
        delete it->second;
    }

    m_loadedFontsByName.clear();

    m_loadedTextures.clear();
    m_loadedTexturesByName.clear();
    m_defaultWhiteTexture = nullptr;
}

//------------------------------------------------------------------------------------------------
void Renderer::DestroyShaders()
{
    for ( int shaderIndex = 0; shaderIndex < static_cast< int >( m_loadedShaders.size() ); ++shaderIndex )
    {
        if ( m_loadedShaders[ shaderIndex ] )
        {
            delete m_loadedShaders[ shaderIndex ];
        }
    }

    m_loadedShaders.clear();
}

//================================================================================================
// Blend
//================================================================================================
#pragma region BlendMode

void Renderer::InitializeBlendModes()
{
    InitializeBlendStates();
}

//------------------------------------------------------------------------------------------------
void Renderer::InitializeBlendStates()
{
    CreateBlendState( BlendMode::OPAQUE );
    CreateBlendState( BlendMode::ALPHA );
    CreateBlendState( BlendMode::ADDITIVE );
}

//------------------------------------------------------------------------------------------------
void Renderer::CreateBlendState( BlendMode blendMode )
{
    std::string      message;
    D3D11_BLEND_DESC blendDesc                        = {};
    blendDesc.RenderTarget[ 0 ].BlendEnable           = TRUE;
    blendDesc.RenderTarget[ 0 ].DestBlend             = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[ 0 ].SrcBlend              = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[ 0 ].BlendOp               = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[ 0 ].SrcBlendAlpha         = blendDesc.RenderTarget[ 0 ].SrcBlend;
    blendDesc.RenderTarget[ 0 ].DestBlendAlpha        = blendDesc.RenderTarget[ 0 ].DestBlend;
    blendDesc.RenderTarget[ 0 ].BlendOpAlpha          = blendDesc.RenderTarget[ 0 ].BlendOp;
    blendDesc.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    switch ( blendMode )
    {
        case BlendMode::OPAQUE:
            {
                message = "BlendMode::OPAQUE";
                break;
            }

        case BlendMode::ALPHA:
            {
                blendDesc.RenderTarget[ 0 ].SrcBlend  = D3D11_BLEND_SRC_ALPHA;
                blendDesc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
                message                               = "BlendMode::ALPHA";
                break;
            }

        case BlendMode::ADDITIVE:
            {
                blendDesc.RenderTarget[ 0 ].SrcBlend  = D3D11_BLEND_ONE;
                blendDesc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_ONE;
                message                               = "BlendMode::ADDITIVE";
                break;
            }

        default:
            ERROR_AND_DIE( "Unknown BlendMode" );
    }

    HRESULT hr = m_device->CreateBlendState( &blendDesc, &m_blendStates[ static_cast< int >( blendMode ) ] );
    if ( !SUCCEEDED( hr ) )
    {
        ERROR_AND_DIE( Stringf( "CreateBlendState for %s failed.", message.c_str() ) );
    }
}

//------------------------------------------------------------------------------------------------
void Renderer::ReleaseBlendResources()
{
    for ( int i = 0; i < static_cast< int >( BlendMode::COUNT ); ++i )
    {
        DX_SAFE_RELEASE( m_blendStates[ i ] );
    }
}

#pragma endregion BlendMode

//================================================================================================
// SamplerMode
//================================================================================================
#pragma region SamplerMode

void Renderer::InitSamplerModes()
{
    InitializeSamplerStates();
}

//------------------------------------------------------------------------------------------------
void Renderer::InitializeSamplerStates()
{
    CreateSamplerState( SamplerMode::POINT_CLAMP );
    CreateSamplerState( SamplerMode::BILINEAR_WRAP );
    CreateSamplerState( SamplerMode::BILINEAR_CLAMP );

    D3D11_SAMPLER_DESC desc = {};
    desc.Filter =
        D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;

    desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;

    desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;

    desc.BorderColor[ 0 ] = 1.f;
    desc.BorderColor[ 1 ] = 1.f;
    desc.BorderColor[ 2 ] = 1.f;
    desc.BorderColor[ 3 ] = 1.f;

    desc.MinLOD = 0.f;
    desc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT hr = m_device->CreateSamplerState(
        &desc,
        &m_shadowComparisonSampler );

    GUARANTEE_OR_DIE(
        SUCCEEDED( hr ),
        "Failed to create shadow comparison sampler" );
}

//------------------------------------------------------------------------------------------------
void Renderer::CreateSamplerState( SamplerMode samplerMode )
{
    std::string        message;
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.ComparisonFunc     = D3D11_COMPARISON_NEVER;
    samplerDesc.MaxLOD             = D3D11_FLOAT32_MAX;
    samplerDesc.MinLOD             = 0.0f;

    switch ( samplerMode )
    {
        case SamplerMode::POINT_CLAMP:
            {
                samplerDesc.Filter   = D3D11_FILTER_MIN_MAG_MIP_POINT;
                samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
                samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
                samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
                message              = "SamplerMode::POINT_CLAMP";
                break;
            }

        case SamplerMode::BILINEAR_WRAP:
            {
                samplerDesc.Filter   = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
                samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
                samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
                samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
                message              = "SamplerMode::BILINEAR_WRAP";
                break;
            }

        case SamplerMode::BILINEAR_CLAMP:
            {
                samplerDesc.Filter        = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
                samplerDesc.AddressU      = D3D11_TEXTURE_ADDRESS_CLAMP;
                samplerDesc.AddressV      = D3D11_TEXTURE_ADDRESS_CLAMP;
                samplerDesc.AddressW      = D3D11_TEXTURE_ADDRESS_CLAMP;
                samplerDesc.MipLODBias    = 0.0f;
                samplerDesc.MaxAnisotropy = 1;
                message                   = "SamplerMode::BILINEAR_CLAMP";
                break;
            }

        default:
            ERROR_AND_DIE( "Unknown SamplerMode" );
    }

    HRESULT hr = m_device->CreateSamplerState( &samplerDesc, &m_samplerStates[ static_cast< int >( samplerMode ) ] );
    if ( !SUCCEEDED( hr ) )
    {
        ERROR_AND_DIE( Stringf( "CreateSamplerState for %s failed.", message.c_str() ) );
    }
}

//------------------------------------------------------------------------------------------------
void Renderer::ReleaseSamplerResources()
{
    for ( int i = 0; i < static_cast< int >( SamplerMode::COUNT ); ++i )
    {
        DX_SAFE_RELEASE( m_samplerStates[ i ] );
    }
}

#pragma endregion SamplerMode

//================================================================================================
// RasterizerMode
//================================================================================================
#pragma region RasterizerMode

void Renderer::InitRasterizerModes()
{
    InitializeRasterizerStates();
}

//------------------------------------------------------------------------------------------------
void Renderer::InitializeRasterizerStates()
{
    CreateRasterizerState( RasterizerMode::SOLID_CULL_NONE );
    CreateRasterizerState( RasterizerMode::SOLID_CULL_BACK );
    CreateRasterizerState( RasterizerMode::WIREFRAME_CULL_NONE );
    CreateRasterizerState( RasterizerMode::WIREFRAME_CULL_BACK );
}

//------------------------------------------------------------------------------------------------
void Renderer::CreateRasterizerState( RasterizerMode rasterizerMode )
{
    HRESULT               hr;
    D3D11_RASTERIZER_DESC rasterizerDesc = {};

    rasterizerDesc.FrontCounterClockwise = true;
    rasterizerDesc.DepthClipEnable       = true;
    rasterizerDesc.AntialiasedLineEnable = true;
    rasterizerDesc.DepthBias             = 0;
    rasterizerDesc.DepthBiasClamp        = 0.0f;
    rasterizerDesc.SlopeScaledDepthBias  = 0.0f;
    rasterizerDesc.ScissorEnable         = false;
    rasterizerDesc.MultisampleEnable     = false;

    switch ( rasterizerMode )
    {
        case RasterizerMode::SOLID_CULL_NONE:
            {
                rasterizerDesc.FillMode = D3D11_FILL_SOLID;
                rasterizerDesc.CullMode = D3D11_CULL_NONE;
                hr                      = m_device->CreateRasterizerState( &rasterizerDesc, &m_rasterizerStates[ static_cast< int >( RasterizerMode::SOLID_CULL_NONE ) ] );
                if ( !SUCCEEDED( hr ) )
                {
                    ERROR_AND_DIE( "CreateRasterizerState for RasterizerMode::SOLID_CULL_NONE failed." );
                }
                break;
            }

        case RasterizerMode::SOLID_CULL_BACK:
            {
                rasterizerDesc.FillMode = D3D11_FILL_SOLID;
                rasterizerDesc.CullMode = D3D11_CULL_BACK;
                hr                      = m_device->CreateRasterizerState( &rasterizerDesc, &m_rasterizerStates[ static_cast< int >( RasterizerMode::SOLID_CULL_BACK ) ] );
                if ( !SUCCEEDED( hr ) )
                {
                    ERROR_AND_DIE( "CreateRasterizerState for RasterizerMode::SOLID_CULL_BACK failed." );
                }
                break;
            }

        case RasterizerMode::WIREFRAME_CULL_NONE:
            {
                rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
                rasterizerDesc.CullMode = D3D11_CULL_NONE;
                hr                      = m_device->CreateRasterizerState( &rasterizerDesc, &m_rasterizerStates[ static_cast< int >( RasterizerMode::WIREFRAME_CULL_NONE ) ] );
                if ( !SUCCEEDED( hr ) )
                {
                    ERROR_AND_DIE( "CreateRasterizerState for RasterizerMode::WIREFRAME_CULL_NONE failed." );
                }
                break;
            }

        case RasterizerMode::WIREFRAME_CULL_BACK:
            {
                rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
                rasterizerDesc.CullMode = D3D11_CULL_BACK;
                hr                      = m_device->CreateRasterizerState( &rasterizerDesc, &m_rasterizerStates[ static_cast< int >( RasterizerMode::WIREFRAME_CULL_BACK ) ] );
                if ( !SUCCEEDED( hr ) )
                {
                    ERROR_AND_DIE( "CreateRasterizerState for RasterizerMode::WIREFRAME_CULL_BACK failed." );
                }
                break;
            }
    }
}

//------------------------------------------------------------------------------------------------
void Renderer::ReleaseRasterizerResources()
{
    for ( int i = 0; i < static_cast< int >( RasterizerMode::COUNT ); ++i )
    {
        DX_SAFE_RELEASE( m_rasterizerStates[ i ] );
    }
}

#pragma endregion RasterizerMode

//================================================================================================
// DepthStencilMode
//================================================================================================
#pragma region DepthStencilMode

void Renderer::InitDepthStencilModes()
{
    CreateDepthStencilTexture();
    CreateDepthStencilDSV();
    InitializeDepthStencilStates();
}

//------------------------------------------------------------------------------------------------
void Renderer::CreateDepthStencilTexture()
{
    TextureDescriptionConfig config;
    config.width     = g_engine->m_window->GetClientDimensions().x;
    config.height    = g_engine->m_window->GetClientDimensions().y;
    config.mipLevels = 1;
    config.format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
    config.bindFlags = D3D11_BIND_DEPTH_STENCIL;

    D3D11_TEXTURE2D_DESC desc = CreateTextureDescription( config );
    HRESULT              hr   = m_device->CreateTexture2D( &desc, nullptr, &m_depthStencilTexture );
    if ( !SUCCEEDED( hr ) )
    {
        ERROR_AND_DIE( "Could not create texture for depth stencil." );
    }
}

//------------------------------------------------------------------------------------------------
void Renderer::CreateDepthStencilDSV()
{
    HRESULT hr;
    hr = m_device->CreateDepthStencilView( m_depthStencilTexture, nullptr, &m_depthStencilDSV );
    if ( !SUCCEEDED( hr ) )
    {
        ERROR_AND_DIE( "Could not create depth stencil view." );
    }
}

//------------------------------------------------------------------------------------------------
void Renderer::InitializeDepthStencilStates()
{
    CreateDepthStencilState( DepthMode::DISABLED );
    CreateDepthStencilState( DepthMode::READ_ONLY_ALWAYS );
    CreateDepthStencilState( DepthMode::READ_WRITE_LESS_EQUAL );
    CreateDepthStencilState( DepthMode::READ_ONLY_LESS_EQUAL );
}

//------------------------------------------------------------------------------------------------
void Renderer::CreateDepthStencilState( DepthMode depthMode )
{
    HRESULT                  hr;
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
    switch ( depthMode )
    {
        case DepthMode::DISABLED:
            {
                hr = m_device->CreateDepthStencilState( &depthStencilDesc, &m_depthStencilStates[ static_cast< int >( DepthMode::DISABLED ) ] );
                if ( !SUCCEEDED( hr ) )
                {
                    ERROR_AND_DIE( "CreateDepthStencilState for DepthMode::DISABLED failed." );
                }
                break;
            }

        case DepthMode::READ_ONLY_ALWAYS:
            {
                depthStencilDesc.DepthEnable    = TRUE;
                depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
                depthStencilDesc.DepthFunc      = D3D11_COMPARISON_ALWAYS;

                hr = m_device->CreateDepthStencilState( &depthStencilDesc, &m_depthStencilStates[ static_cast< int >( DepthMode::READ_ONLY_ALWAYS ) ] );
                if ( !SUCCEEDED( hr ) )
                {
                    ERROR_AND_DIE( "CreateDepthStencilState for DepthMode::READ_ONLY_ALWAYS failed." );
                }
                break;
            }

        case DepthMode::READ_ONLY_LESS_EQUAL:
            {
                depthStencilDesc.DepthEnable    = TRUE;
                depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
                depthStencilDesc.DepthFunc      = D3D11_COMPARISON_LESS_EQUAL;

                hr = m_device->CreateDepthStencilState( &depthStencilDesc, &m_depthStencilStates[ static_cast< int >( DepthMode::READ_ONLY_LESS_EQUAL ) ] );
                if ( !SUCCEEDED( hr ) )
                {
                    ERROR_AND_DIE( "CreateDepthStencilState for DepthMode::READ_ONLY_LESS_EQUAL failed." );
                }
                break;
            }

        case DepthMode::READ_WRITE_LESS_EQUAL:
            {
                depthStencilDesc.DepthEnable    = TRUE;
                depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
                depthStencilDesc.DepthFunc      = D3D11_COMPARISON_LESS_EQUAL;
                hr                              = m_device->CreateDepthStencilState( &depthStencilDesc, &m_depthStencilStates[ static_cast< int >( DepthMode::READ_WRITE_LESS_EQUAL ) ] );
                if ( !SUCCEEDED( hr ) )
                {
                    ERROR_AND_DIE( "CreateDepthStencilState for DepthMode::READ_WRITE_LESS_EQUAL failed." );
                }
                break;
            }
    }
}

//------------------------------------------------------------------------------------------------
void Renderer::SetDepthMode( DepthMode depthMode )
{
    m_desiredDepthMode = depthMode;
}

//------------------------------------------------------------------------------------------------
void Renderer::ResetSamplerModes()
{
    for ( int slot = 0; slot < static_cast< int >( ResourceSlot::COUNT ); ++slot )
    {
        SetSamplerMode( SamplerMode::POINT_CLAMP, static_cast< ResourceSlot >( slot ) );
    }
}

//------------------------------------------------------------------------------------------------
void Renderer::ReleaseDepthResources()
{
    for ( int i = 0; i < static_cast< int >( DepthMode::COUNT ); ++i )
    {
        DX_SAFE_RELEASE( m_depthStencilStates[ i ] );
    }

    DX_SAFE_RELEASE( m_depthStencilTexture );
    DX_SAFE_RELEASE( m_depthStencilDSV );
}

//------------------------------------------------------------------------------------------------
RenderTexture* Renderer::CreateRenderTexture()
{
    RenderTexture*           newRenderTexture = new RenderTexture();
    HRESULT                  hr;

    TextureDescriptionConfig config;
    config.width     = g_engine->m_window->GetClientDimensions().x;
    config.height    = g_engine->m_window->GetClientDimensions().y;
    config.format    = DXGI_FORMAT_R16G16B16A16_FLOAT;
    config.bindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    D3D11_TEXTURE2D_DESC desc = CreateTextureDescription( config );

    hr = m_device->CreateTexture2D( &desc, nullptr, &newRenderTexture->m_texture );
    GUARANTEE_OR_DIE( SUCCEEDED( hr ), "Failed to create Texture" );

    hr = m_device->CreateRenderTargetView( newRenderTexture->m_texture, nullptr, &newRenderTexture->m_renderTargetView );
    GUARANTEE_OR_DIE( SUCCEEDED( hr ), "Failed to create renderTargetView" );

    hr = m_device->CreateShaderResourceView( newRenderTexture->m_texture, nullptr, &newRenderTexture->m_shaderResourceView );
    GUARANTEE_OR_DIE( SUCCEEDED( hr ), "Failed to create shaderResourceView" );
    return newRenderTexture;
}

//------------------------------------------------------------------------------------------------
RenderTexture* Renderer::CreateRenderTexture( unsigned int width, unsigned int height, DXGI_FORMAT format )
{
    RenderTexture*           newRenderTexture = new RenderTexture();

    TextureDescriptionConfig config;
    config.width     = width;
    config.height    = height;
    config.mipLevels = 1;
    config.arraySize = 1;
    config.format    = format;
    config.bindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    D3D11_TEXTURE2D_DESC desc = CreateTextureDescription( config );

    HRESULT              hr = m_device->CreateTexture2D( &desc, nullptr, &newRenderTexture->m_texture );
    GUARANTEE_OR_DIE( SUCCEEDED( hr ), "Failed to create render texture." );

    hr = m_device->CreateRenderTargetView( newRenderTexture->m_texture, nullptr, &newRenderTexture->m_renderTargetView );
    GUARANTEE_OR_DIE( SUCCEEDED( hr ), "Failed to create render texture RTV." );

    hr = m_device->CreateShaderResourceView( newRenderTexture->m_texture, nullptr, &newRenderTexture->m_shaderResourceView );
    GUARANTEE_OR_DIE( SUCCEEDED( hr ), "Failed to create render texture SRV." );

    return newRenderTexture;
}

//------------------------------------------------------------------------------------------------
void Renderer::ConvertEquirectangularToCubemap( Texture* texture )
{
    BindShader( m_equirectangularToCubemap );
    BindTexture( texture, ResourceSlot::ENVIRONMENTCUBEMAP );

    SetSamplerMode( SamplerMode::BILINEAR_WRAP, ResourceSlot::ENVIRONMENTCUBEMAP );
    SetDepthMode( DepthMode::DISABLED );
    SetRasterizerMode( RasterizerMode::SOLID_CULL_NONE );

    EulerAngles faceOrientations[ 6 ] = { EulerAngles( 0.f, 0.f, -90.f ), EulerAngles( 180.f, 0.f, 90.f ), EulerAngles( 90.f, 0.f, 180.f ), EulerAngles( -90.f, 0.f, 0.f ), EulerAngles( 0.f, -90.f, -90.f ), EulerAngles( 0.f, 90.f, -90.f ) };
    for ( int faceIndex = 0; faceIndex < 6; ++faceIndex )
    {
        ID3D11RenderTargetView* faceRTV = m_environmentTexture->m_renderTargetViews[ faceIndex ];
        SetRenderTarget( faceRTV, nullptr );
        ClearRenderTarget( faceRTV );

        Camera faceCamera;
        faceCamera.SetPerspectiveView( 1.f, 90.f, 0.1f, 10.f );
        faceCamera.SetPositionAndOrientation( Vec3::ZERO, faceOrientations[ faceIndex ] );
        faceCamera.SetCameraToRenderTransform( Mat44::MakeCameraToRenderTransform() );

        BeginCamera( faceCamera, 512.f, 512.f );
        DrawVertexArray( m_cubeVertices );
        EndCamera( faceCamera );
    }

    m_deviceContext->OMSetRenderTargets( 0, nullptr, nullptr );
    m_deviceContext->GenerateMips( m_environmentTexture->m_shaderResourceView );

    UnbindTexture( ResourceSlot::ENVIRONMENTCUBEMAP );
    SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
    SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
    BindDefaultShader();

    SetRenderTarget( m_renderTargetView, m_depthStencilDSV );
    ClearRenderTarget( m_renderTargetView );
}

//------------------------------------------------------------------------------------------------
void Renderer::GenerateIrradianceCubemap()
{
    UnbindTexture( ResourceSlot::IRRADIANCE );
    BindShader( m_irradianceConvolution );
    BindTexture( m_environmentTexture, ResourceSlot::ENVIRONMENTCUBEMAP );
    SetSamplerMode( SamplerMode::BILINEAR_CLAMP, ResourceSlot::ENVIRONMENTCUBEMAP );
    SetDepthMode( DepthMode::DISABLED );
    SetRasterizerMode( RasterizerMode::SOLID_CULL_NONE );
    SetModelConstants();

    EulerAngles faceOrientations[ 6 ] = { EulerAngles( 0.f, 0.f, -90.f ), EulerAngles( 180.f, 0.f, 90.f ), EulerAngles( 90.f, 0.f, 180.f ), EulerAngles( -90.f, 0.f, 0.f ), EulerAngles( 0.f, -90.f, -90.f ), EulerAngles( 0.f, 90.f, -90.f ) };

    for ( int faceIndex = 0; faceIndex < 6; ++faceIndex )
    {
        ID3D11RenderTargetView* faceRTV = m_irradianceTexture->m_renderTargetViews[ faceIndex ];
        SetRenderTarget( faceRTV, nullptr );
        ClearRenderTarget( faceRTV );

        Camera faceCamera;
        faceCamera.SetPerspectiveView( 1.f, 90.f, 0.1f, 10.f );
        faceCamera.SetPositionAndOrientation( Vec3::ZERO, faceOrientations[ faceIndex ] );
        faceCamera.SetCameraToRenderTransform( Mat44::MakeCameraToRenderTransform() );

        BeginCamera( faceCamera, 32.f, 32.f );
        DrawVertexArray( m_cubeVertices );
        EndCamera( faceCamera );
    }

    m_deviceContext->OMSetRenderTargets( 0, nullptr, nullptr );

    UnbindTexture( ResourceSlot::ENVIRONMENTCUBEMAP );
    SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
    SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
    BindDefaultShader();

    SetRenderTarget( m_renderTargetView, m_depthStencilDSV );
    ClearRenderTarget( m_renderTargetView );
}

//------------------------------------------------------------------------------------------------
void Renderer::BeginShadowPass()
{
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX       = 0.f;
    viewport.TopLeftY       = 0.f;
    viewport.Width          = 2048.f;
    viewport.Height         = 2048.f;
    viewport.MinDepth       = 0.f;
    viewport.MaxDepth       = 1.f;

    UnbindTexture( ResourceSlot::SHADOWMAP );
    g_engine->m_render->m_deviceContext->RSSetViewports( 1, &viewport );
    g_engine->m_render->m_deviceContext->OMSetRenderTargets( 0, nullptr, m_shadowMapTexture->m_depthStencilView );

    g_engine->m_render->m_deviceContext->ClearDepthStencilView( m_shadowMapTexture->m_depthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0 );
    g_engine->m_render->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
    BindShader( m_shadowMap );
}

//------------------------------------------------------------------------------------------------
void Renderer::EndShadowPass()
{
    m_deviceContext->OMSetRenderTargets( 0, nullptr, nullptr );
}

//------------------------------------------------------------------------------------------------
void Renderer::GeneratePrefilteredCubemap()
{
    UnbindTexture( ResourceSlot::PREFILTERED );

    BindShader( m_prefilterEnvironment );
    BindTexture( m_environmentTexture, ResourceSlot::ENVIRONMENTCUBEMAP );
    SetSamplerMode( SamplerMode::BILINEAR_CLAMP, ResourceSlot::ENVIRONMENTCUBEMAP );
    SetDepthMode( DepthMode::DISABLED );
    SetRasterizerMode( RasterizerMode::SOLID_CULL_NONE );
    SetModelConstants();

    D3D11_TEXTURE2D_DESC prefilteredTextureDesc = {};
    m_prefilteredTexture->m_texture->GetDesc( &prefilteredTextureDesc );

    EulerAngles faceOrientations[ 6 ] = {
        EulerAngles( 0.f, 0.f, -90.f ),
        EulerAngles( 180.f, 0.f, 90.f ),
        EulerAngles( 90.f, 0.f, 180.f ),
        EulerAngles( -90.f, 0.f, 0.f ),
        EulerAngles( 0.f, -90.f, -90.f ),
        EulerAngles( 0.f, 90.f, -90.f )
    };

    for ( unsigned int mipIndex = 0; mipIndex < prefilteredTextureDesc.MipLevels; ++mipIndex )
    {
        unsigned int mipWidth  = prefilteredTextureDesc.Width >> mipIndex;
        unsigned int mipHeight = prefilteredTextureDesc.Height >> mipIndex;

        if ( mipWidth == 0 ) mipWidth = 1;
        if ( mipHeight == 0 ) mipHeight = 1;

        float              roughness = static_cast< float >( mipIndex ) / static_cast< float >( prefilteredTextureDesc.MipLevels - 1 );

        PrefilterConstants constants = {};
        constants.c_roughness        = roughness;
        CopyCPUToGPU( &constants, sizeof( PrefilterConstants ), m_prefilterCBO );
        BindConstantBuffer( k_prefilterConstantsSlot, m_prefilterCBO );

        for ( unsigned int faceIndex = 0; faceIndex < 6; ++faceIndex )
        {
            unsigned int            rtvIndex   = mipIndex * 6 + faceIndex;
            ID3D11RenderTargetView* faceMipRTV = m_prefilteredTexture->m_renderTargetViews[ rtvIndex ];

            SetRenderTarget( faceMipRTV, nullptr );
            ClearRenderTarget( faceMipRTV );

            Camera faceCamera;
            faceCamera.SetPerspectiveView( 1.f, 90.f, 0.1f, 10.f );
            faceCamera.SetPositionAndOrientation( Vec3::ZERO, faceOrientations[ faceIndex ] );
            faceCamera.SetCameraToRenderTransform( Mat44::MakeCameraToRenderTransform() );

            BeginCamera( faceCamera, static_cast< float >( mipWidth ), static_cast< float >( mipHeight ) );
            DrawVertexArray( m_cubeVertices );
            EndCamera( faceCamera );
        }
    }

    m_deviceContext->OMSetRenderTargets( 0, nullptr, nullptr );

    UnbindTexture( ResourceSlot::ENVIRONMENTCUBEMAP );
    SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
    SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
    BindDefaultShader();

    SetRenderTarget( m_renderTargetView, m_depthStencilDSV );
    ClearRenderTarget( m_renderTargetView );
}

//------------------------------------------------------------------------------------------------
void Renderer::GenerateBRDFLUT()
{
    UnbindTexture( ResourceSlot::BRDF_LUT );

    BindShader( m_brdfIntegration );
    SetBlendMode( BlendMode::OPAQUE );
    SetDepthMode( DepthMode::DISABLED );
    SetRasterizerMode( RasterizerMode::SOLID_CULL_NONE );

    D3D11_TEXTURE2D_DESC textureDesc = {};
    m_brdfLUTTexture->m_texture->GetDesc( &textureDesc );

    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX       = 0.0f;
    viewport.TopLeftY       = 0.0f;
    viewport.Width          = static_cast< float >( textureDesc.Width );
    viewport.Height         = static_cast< float >( textureDesc.Height );
    viewport.MinDepth       = 0.0f;
    viewport.MaxDepth       = 1.0f;

    m_deviceContext->RSSetViewports( 1, &viewport );
    SetRenderTarget( m_brdfLUTTexture->m_renderTargetView, nullptr );
    ClearRenderTarget( m_brdfLUTTexture->m_renderTargetView );

    DrawFullQuad();

    m_deviceContext->OMSetRenderTargets( 0, nullptr, nullptr );

    SetBlendMode( BlendMode::ALPHA );
    SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
    SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
    BindDefaultShader();

    SetRenderTarget( m_renderTargetView, m_depthStencilDSV );
    ClearRenderTarget( m_renderTargetView );
}

//------------------------------------------------------------------------------------------------
void Renderer::InitializeFullQuad()
{
    AddVertsForQuad3D( m_fullQuadVertices, m_fullQuadIndices, Vec3( -1.f, -1.f, 0.f ), Vec3( 1.f, -1.f, 0.f ), Vec3( 1.f, 1.f, 0.f ), Vec3( -1.f, 1.f, 0.f ), Rgba8::WHITE, AABB2::ZERO_TO_ONE );

    unsigned int vertexBufferStride = sizeof( Vertex );
    unsigned int vertexBufferSize   = static_cast< unsigned int >( m_fullQuadVertices.size() ) * vertexBufferStride;
    m_fullQuadVertexBuffer          = CreateVertexBuffer( vertexBufferSize, vertexBufferStride );
    CopyCPUToGPU( m_fullQuadVertices.data(), static_cast< unsigned int >( m_fullQuadVertices.size() ) * sizeof( Vertex ), m_fullQuadVertexBuffer );

    unsigned int indexBufferStride = sizeof( unsigned int );
    unsigned int indexBufferSize   = static_cast< unsigned int >( m_fullQuadIndices.size() ) * indexBufferStride;
    m_fullQuadIndexBuffer          = CreateIndexBuffer( indexBufferSize, indexBufferStride );
    CopyCPUToGPU( m_fullQuadIndices.data(), static_cast< unsigned int >( m_fullQuadIndices.size() ) * sizeof( unsigned int ), m_fullQuadIndexBuffer );
}

//------------------------------------------------------------------------------------------------
void Renderer::InitializeCube()
{
    AddVertsForAABB3D( m_cubeVertices, AABB3( Vec3( -1.f, -1.f, -1.f ), Vec3( 1.f, 1.f, 1.f ) ), Rgba8::WHITE, AABB2::ZERO_TO_ONE );

    unsigned int vertexBufferStride = sizeof( Vertex );
    unsigned int vertexBufferSize   = static_cast< unsigned int >( m_cubeVertices.size() ) * vertexBufferStride;
    m_cubeVertexBuffer              = CreateVertexBuffer( vertexBufferSize, vertexBufferStride );
    CopyCPUToGPU( m_cubeVertices.data(), static_cast< unsigned int >( m_cubeVertices.size() ) * sizeof( Vertex ), m_cubeVertexBuffer );

    unsigned int indexBufferStride = sizeof( unsigned int );
    unsigned int indexBufferSize   = static_cast< unsigned int >( m_cubeVertices.size() ) * indexBufferStride;
    m_cubeIndexBuffer              = CreateIndexBuffer( indexBufferSize, indexBufferStride );
    CopyCPUToGPU( m_cubeIndices.data(), static_cast< unsigned int >( m_cubeIndices.size() ) * sizeof( unsigned int ), m_cubeIndexBuffer );
}

//================================================================================================
// Debug
//================================================================================================
void Renderer::InitDebugLayer()
{
#if defined( ENGINE_DEBUG_RENDER )
    m_dxgiDebugModule = static_cast< void* >( LoadLibraryA( "dxgidebug.dll" ) );
    if ( m_dxgiDebugModule == nullptr )
    {
        ERROR_AND_DIE( "Could not load dxgidebug.dll." );
    }

    using GetDebugModuleCB = HRESULT( WINAPI* )( REFIID, void** );
    ( (GetDebugModuleCB)GetProcAddress( static_cast< HMODULE >( m_dxgiDebugModule ), "DXGIGetDebugInterface" ) )( __uuidof( IDXGIDebug ), &m_dxgiDebug );

    if ( m_dxgiDebug == nullptr )
    {
        ERROR_AND_DIE( "Could not load debug module." );
    }
#endif
}

//------------------------------------------------------------------------------------------------
void Renderer::ShutdownDebugLayer()
{
#if defined( ENGINE_DEBUG_RENDER )
    static_cast< IDXGIDebug* >( m_dxgiDebug )->ReportLiveObjects( DXGI_DEBUG_ALL, static_cast< DXGI_DEBUG_RLO_FLAGS >( DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL ) );

    static_cast< IDXGIDebug* >( m_dxgiDebug )->Release();
    m_dxgiDebug = nullptr;

    FreeLibrary( static_cast< HMODULE >( m_dxgiDebugModule ) );
    m_dxgiDebugModule = nullptr;
#endif
}