#include "Engine/Renderer/RendererTexture.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <d3d11.h>

//-----------------------------------------------------------------------------------------------
RenderTexture::~RenderTexture()
{
    DX_SAFE_RELEASE( m_renderTargetView );
}