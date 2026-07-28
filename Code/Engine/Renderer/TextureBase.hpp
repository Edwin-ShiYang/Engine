#pragma once

//-----------------------------------------------------------------------------------------------
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;

//-----------------------------------------------------------------------------------------------
class TextureBase
{
    friend class Renderer;

protected:
    TextureBase()                          = default;
    TextureBase( TextureBase const& copy ) = delete;

public:
    virtual ~TextureBase();

protected:
    ID3D11ShaderResourceView* m_shaderResourceView = nullptr;
    ID3D11Texture2D*          m_texture            = nullptr;
};