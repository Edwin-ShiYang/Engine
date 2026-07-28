#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Renderer/TextureBase.hpp"

//-----------------------------------------------------------------------------------------------
struct ID3D11DepthStencilView;

//-----------------------------------------------------------------------------------------------
class DepthRenderTexture : public TextureBase
{
    friend class Renderer;

private:
    DepthRenderTexture()                                 = default;
    DepthRenderTexture( DepthRenderTexture const& copy ) = delete;
    ~DepthRenderTexture();

protected:
    ID3D11DepthStencilView* m_depthStencilView = nullptr;
};