#include "DepthRenderTexture.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <d3d11.h>

//-----------------------------------------------------------------------------------------------
DepthRenderTexture::~DepthRenderTexture()
{
    DX_SAFE_RELEASE( m_depthStencilView );
}