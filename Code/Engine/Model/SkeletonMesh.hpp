#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Model/SkeletonMeshSection.hpp"
#include "Engine/Math/Mat44.hpp"

//-----------------------------------------------------------------------------------------------
#include <vector>

//-----------------------------------------------------------------------------------------------
struct Node;
struct Material;
struct SkeletonModel;

//-----------------------------------------------------------------------------------------------
class SkeletonMesh
{
public:
    SkeletonMesh()  = default;
    ~SkeletonMesh() = default;

    void                               RenderMesh( SkeletonModel const* skeletonModel, Mat44 const& modelToWorldTransform ) const;
    void                               Render();

    std::vector< SkeletonMeshSection > m_sections;
};