#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/Mat44.hpp"

//-----------------------------------------------------------------------------------------------
#include <map>
#include <string>
#include <vector>

//-----------------------------------------------------------------------------------------------
class Texture;

//-----------------------------------------------------------------------------------------------
class OBJModel
{
public:
    std::vector< Vertex >       m_vertices;
    std::vector< unsigned int > m_indices;
    Texture const*              m_diffuse = nullptr;
    Texture const*              m_normal  = nullptr;
    Texture const*              m_sge     = nullptr;
};

//-----------------------------------------------------------------------------------------------
class OBJImporter
{
public:
    static std::map< std::string, OBJModel > s_models;

    static bool            CreateOrGetModelFromFile( std::string modelPath, std::string diffusePath = "", std::string normalPath = "", std::string sgePath = "" );
    static OBJModel const& GetLoadedModelByName( std::string name );
};