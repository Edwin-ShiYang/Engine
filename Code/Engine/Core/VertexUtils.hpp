#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/Vec2.hpp"
#include <vector>

//-----------------------------------------------------------------------------------------------
struct AABB2;
struct AABB3;
struct OBB2;
struct Mat44;

//-----------------------------------------------------------------------------------------------
void TransformVertexArrayXY3D( int numVerts, Vertex* verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY );
void TransTextVertexArrayXY3D( std::vector< Vertex >& verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY );
void TransformVertexArray3D( std::vector< Vertex >& verts, Mat44 const& transform );

void AddVertsForDisc2D( std::vector< Vertex >& verts, Vec2 discCenter, float discRadius, Rgba8 color, int numSides = 32 );

void AddVertsForAABB2D( std::vector< Vertex >& verts, AABB2 const& alignedBox, Rgba8 color );
void AddVertsForAABB2D( std::vector< Vertex >& verts, AABB2 const& alignedBox, Rgba8 color, AABB2 const& uvs );

void AddVertsForOBB2D( std::vector< Vertex >& verts, OBB2 const& orientedBox, Rgba8 color );
void AddVertsForRing2D( std::vector< Vertex >& verts, Vec2 ringCenter, float ringRadius, float thickness, Rgba8 color, int numSides = 32 );

void AddVertsForCapsule2D( std::vector< Vertex >& verts, Vec2 boneStart, Vec2 boneEnd, float radius, Rgba8 color );
void AddVertsForCapsule3D( std::vector< Vertex >& verts, std::vector< unsigned int >& indexes, Vec2 const& centerXY, FloatRange const& minMaxz, float radius, Rgba8 const& color = Rgba8::WHITE, float numSlices = 8.f, float numStacks = 4.f );

void AddVertsForLineSegment2D( std::vector< Vertex >& verts, Vec2 start, Vec2 end, Vec2 thickness, Rgba8 color );
void AddVertsForLineSegment3D( std::vector< Vertex >& verts, Vec3 const& start, Vec3 const& end, float thickness, Rgba8 const& color );

void AddVertsForTriangle2D( std::vector< Vertex >& verts, Vec2 ccw0, Vec2 ccw1, Vec2 ccw2, Rgba8 color );
void AddVertsForTriangle3D( std::vector< Vertex >& verts, Vec3 ccw0, Vec3 ccw1, Vec3 ccw2, Rgba8 color, Vec2 uv0, Vec2 uv1, Vec2 uvPole );
void AddVertsForTriangle3D( std::vector< Vertex >& verts, Vec3 ccw0, Vec3 ccw1, Vec3 ccw2, Rgba8 color );
void AddVertsForTriangle3D( std::vector< Vertex >& verts, Vec3 ccw0, Vec3 ccw1, Vec3 ccw2, Rgba8 color, Vec2 uv0, Vec2 uv1, Vec2 uv2, Vec3 normal );

void AddVertsForSector( std::vector< Vertex >& verts, Vec2 sectorCenter, float sectorRadius, float orientation, float apertureDegrees, Rgba8 color, int numSides = 32 );
void AddVertsForArrow2D( std::vector< Vertex >& verts, Vec2 const& start, Vec2 const& end, float thickness, float arrowRadius, Rgba8 const& color );

void AddVertsForQuad3D( std::vector< Vertex >& verts, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE );
void AddVertsForQuad3D( std::vector< Vertex >& vertexes, std::vector< unsigned int >& indexes, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE );

void AddVertsForUVSphereZ3D( std::vector< Vertex >& verts, Vec3 const& center, float radius, float numSlices = 32.f, float numStacks = 16.f, Rgba8 const& tint = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE );
void AddVertsForUVSphereZ3D( std::vector< Vertex >& verts, std::vector< unsigned int >& indexes, Vec3 const& center, float radius, float numSlices = 32.f, float numStacks = 16.f, Rgba8 const& tint = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE );
void AddVertsForUVSphereZWireFrame3D( std::vector< Vertex >& verts, Vec3 const& center, float radius, float numSlices = 32.f, float numStacks = 16.f, float lineThickness = 0.01f, Rgba8 const& tint = Rgba8::WHITE );

