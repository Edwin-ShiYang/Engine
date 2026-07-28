#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Renderer/TextureBase.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <string>

//-----------------------------------------------------------------------------------------------
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;

//-----------------------------------------------------------------------------------------------
class Texture : public TextureBase
{
    friend class Renderer;

private:
    Texture();
    Texture( Texture const& copy ) = delete;
    ~Texture();

public:
    IntVec2 GetDimensions() const
    {
        return m_dimensions;
    }
    std::string const& GetImageFilePath() const
    {
        return m_name;
    }

protected:
    std::string m_name;
    IntVec2     m_dimensions;
};