#include "Engine/Renderer/TextureBase.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <d3d11.h>

//-----------------------------------------------------------------------------------------------
TextureBase::~TextureBase()
{
    DX_SAFE_RELEASE( m_texture );
    DX_SAFE_RELEASE( m_shaderResourceView );
}