void AddVertsForArrow3D( std::vector< Vertex >& verts, Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color = Rgba8::WHITE, int numSlices = 32 );

void AddVertsForCylinder3D( std::vector< Vertex >& verts, Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE, int numSlices = 32 );
void AddVertsForCylinder3D( std::vector< Vertex >& verts, Vec2 const& centerXY, FloatRange const& minMaxz, float radius, float numSlices, Rgba8 const& tint = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE );
void AddVertsForCylinder3D( std::vector< Vertex >& verts, std::vector< unsigned int >& indexes, Vec2 const& centerXY, FloatRange const& minMaxz, float radius, float numSlices, Rgba8 const& tint = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE );

void AddVertsForCylinderZWireFrame3D( std::vector< Vertex >& verts, Vec2 const& centerXY, FloatRange const& minMaxz, float radius, float numSlices, float lineThickness, Rgba8 const& tint = Rgba8::WHITE );

void AddVertsForCone3D( std::vector< Vertex >& verts, Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE, int numSlices = 32 );

void AddVertsForAABB3D( std::vector< Vertex >& verts, AABB3 const& bounds, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE );
void AddVertsForAABB3D( std::vector< Vertex >& verts, AABB3 const& bounds, Rgba8 const& colXPos, Rgba8 const& colXNeg, Rgba8 const& colYPos, Rgba8 const& colYNeg, Rgba8 const& colZPos, Rgba8 const& colZNeg, AABB2 const& UVs = AABB2::ZERO_TO_ONE );
void AddVertsForAABB3D( std::vector< Vertex >& verts, std::vector< unsigned int >& indexes, AABB3 const& bounds, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE );

void AddVertsForAABBWireFrame3D( std::vector< Vertex >& verts, AABB3 const& bounds, float lineThickness, Rgba8 const& tint = Rgba8::WHITE );

void AddVertsForPlaneWireFrame3D( std::vector< Vertex >& verts, Plane3 const& plane, int size, float lineThickness );

void AddVertsForQuadWireFrame( std::vector< Vertex >& verts, float lineThickness, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color = Rgba8::WHITE );

void AddVertsForQuad2D( std::vector< Vertex >& verts, Vec2 ccw0, Vec2 ccw1, Vec2 ccw2, Vec2 ccw3, Rgba8 tint = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE );
void AddVertsForQuad2D( std::vector< Vertex >& verts, AABB2 const& bounds, Rgba8 const& tint = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE );

void AddVertsForOBB3D( std::vector< Vertex >& verts, OBB3 const& orientedBox, Rgba8 const& tint = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE );
void AddVertsForOBB3D( std::vector< Vertex >& verts, std::vector< unsigned int >& indexes, OBB3 const& orientedBox, Rgba8 const& tint = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE );
void AddVertsForOBBWireFrame3D( std::vector< Vertex >& verts, OBB3 const& orientedBox, float lineThickness, Rgba8 const& tint = Rgba8::WHITE );

void AddVertsAtOriginXForward( std::vector< Vertex >& verts, AABB2 bounds, Vec2 const& pivot );

AABB2 GetVertexBounds2D( std::vector< Vertex >& verts );

void IndexifyStaticMesh( std::vector< Vertex >& vertexArray_InOut, std::vector< unsigned int >& indexArray_Out );

void GenerateNormalsPerSurface( std::vector< Vertex >& vertices, std::vector< unsigned int >& indices );
void GenerateNormalsPerVertex( std::vector< Vertex >& vertices, std::vector< unsigned int >& indices );
void GenerateTangentsFromUVs( std::vector< Vertex >& vertices, std::vector< unsigned int >& indices, bool hasNormal = true );
void GenerateTangentsArbitrary( std::vector< Vertex >& vertices );