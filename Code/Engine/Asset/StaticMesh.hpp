#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Model/StaticMeshSection.hpp"
#include "Engine/Math/Mat44.hpp"

//-----------------------------------------------------------------------------------------------
#include <vector>

//-----------------------------------------------------------------------------------------------
class IndexBuffer;
class VertexBuffer;
class Model;
struct Node;
struct Material;

//-----------------------------------------------------------------------------------------------
class StaticMesh
{
public:
    StaticMesh()  = default;
    ~StaticMesh() = default;

    //#todo model should store the convertMatrix
    void                             Render( std::vector< Node > const& nodes, std::vector< Material > const& materials, Mat44 const& modelToWorldTransform, Mat44 const& toEngineMatrix ) const;
    std::vector< StaticMeshSection > m_sections;
};