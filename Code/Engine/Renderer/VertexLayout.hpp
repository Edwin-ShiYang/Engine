#pragma once

//----------------------------------------------------------------------------------------------
#include "dxgiformat.h"
#include <vector>

//----------------------------------------------------------------------------------------------
enum class VertexLayoutType
{
    PCUTBN,
    Skinned,
};

//----------------------------------------------------------------------------------------------
enum class VertexLayoutAttribute
{
    POSITION,
    COLOR,
    UVTEXCOORDS,
    TANGENT,
    BITANGENT,
    NORMAL,
    JOINTINDICES,
    JOINTWEIGHTS
};

//----------------------------------------------------------------------------------------------
struct VertexLayout
{
    VertexLayoutAttribute m_semanticName;
    unsigned int          m_alignedByteOffset;
    DXGI_FORMAT           m_format;

    VertexLayout( VertexLayoutAttribute semanticName, unsigned int alignedByteOffset, DXGI_FORMAT format )
        : m_semanticName( semanticName )
        , m_alignedByteOffset( alignedByteOffset )
        , m_format( format )
    {
    }
};

//----------------------------------------------------------------------------------------------
struct VertexDescriptor
{
    std::vector< VertexLayout > m_attributes;
};