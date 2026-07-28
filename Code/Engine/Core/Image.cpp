#define STB_IMAGE_IMPLEMENTATION  // Exactly one .CPP (this Image.cpp) should #define this before #including stb_image.h
#include "Engine/Core/Image.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "ThirdParty/stb/stb_image.h"

//-----------------------------------------------------------------------------------------------
Image::Image( char const* imageFilePath )
    : m_imageFilePath( imageFilePath )
{
    stbi_set_flip_vertically_on_load( 1 );
    int numComponents = 0;
    m_isHDR           = stbi_is_hdr( imageFilePath ) != 0;

    if ( m_isHDR )
    {
        float* imageDataFloats = stbi_loadf( imageFilePath, &m_dimensions.x, &m_dimensions.y, &numComponents, 4 );
        GUARANTEE_OR_DIE( imageDataFloats != nullptr, Stringf( "Failed to load HDR image %s", imageFilePath ) );

        int numTexels = m_dimensions.x * m_dimensions.y;
        m_texelFloatData.resize( numTexels );

        for ( int texelIndex = 0; texelIndex < numTexels; ++texelIndex )
        {
            Vec4& texel = m_texelFloatData[ texelIndex ];
            texel.x     = imageDataFloats[ texelIndex * 4 + 0 ];
            texel.y     = imageDataFloats[ texelIndex * 4 + 1 ];
            texel.z     = imageDataFloats[ texelIndex * 4 + 2 ];
            texel.w     = imageDataFloats[ texelIndex * 4 + 3 ];
        }

        stbi_image_free( imageDataFloats );
    }
    else
    {
        unsigned char* imageDataByes = stbi_load( imageFilePath, &m_dimensions.x, &m_dimensions.y, &numComponents, 0 );
        GUARANTEE_OR_DIE( imageDataByes != nullptr, Stringf( "Failed to load image %s", imageFilePath ) );
        //GUARANTEE_OR_DIE( numComponents == 3 || numComponents == 4, Stringf( "Can't load image from file %s; has %i components; we only support 3 or 4", imageFilePath, numComponents ) );

        int numTexels = m_dimensions.x * m_dimensions.y;
        m_texelRgba8Data.resize( numTexels );

        for ( int texelIndex = 0; texelIndex < numTexels; ++texelIndex )
        {
            Rgba8& texel = m_texelRgba8Data[ texelIndex ];
            texel.r      = imageDataByes[ ( texelIndex * numComponents ) + 0 ];
            texel.g      = imageDataByes[ ( texelIndex * numComponents ) + 1 ];
            texel.b      = imageDataByes[ ( texelIndex * numComponents ) + 2 ];
            texel.a      = ( numComponents == 3 ) ? 255 : imageDataByes[ ( texelIndex * numComponents ) + 3 ];
        }

        stbi_image_free( imageDataByes );
    }
}

//-----------------------------------------------------------------------------------------------
Image::Image( IntVec2 size, unsigned char const* imageDataBytes, int numComponents )
    : m_dimensions( size )
{
    GUARANTEE_OR_DIE( imageDataBytes != nullptr, "Image raw data was null" );
    GUARANTEE_OR_DIE( numComponents == 3 || numComponents == 4, Stringf( "Can't create image from raw data; has %i components; we only support 3 or 4", numComponents ) );

    int numTexels = m_dimensions.x * m_dimensions.y;
    m_texelRgba8Data.resize( numTexels );

    for ( int texelIndex = 0; texelIndex < numTexels; ++texelIndex )
    {
        Rgba8& texel = m_texelRgba8Data[ texelIndex ];

        texel.r = imageDataBytes[ texelIndex * numComponents + 0 ];
        texel.g = imageDataBytes[ texelIndex * numComponents + 1 ];
        texel.b = imageDataBytes[ texelIndex * numComponents + 2 ];
        texel.a = ( numComponents == 3 ) ? 255 : imageDataBytes[ texelIndex * numComponents + 3 ];
    }
}

//-----------------------------------------------------------------------------------------------
Image::Image( IntVec2 size, Rgba8 color )
    : m_dimensions( size )
{
    int numTexels = m_dimensions.x * m_dimensions.y;
    m_texelRgba8Data.resize( numTexels, color );
}

//-----------------------------------------------------------------------------------------------
IntVec2 Image::GetDimensions() const
{
    return m_dimensions;
}

//-----------------------------------------------------------------------------------------------
std::string const& Image::GetImageFilePath() const
{
    return m_imageFilePath;
}

//-----------------------------------------------------------------------------------------------
void const* Image::GetRawData() const
{
    return m_isHDR ? static_cast< void const* >( m_texelFloatData.data() ) : static_cast< void const* >( m_texelRgba8Data.data() );
}

//-----------------------------------------------------------------------------------------------
Rgba8 Image::GetTexelColor( IntVec2 const& texelCoords ) const
{
    int index = texelCoords.y * m_dimensions.x + texelCoords.x;
    return m_texelRgba8Data[ index ];
}

//-----------------------------------------------------------------------------------------------
void Image::SetTexelColor( IntVec2 const& texelCoords, Rgba8 const& newColor )
{
    int index                 = texelCoords.y * m_dimensions.x + texelCoords.x;
    m_texelRgba8Data[ index ] = newColor;
}