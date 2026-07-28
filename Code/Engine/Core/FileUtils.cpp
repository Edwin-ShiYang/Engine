#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#include <sstream>

//-----------------------------------------------------------------------------------------------
int FileReadToBuffer( std::vector<uint8_t>& outBuffer, std::string const& filename )
{
	FILE* file = nullptr;
	errno_t err = fopen_s( &file, filename.c_str(), "rb" );

	GUARANTEE_OR_DIE( err == 0, Stringf( "Cannot open the file: %s ", filename.c_str() ) );

	fseek( file, 0, SEEK_END );
	long fileSize = ftell( file );

	rewind( file );

	if ( fileSize > 0 )
	{
		outBuffer.resize( fileSize );
		size_t bytesRead = fread( outBuffer.data(), 1, fileSize, file );
		GUARANTEE_OR_DIE( bytesRead == static_cast< size_t > ( fileSize ), Stringf( "Incomplete file load for: %s", filename.c_str() ) );
	}

	fclose( file );

	return static_cast< int > ( outBuffer.size() );
}

//-----------------------------------------------------------------------------------------------
int FileReadToString( std::string& outString, std::string const& filename )
{
	std::vector<uint8_t> buffer;
	int numBytes = FileReadToBuffer( buffer, filename );

	if ( numBytes == 0 )
	{
		outString.clear();
		return 0;
	}

	buffer.push_back( '\0' );
	outString = std::string( reinterpret_cast< char const* >( buffer.data() ), buffer.size() );

	return numBytes;
}

//-----------------------------------------------------------------------------------------------
void LoadMeshFromObj( std::vector< Vertex >& outVerts, std::string const& objFilePath )
{
	std::vector< Vec3 > positions;
	std::vector< Vec3 > normals;
	std::vector< Vec2 > uvTexCoords;

	std::string objSource;
	int size = FileReadToString( objSource, objFilePath );
	GUARANTEE_OR_DIE( size != 0, Stringf( "Obj is empty!" ) );

	std::stringstream fileStream( objSource );
	std::string line;
	while ( std::getline( fileStream, line ) )
	{
		std::stringstream lineStream( line );
		std::string prefix;
		lineStream >> prefix;

		if ( prefix != "v" && prefix != "vn" && prefix != "vt" && prefix != "f" )
		{
			continue;
		}
		if ( prefix == "v" )
		{
			float x, y, z;
			lineStream >> x >> y >> z;
			positions.push_back( Vec3( x, y, z ) );
		}
		else if ( prefix == "vn" )
		{
			float x, y, z;
			lineStream >> x >> y >> z;
			normals.push_back( Vec3( x, y, z ) );
		}
		else if ( prefix == "vt" )
		{
			float x, y;
			lineStream >> x >> y;
			uvTexCoords.push_back( Vec2( x, y ) );
		}
		else if ( prefix == "f" )
		{
			std::string rawReferences;
			while ( lineStream >> rawReferences ) {
				// v/vt/vn
				Strings references = SplitStringOnDelimiter( rawReferences, '/' );
				unsigned int positionIndex = stoi( references [ 0 ] ) - 1;
				unsigned int uvIndex = stoi( references [ 1 ] ) - 1;
				unsigned int normalIndex = stoi( references [ 2 ] ) - 1;

				Vec3 position = positions [ positionIndex ];
				Vec3 normal = normals [ normalIndex ];
				Vec2 uvTexCoord = uvTexCoords [ uvIndex ];

				Vertex vertex( position, Rgba8::WHITE, uvTexCoord, Vec3::ZERO, Vec3::ZERO, normal );
				outVerts.push_back( vertex );
			}
		}
	}
}