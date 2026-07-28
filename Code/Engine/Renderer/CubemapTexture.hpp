#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Renderer/TextureBase.hpp"
struct ID3D11RenderTargetView;

//-----------------------------------------------------------------------------------------------
#include <vector>

//-----------------------------------------------------------------------------------------------
class CubemapTexture : public TextureBase
{
    friend class Renderer;

private:
    CubemapTexture()                             = default;
    CubemapTexture( CubemapTexture const& copy ) = delete;
    ~CubemapTexture();

protected:
    // ID3D11RenderTargetView*                m_renderTargetViews[ 6 ] = {};
    std::vector< ID3D11RenderTargetView* > m_renderTargetViews;
    unsigned int                           m_mipLevelCount = 0;
};