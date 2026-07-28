#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Renderer/TextureBase.hpp"

//-----------------------------------------------------------------------------------------------
struct ID3D11RenderTargetView;

//-----------------------------------------------------------------------------------------------
class RenderTexture : public TextureBase
{
    friend class Renderer;

private:
    RenderTexture()                            = default;
    RenderTexture( RenderTexture const& copy ) = delete;
    ~RenderTexture();

protected:
    ID3D11RenderTargetView* m_renderTargetView = nullptr;
};