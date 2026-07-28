#pragma once

//-----------------------------------------------------------------------------------------------
#include <vector>
#include <string>
#include "Vertex.hpp"

//-----------------------------------------------------------------------------------------------
int  FileReadToBuffer( std::vector<uint8_t>& outBuffer, std::string const& filename );
int  FileReadToString( std::string& outString, std::string const& filename );

void LoadMeshFromObj( std::vector< Vertex >& outVerts, std::string const& objFilePath );