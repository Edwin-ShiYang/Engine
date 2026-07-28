#include "Engine/Renderer/CubemapTexture.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <d3d11.h>

//-----------------------------------------------------------------------------------------------
CubemapTexture::~CubemapTexture()
{
    for ( ID3D11RenderTargetView*& renderTargetView : m_renderTargetViews )
    {
        DX_SAFE_RELEASE( renderTargetView );
    }
}