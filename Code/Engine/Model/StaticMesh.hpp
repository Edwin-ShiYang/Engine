#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Model/StaticMeshSection.hpp"
#include "Engine/Math/Mat44.hpp"

//-----------------------------------------------------------------------------------------------
#include <vector>

//-----------------------------------------------------------------------------------------------
struct StaticModel;
struct Material;

//-----------------------------------------------------------------------------------------------
class StaticMesh
{
public:
    StaticMesh()  = default;
    ~StaticMesh() = default;

    void                             Render( StaticModel const* staticModel, Mat44 const& modelToWorldTransform ) const;
    std::vector< StaticMeshSection > m_sections;
};