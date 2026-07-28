#pragma once

//-----------------------------------------------------------------------------------------------
#include "../Math/Vec4.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <string>
#include <vector>

//-----------------------------------------------------------------------------------------------
struct Rgba8;

//-----------------------------------------------------------------------------------------------
class Image
{
    friend class Renderer;

public:
    Image()  = default;
    ~Image() = default;
    Image( char const* imageFilePath );
    Image( IntVec2 size, Rgba8 color );
    Image( IntVec2 size, unsigned char const* imageDataBytes, int numComponents );

    IntVec2            GetDimensions() const;
    std::string const& GetImageFilePath() const;
    void const*        GetRawData() const;
    Rgba8              GetTexelColor( IntVec2 const& texelCoords ) const;

    void SetTexelColor( IntVec2 const& texelCoords, Rgba8 const& newColor );

private:
    std::string          m_imageFilePath;
    IntVec2              m_dimensions = IntVec2( 0, 0 );
    std::vector< Rgba8 > m_texelRgba8Data;  // or Rgba8* m_rgbaTexels = nullptr; if you prefer new[] and delete[]
    std::vector< Vec4 >  m_texelFloatData;
    bool                 m_isHDR = false;
};