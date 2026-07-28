#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/OBB2.hpp"
#include "ErrorWarningAssert.hpp"
#include <vector>

#include "Engine/Math/RandomNumberGenerator.hpp"

//-----------------------------------------------------------------------------------------------
void TransformVertexArrayXY3D( int numVerts, Vertex* verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY )
{
    Vec2 iBasis = Vec2::MakeFromPolarDegrees( rotationDegreesAboutZ, uniformScaleXY );
    Vec2 jBasis = iBasis.GetRotatedBy90Degrees();

    for ( int vertIndex = 0; vertIndex < numVerts; ++vertIndex )
    {
        TransformPositionXY3D( verts[ vertIndex ].m_position, iBasis, jBasis, translationXY );
    }
}

//-----------------------------------------------------------------------------------------------
void AddVertsForDisc2D( std::vector< Vertex >& verts, Vec2 discCenter, float discRadius, Rgba8 color, int const numSides )
{
    float const         degreePerSide = 360.f / static_cast< float >( numSides );

    std::vector< Vec2 > vertPositions( numSides );
    for ( int sideNum = 0; sideNum < numSides; ++sideNum )
    {
        float degree               = degreePerSide * static_cast< float >( sideNum );
        vertPositions[ sideNum ].x = discRadius * CosDegrees( degree );
        vertPositions[ sideNum ].y = discRadius * SinDegrees( degree );
    }

    for ( int triNum = 0; triNum < numSides; ++triNum )
    {
        int         startRadiusIndex = triNum;
        int         endRadiusIndex   = ( triNum + 1 ) % numSides;

        Vec2 const& secondVertOfs = vertPositions[ startRadiusIndex ];
        Vec2 const& thirdVertOfs  = vertPositions[ endRadiusIndex ];

        verts.push_back( Vertex{ Vec3( discCenter.x, discCenter.y, 0.f ), color, Vec2() } );
        verts.push_back( Vertex{ Vec3( discCenter.x + secondVertOfs.x, discCenter.y + secondVertOfs.y, 0.f ), color, Vec2( 0.f, 0.f ) } );
        verts.push_back( Vertex{ Vec3( discCenter.x + thirdVertOfs.x, discCenter.y + thirdVertOfs.y, 0.f ), color, Vec2( 0.f, 0.f ) } );
    }
}

void AddVertsForSector( std::vector< Vertex >& verts, Vec2 sectorCenter, float sectorRadius, float orientation, float apertureDegrees, Rgba8 color, int const numSides )
{
    std::vector< Vec2 > vertPositions;

    float               degreePerSide = apertureDegrees / static_cast< float >( numSides - 1 );
    float               startDegrees  = orientation - 90.f;

    for ( int sideNum = 0; sideNum < numSides; ++sideNum )
    {
        float degree = startDegrees + degreePerSide * sideNum;
        vertPositions.push_back( Vec2( sectorRadius * CosDegrees( degree ), sectorRadius * SinDegrees( degree ) ) );
    }

    for ( int triNum = 0; triNum < numSides - 1; ++triNum )
    {
        Vec2 const& secondVertOfs = vertPositions[ triNum ];
        Vec2 const& thirdVertOfs  = vertPositions[ triNum + 1 ];

        verts.push_back( Vertex{ Vec3( sectorCenter.x, sectorCenter.y, 0.f ), color, Vec2() } );
        verts.push_back( Vertex{ Vec3( sectorCenter.x + secondVertOfs.x, sectorCenter.y + secondVertOfs.y, 0.f ), color, Vec2( 0.f, 0.f ) } );
        verts.push_back( Vertex{ Vec3( sectorCenter.x + thirdVertOfs.x, sectorCenter.y + thirdVertOfs.y, 0.f ), color, Vec2( 0.f, 0.f ) } );
    }
}

//-----------------------------------------------------------------------------------------------
void AddVertsForAABB2D( std::vector< Vertex >& verts, AABB2 const& alignedBox, Rgba8 color )
{
    AddVertsForAABB2D( verts, alignedBox, color, AABB2::ZERO_TO_ONE );
}

//-----------------------------------------------------------------------------------------------
void AddVertsForAABB2D( std::vector< Vertex >& verts, AABB2 const& alignedBox, Rgba8 color, AABB2 const& uvs )
{
    Vec2 topLeft( alignedBox.m_mins.x, alignedBox.m_maxs.y );
    Vec2 topRight( alignedBox.m_maxs.x, alignedBox.m_maxs.y );
    Vec2 bottomLeft( alignedBox.m_mins.x, alignedBox.m_mins.y );
    Vec2 bottomRight( alignedBox.m_maxs.x, alignedBox.m_mins.y );

    Vec2 uvTopLeft( uvs.m_mins.x, uvs.m_maxs.y );
    Vec2 uvTopRight( uvs.m_maxs.x, uvs.m_maxs.y );
    Vec2 uvBottomLeft( uvs.m_mins.x, uvs.m_mins.y );
    Vec2 uvBottomRight( uvs.m_maxs.x, uvs.m_mins.y );

    verts.push_back( Vertex{ Vec3( topLeft.x, topLeft.y, 0.f ), color, uvTopLeft } );
    verts.push_back( Vertex{ Vec3( bottomLeft.x, bottomLeft.y, 0.f ), color, uvBottomLeft } );
    verts.push_back( Vertex{ Vec3( topRight.x, topRight.y, 0.f ), color, uvTopRight } );

    verts.push_back( Vertex{ Vec3( topRight.x, topRight.y, 0.f ), color, uvTopRight } );
    verts.push_back( Vertex{ Vec3( bottomLeft.x, bottomLeft.y, 0.f ), color, uvBottomLeft } );
    verts.push_back( Vertex{ Vec3( bottomRight.x, bottomRight.y, 0.f ), color, uvBottomRight } );
}

//-----------------------------------------------------------------------------------------------
void AddVertsForOBB2D( std::vector< Vertex >& verts, OBB2 const& orientedBox, Rgba8 color )
{
    Vec2 iBasis = orientedBox.m_iBasisNormal;
    Vec2 jBasis = iBasis.GetRotatedBy90Degrees();

    Vec2 topRight    = orientedBox.m_center + ( iBasis * orientedBox.m_halfDimensions.x ) + ( jBasis * orientedBox.m_halfDimensions.y );
    Vec2 topLeft     = orientedBox.m_center - ( iBasis * orientedBox.m_halfDimensions.x ) + ( jBasis * orientedBox.m_halfDimensions.y );
    Vec2 bottomLeft  = orientedBox.m_center - ( iBasis * orientedBox.m_halfDimensions.x ) - ( jBasis * orientedBox.m_halfDimensions.y );
    Vec2 bottomRight = orientedBox.m_center + ( iBasis * orientedBox.m_halfDimensions.x ) - ( jBasis * orientedBox.m_halfDimensions.y );

    verts.push_back( Vertex{ Vec3( topLeft.x, topLeft.y, 0.f ), color, Vec2() } );
    verts.push_back( Vertex{ Vec3( bottomLeft.x, bottomLeft.y, 0.f ), color, Vec2() } );
    verts.push_back( Vertex{ Vec3( bottomRight.x, bottomRight.y, 0.f ), color, Vec2() } );

    verts.push_back( Vertex{ Vec3( topLeft.x, topLeft.y, 0.f ), color, Vec2() } );
    verts.push_back( Vertex{ Vec3( bottomRight.x, bottomRight.y, 0.f ), color, Vec2() } );
    verts.push_back( Vertex{ Vec3( topRight.x, topRight.y, 0.f ), color, Vec2() } );
}

//-----------------------------------------------------------------------------------------------
void AddVertsForRing2D( std::vector< Vertex >& verts, Vec2 ringCenter, float ringRadius, float thickness, Rgba8 color, int const numSides )
{
    float       halfThickness = 0.5f * thickness;
    float       innerRadius   = ringRadius - halfThickness;
    float       outerRadius   = ringRadius + halfThickness;

    float const DEGREES_PER_SIDE = 360.f / static_cast< float >( numSides );

    for ( int sideNum = 0; sideNum < numSides; ++sideNum )
    {
        float startDegree = DEGREES_PER_SIDE * static_cast< float >( sideNum );
        float endDegrees  = DEGREES_PER_SIDE * static_cast< float >( sideNum + 1 );

        float cosStart = CosDegrees( startDegree );
        float sinStart = SinDegrees( startDegree );

        float cosEnd = CosDegrees( endDegrees );
        float sinEnd = SinDegrees( endDegrees );

        Vec3  innerStartPos( ringCenter.x + innerRadius * cosStart, ringCenter.y + innerRadius * sinStart, 0.f );
        Vec3  outerStartPos( ringCenter.x + outerRadius * cosStart, ringCenter.y + outerRadius * sinStart, 0.f );

        Vec3  outerEndPos( ringCenter.x + outerRadius * cosEnd, ringCenter.y + outerRadius * sinEnd, 0.f );
        Vec3  innerEndPos( ringCenter.x + innerRadius * cosEnd, ringCenter.y + innerRadius * sinEnd, 0.f );

        verts.push_back( Vertex{ innerEndPos, color, Vec2() } );
        verts.push_back( Vertex{ innerStartPos, color, Vec2() } );
        verts.push_back( Vertex{ outerStartPos, color, Vec2() } );

        verts.push_back( Vertex{ innerEndPos, color, Vec2() } );
        verts.push_back( Vertex{ outerStartPos, color, Vec2() } );
        verts.push_back( Vertex{ outerEndPos, color, Vec2() } );
    }
}

//-----------------------------------------------------------------------------------------------
void AddVertsForCapsule2D( std::vector< Vertex >& verts, Vec2 boneStart, Vec2 boneEnd, float radius, Rgba8 color )
{
    float orientation = ( boneEnd - boneStart ).GetOrientationDegrees();

    AddVertsForSector( verts, boneStart, radius, orientation - 180.f, 180.f, color );
    AddVertsForLineSegment2D( verts, boneStart, boneEnd, Vec2( radius * 2.f, radius * 2.f ), color );
    AddVertsForSector( verts, boneEnd, radius, orientation, 180.f, color );
}

//-----------------------------------------------------------------------------------------------
void AddVertsForCapsule3D( std::vector< Vertex >& verts, std::vector< unsigned int >& indexes, Vec2 const& centerXY, FloatRange const& minMaxz, float radius, Rgba8 const& color, float numSlices, float numStacks )
{
    Vec3         boneStart = Vec3( centerXY.x, centerXY.y, minMaxz.m_min );
    Vec3         boneEnd   = Vec3( centerXY.x, centerXY.y, minMaxz.m_max );

    float        degreesPerSlice = 360.f / numSlices;
    float        degreesPerStack = 90.f / numStacks;

    // slice
    unsigned int startIndex = static_cast< unsigned int >( verts.size() );
    for ( int i = 0; i < static_cast< int >( numSlices ); ++i )
    {
        float cosDegrees     = CosDegrees( static_cast< float >( i ) * degreesPerSlice ) * radius;
        float sinDegrees     = SinDegrees( static_cast< float >( i ) * degreesPerSlice ) * radius;
        float nextCosDegrees = CosDegrees( static_cast< float >( i + 1 ) * degreesPerSlice ) * radius;
        float nextSinDegrees = SinDegrees( static_cast< float >( i + 1 ) * degreesPerSlice ) * radius;

        Vec3  bl = Vec3( centerXY.x, centerXY.y, 0.f ) + Vec3( cosDegrees, sinDegrees, minMaxz.m_min );
        Vec3  br = Vec3( centerXY.x, centerXY.y, 0.f ) + Vec3( nextCosDegrees, nextSinDegrees, minMaxz.m_min );

        Vec3  tr = Vec3( centerXY.x, centerXY.y, 0.f ) + Vec3( nextCosDegrees, nextSinDegrees, minMaxz.m_max );
        Vec3  tl = Vec3( centerXY.x, centerXY.y, 0.f ) + Vec3( cosDegrees, sinDegrees, minMaxz.m_max );

        verts.emplace_back( bl, color, Vec2::ZERO );
        verts.emplace_back( br, color, Vec2::ZERO );
        verts.emplace_back( tr, color, Vec2::ZERO );
        verts.emplace_back( tl, color, Vec2::ZERO );
    }

    for ( int i = 0; i < static_cast< int >( numSlices ); ++i )
    {
        unsigned int bl = startIndex + i * 4 + 0;
        unsigned int br = startIndex + i * 4 + 1;
        unsigned int tr = startIndex + i * 4 + 2;
        unsigned int tl = startIndex + i * 4 + 3;

        indexes.push_back( bl );
        indexes.push_back( br );
        indexes.push_back( tl );

        indexes.push_back( br );
        indexes.push_back( tr );
        indexes.push_back( tl );
    }

    // bottom cap
    startIndex = static_cast< unsigned int >( verts.size() );
    for ( int i = 0; i <= static_cast< int >( numStacks ); ++i )
    {
        for ( int j = 0; j <= static_cast< int >( numSlices ); ++j )
        {
            float yaw      = degreesPerSlice * static_cast< float >( j );
            float pitch    = 90.f + degreesPerStack * static_cast< float >( i );
            Vec3  position = boneStart + Vec3::MakeFromPolarDegrees( yaw, pitch, radius );
            verts.emplace_back( position, color, Vec2::ZERO );
        }
    }

    int stride = static_cast< int >( numSlices ) + 1;
    for ( int i = 0; i < static_cast< int >( numStacks ); ++i )
    {
        for ( int j = 0; j < static_cast< int >( numSlices ); ++j )
        {
            unsigned int bl = startIndex + i * stride + j;
            unsigned int br = bl + 1;
            unsigned int tl = bl + stride;
            unsigned int tr = bl + stride + 1;

            indexes.push_back( bl );
            indexes.push_back( br );
            indexes.push_back( tr );

            indexes.push_back( bl );
            indexes.push_back( tr );
            indexes.push_back( tl );
        }
    }

    // top cap
    startIndex = static_cast< unsigned int >( verts.size() );
    for ( int i = 0; i <= static_cast< int >( numStacks ); ++i )
    {
        for ( int j = 0; j <= static_cast< int >( numSlices ); ++j )
        {
            float yaw      = degreesPerSlice * static_cast< float >( j );
            float pitch    = degreesPerStack * static_cast< float >( i );
            Vec3  position = boneEnd + Vec3::MakeFromPolarDegrees( yaw, -pitch, radius );
            verts.emplace_back( position, color, Vec2::ZERO );
        }
    }

    stride = static_cast< int >( numSlices ) + 1;
    for ( int i = 0; i < static_cast< int >( numStacks ); ++i )
    {
        for ( int j = 0; j < static_cast< int >( numSlices ); ++j )
        {
            unsigned int bl = startIndex + i * stride + j;
            unsigned int br = bl + 1;
            unsigned int tl = bl + stride;
            unsigned int tr = bl + stride + 1;

            indexes.push_back( bl );
            indexes.push_back( br );
            indexes.push_back( tr );

            indexes.push_back( bl );
            indexes.push_back( tr );
            indexes.push_back( tl );
        }
    }
}

//-----------------------------------------------------------------------------------------------
void AddVertsForLineSegment2D( std::vector< Vertex >& verts, Vec2 start, Vec2 end, Vec2 thickness, Rgba8 color )
{
    Vec2 halfThickness = thickness * 0.5f;

    Vec2 direction  = ( end - start ).GetNormalized();
    Vec2 leftVector = direction.GetRotatedBy90Degrees();

    Vec2 topLeft    = start + ( leftVector * halfThickness.x );
    Vec2 bottomLeft = start - ( leftVector * halfThickness.x );

    Vec2 topRight    = end + ( leftVector * halfThickness.y );
    Vec2 bottomRight = end - ( leftVector * halfThickness.y );

    verts.push_back( Vertex{ Vec3( topRight.x, topRight.y, 0.f ), color, Vec2() } );
    verts.push_back( Vertex{ Vec3( topLeft.x, topLeft.y, 0.f ), color, Vec2() } );
    verts.push_back( Vertex{ Vec3( bottomLeft.x, bottomLeft.y, 0.f ), color, Vec2() } );

    verts.push_back( Vertex{ Vec3( topRight.x, topRight.y, 0.f ), color, Vec2() } );
    verts.push_back( Vertex{ Vec3( bottomLeft.x, bottomLeft.y, 0.f ), color, Vec2() } );
    verts.push_back( Vertex{ Vec3( bottomRight.x, bottomRight.y, 0.f ), color, Vec2() } );
}

//-----------------------------------------------------------------------------------------------
void AddVertsForLineSegment3D( std::vector< Vertex >& verts, Vec3 const& start, Vec3 const& end, float thickness, Rgba8 const& color )
{
    float halfThickness = thickness * 0.5f;
    Vec3  bottomLeft, bottomRight, topLeft, topRight;
    Vec3  direction       = ( end - start ).GetNormalized();
    Vec3  referenceVector = Vec3::WORLD_UP;

    if ( std::abs( direction.z ) > 0.999f )
    {
        referenceVector = Vec3::WORLD_FORWARD;
    }

    Vec3 leftVector = CrossProduct3D( referenceVector, direction ).GetNormalized();

    bottomLeft  = start + leftVector * halfThickness;
    bottomRight = start - leftVector * halfThickness;

    topLeft  = end + ( leftVector * halfThickness );
    topRight = end - ( leftVector * halfThickness );

    verts.push_back( Vertex{ bottomLeft, color, Vec2::ZERO } );
    verts.push_back( Vertex{ bottomRight, color, Vec2::ZERO } );
    verts.push_back( Vertex{ topRight, color, Vec2::ZERO } );

    verts.push_back( Vertex{ bottomLeft, color, Vec2::ZERO } );
    verts.push_back( Vertex{ topRight, color, Vec2::ZERO } );
    verts.push_back( Vertex{ topLeft, color, Vec2::ZERO } );

    verts.push_back( Vertex{ bottomRight, color, Vec2::ZERO } );
    verts.push_back( Vertex{ bottomLeft, color, Vec2::ZERO } );
    verts.push_back( Vertex{ topLeft, color, Vec2::ZERO } );

    verts.push_back( Vertex{ bottomRight, color, Vec2::ZERO } );
    verts.push_back( Vertex{ topLeft, color, Vec2::ZERO } );
    verts.push_back( Vertex{ topRight, color, Vec2::ZERO } );

    Vec3 upVector = CrossProduct3D( direction, leftVector );

    bottomLeft  = start + upVector * halfThickness;
    bottomRight = start - upVector * halfThickness;

    topLeft  = end + ( upVector * halfThickness );
    topRight = end - ( upVector * halfThickness );

    verts.push_back( Vertex{ bottomLeft, color, Vec2::ZERO } );
    verts.push_back( Vertex{ bottomRight, color, Vec2::ZERO } );
    verts.push_back( Vertex{ topRight, color, Vec2::ZERO } );

    verts.push_back( Vertex{ bottomLeft, color, Vec2::ZERO } );
    verts.push_back( Vertex{ topRight, color, Vec2::ZERO } );
    verts.push_back( Vertex{ topLeft, color, Vec2::ZERO } );

    verts.push_back( Vertex{ bottomRight, color, Vec2::ZERO } );
    verts.push_back( Vertex{ bottomLeft, color, Vec2::ZERO } );
    verts.push_back( Vertex{ topLeft, color, Vec2::ZERO } );

    verts.push_back( Vertex{ bottomRight, color, Vec2::ZERO } );
    verts.push_back( Vertex{ topLeft, color, Vec2::ZERO } );
    verts.push_back( Vertex{ topRight, color, Vec2::ZERO } );
}

//-----------------------------------------------------------------------------------------------
void AddVertsForQuadWireFrame( std::vector< Vertex >& verts, float lineThickness, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color /*= Rgba8::WHITE */ )
{
    AddVertsForLineSegment3D( verts, bottomLeft, bottomRight, lineThickness, color );
    AddVertsForLineSegment3D( verts, bottomLeft, topLeft, lineThickness, color );

    AddVertsForLineSegment3D( verts, topRight, bottomRight, lineThickness, color );
    AddVertsForLineSegment3D( verts, topRight, topLeft, lineThickness, color );
}

//-----------------------------------------------------------------------------------------------
void AddVertsAtOriginXForward( std::vector< Vertex >& verts, AABB2 bounds, Vec2 const& pivot )
{
    Mat44 transformMatrix;

    Mat44 orientationMatrix;
    Vec3  iBasis = Vec3( 0.0f, 1.0f, 0.0f );
    Vec3  jBasis = Vec3( 0.0f, 0.0f, 1.0f );
    Vec3  kBasis = Vec3( 1.0f, 0.0f, 0.0f );
    orientationMatrix.SetIJK3D( iBasis, jBasis, kBasis );

    transformMatrix.Append( orientationMatrix );
    transformMatrix.AppendTranslation3D( Vec3( -pivot.x * bounds.m_maxs.x, -pivot.y * bounds.m_maxs.y, 0.f ) );

    TransformVertexArray3D( verts, transformMatrix );
}

//-----------------------------------------------------------------------------------------------
void AddVertsForAABBWireFrame3D( std::vector< Vertex >& verts, AABB3 const& bounds, float lineThickness, Rgba8 const& tint /*= Rgba8::WHITE */ )
{
    Vec3 posA = Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z );
    Vec3 posB = Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z );
    Vec3 posC = Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z );
    Vec3 posD = Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z );
    Vec3 posE = Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z );
    Vec3 posF = Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z );
    Vec3 posG = Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z );
    Vec3 posH = Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z );

    AddVertsForQuadWireFrame( verts, lineThickness, posA, posB, posC, posD, tint );  // +X
    AddVertsForQuadWireFrame( verts, lineThickness, posF, posE, posH, posG, tint );  // -X
    AddVertsForQuadWireFrame( verts, lineThickness, posB, posF, posG, posC, tint );  // +Y
    AddVertsForQuadWireFrame( verts, lineThickness, posE, posA, posD, posH, tint );  // -Y
    AddVertsForQuadWireFrame( verts, lineThickness, posD, posC, posG, posH, tint );  // +Z
    AddVertsForQuadWireFrame( verts, lineThickness, posE, posF, posB, posA, tint );  // -Z
}

//-----------------------------------------------------------------------------------------------
void AddVertsForPlaneWireFrame3D( std::vector< Vertex >& verts, Plane3 const& plane, int size, float lineThickness )
{
    Vec3 forward = plane.m_normal;
    Vec3 left;
    Vec3 up;
    if ( fabsf( DotProduct3D( forward, Vec3::WORLD_UP ) ) > 0.99f )
    {
        up   = CrossProduct3D( Vec3::WORLD_LEFT, forward );
        left = CrossProduct3D( up, forward );
    }
    else
    {
        left = CrossProduct3D( Vec3::WORLD_UP, forward );
        up   = CrossProduct3D( left, forward );
    }

    float halfSize = static_cast< float >( size ) * 0.5f;
    left           = left.GetNormalized();
    up             = up.GetNormalized();

    Vec3 bottomLeft  = left * halfSize + up * -halfSize;
    Vec3 bottomRight = left * -halfSize + up * -halfSize;
    Vec3 topLeft     = left * -halfSize + up * halfSize;
    Vec3 topRight    = left * halfSize + up * halfSize;

    Vec3 bl, br, tl, tr;
    Vec3 startPos = bottomLeft;
    for ( int i = 0; i < size; i++ )
    {
        for ( int j = 0; j < size; j++ )
        {
            bl = startPos + static_cast< float >( j ) * -left + static_cast< float >( i ) * up;
            br = startPos + static_cast< float >( j + 1 ) * -left + static_cast< float >( i ) * up;
            tl = bl + up;
            tr = br + up;
            AddVertsForQuadWireFrame( verts, lineThickness, bl, br, tr, tl );
        }
    }
}

//-----------------------------------------------------------------------------------------------
void AddVertsForTriangle2D( std::vector< Vertex >& verts, Vec2 ccw0, Vec2 ccw1, Vec2 ccw2, Rgba8 color )
{
    verts.push_back( Vertex{ Vec3( ccw0.x, ccw0.y, 0.f ), color, Vec2() } );
    verts.push_back( Vertex{ Vec3( ccw1.x, ccw1.y, 0.f ), color, Vec2() } );
    verts.push_back( Vertex{ Vec3( ccw2.x, ccw2.y, 0.f ), color, Vec2() } );
}

//-----------------------------------------------------------------------------------------------
void AddVertsForQuad2D( std::vector< Vertex >& verts, Vec2 ccw0, Vec2 ccw1, Vec2 ccw2, Vec2 ccw3, Rgba8 tint, AABB2 const& UVs /*= AABB2::ZERO_TO_ONE */ )
{
    Vec2 uvTopLeft     = Vec2( UVs.m_mins.x, UVs.m_maxs.y );
    Vec2 uvTopRight    = Vec2( UVs.m_maxs.x, UVs.m_maxs.y );
    Vec2 uvBottomLeft  = Vec2( UVs.m_mins.x, UVs.m_mins.y );
    Vec2 uvBottomRight = Vec2( UVs.m_maxs.x, UVs.m_mins.y );

    verts.emplace_back( Vertex{ Vec3( ccw0.x, ccw0.y, 0.f ), tint, uvBottomLeft } );
    verts.emplace_back( Vertex{ Vec3( ccw1.x, ccw1.y, 0.f ), tint, uvBottomRight } );
    verts.emplace_back( Vertex{ Vec3( ccw2.x, ccw2.y, 0.f ), tint, uvTopRight } );

    verts.emplace_back( Vertex{ Vec3( ccw0.x, ccw0.y, 0.f ), tint, uvBottomLeft } );
    verts.emplace_back( Vertex{ Vec3( ccw2.x, ccw2.y, 0.f ), tint, uvTopRight } );
    verts.emplace_back( Vertex{ Vec3( ccw3.x, ccw3.y, 0.f ), tint, uvTopLeft } );
}

//-----------------------------------------------------------------------------------------------
void AddVertsForQuad2D( std::vector< Vertex >& verts, AABB2 const& bounds, Rgba8 const& tint /*= Rgba8::WHITE*/, AABB2 const& UVs /*= AABB2::ZERO_TO_ONE */ )
{
    Vec2 uvTopLeft     = Vec2( UVs.m_mins.x, UVs.m_maxs.y );
    Vec2 uvTopRight    = Vec2( UVs.m_maxs.x, UVs.m_maxs.y );
    Vec2 uvBottomLeft  = Vec2( UVs.m_mins.x, UVs.m_mins.y );
    Vec2 uvBottomRight = Vec2( UVs.m_maxs.x, UVs.m_mins.y );

    Vec2 bottomLeft  = Vec2( bounds.m_mins.x, bounds.m_mins.y );
    Vec2 bottomRight = Vec2( bounds.m_maxs.x, bounds.m_mins.y );
    Vec2 topRight    = Vec2( bounds.m_maxs.x, bounds.m_maxs.y );
    Vec2 topLeft     = Vec2( bounds.m_mins.x, bounds.m_maxs.y );

    verts.emplace_back( Vertex{ Vec3( bottomLeft.x, bottomLeft.y, 0.f ), tint, uvBottomLeft } );
    verts.emplace_back( Vertex{ Vec3( bottomRight.x, bottomRight.y, 0.f ), tint, uvBottomRight } );
    verts.emplace_back( Vertex{ Vec3( topRight.x, topRight.y, 0.f ), tint, uvTopRight } );

    verts.emplace_back( Vertex{ Vec3( bottomLeft.x, bottomLeft.y, 0.f ), tint, uvBottomLeft } );
    verts.emplace_back( Vertex{ Vec3( topRight.x, topRight.y, 0.f ), tint, uvTopRight } );
    verts.emplace_back( Vertex{ Vec3( topLeft.x, topLeft.y, 0.f ), tint, uvTopLeft } );
}

//-----------------------------------------------------------------------------------------------
void AddVertsForOBB3D( std::vector< Vertex >& verts, OBB3 const& orientedBox, Rgba8 const& tint /*= Rgba8::WHITE*/, [[maybe_unused]] AABB2 const& UVs /*= AABB2::ZERO_TO_ONE */ )
{
    Vec3  iBasis = orientedBox.m_iBasisNormal;
    Vec3  jBasis = orientedBox.m_jBasisNormal;
    Vec3  kBasis = orientedBox.m_kBasisNormal;

    float halfX = orientedBox.m_halfDimensions.x;
    float halfY = orientedBox.m_halfDimensions.y;
    float halfZ = orientedBox.m_halfDimensions.z;

    Vec3  TBL = ( iBasis * -halfX ) + ( kBasis * halfZ ) + ( jBasis * halfY );
    Vec3  TBR = ( iBasis * -halfX ) + ( kBasis * halfZ ) + ( jBasis * -halfY );
    Vec3  TTR = ( iBasis * halfX ) + ( kBasis * halfZ ) + ( jBasis * -halfY );
    Vec3  TTL = ( iBasis * halfX ) + ( kBasis * halfZ ) + ( jBasis * halfY );

    Vec3  BBL = ( iBasis * -halfX ) + ( kBasis * -halfZ ) + ( jBasis * halfY );
    Vec3  BBR = ( iBasis * -halfX ) + ( kBasis * -halfZ ) + ( jBasis * -halfY );
    Vec3  BTR = ( iBasis * halfX ) + ( kBasis * -halfZ ) + ( jBasis * -halfY );
    Vec3  BTL = ( iBasis * halfX ) + ( kBasis * -halfZ ) + ( jBasis * +halfY );

    AddVertsForQuad3D( verts, TBL, TBR, TTR, TTL, tint );
    AddVertsForQuad3D( verts, BTL, BTR, BBR, BBL, tint );
    AddVertsForQuad3D( verts, BTL, BBL, TBL, TTL, tint );
    AddVertsForQuad3D( verts, BBR, BTR, TTR, TBR, tint );
    AddVertsForQuad3D( verts, BBL, BBR, TBR, TBL, tint );
    AddVertsForQuad3D( verts, BTR, BTL, TTL, TTR, tint );
}

//-----------------------------------------------------------------------------------------------
void AddVertsForOBB3D( std::vector< Vertex >& verts, std::vector< unsigned int >& indexes, OBB3 const& orientedBox, Rgba8 const& tint /*= Rgba8::WHITE*/, [[maybe_unused]] AABB2 const& UVs /*= AABB2::ZERO_TO_ONE */ )
{
    Vec3  iBasis = orientedBox.m_iBasisNormal;
    Vec3  jBasis = orientedBox.m_jBasisNormal;
    Vec3  kBasis = orientedBox.m_kBasisNormal;

    float halfX = orientedBox.m_halfDimensions.x;
    float halfY = orientedBox.m_halfDimensions.y;
    float halfZ = orientedBox.m_halfDimensions.z;

    Vec3  center = orientedBox.m_center;

    Vec3  TBL = center + ( iBasis * -halfX ) + ( kBasis * halfZ ) + ( jBasis * halfY );
    Vec3  TBR = center + ( iBasis * -halfX ) + ( kBasis * halfZ ) + ( jBasis * -halfY );
    Vec3  TTR = center + ( iBasis * halfX ) + ( kBasis * halfZ ) + ( jBasis * -halfY );
    Vec3  TTL = center + ( iBasis * halfX ) + ( kBasis * halfZ ) + ( jBasis * halfY );

    Vec3  BBL = center + ( iBasis * -halfX ) + ( kBasis * -halfZ ) + ( jBasis * halfY );
    Vec3  BBR = center + ( iBasis * -halfX ) + ( kBasis * -halfZ ) + ( jBasis * -halfY );
    Vec3  BTR = center + ( iBasis * halfX ) + ( kBasis * -halfZ ) + ( jBasis * -halfY );
    Vec3  BTL = center + ( iBasis * halfX ) + ( kBasis * -halfZ ) + ( jBasis * halfY );

    AddVertsForQuad3D( verts, indexes, TBR, TTR, TTL, TBL, tint );
    AddVertsForQuad3D( verts, indexes, BBL, BTL, BTR, BBR, tint );

    AddVertsForQuad3D( verts, indexes, BTL, BBL, TBL, TTL, tint );  // +y
    AddVertsForQuad3D( verts, indexes, BBR, BTR, TTR, TBR, tint );

    AddVertsForQuad3D( verts, indexes, BBL, BBR, TBR, TBL, tint );
    AddVertsForQuad3D( verts, indexes, BTR, BTL, TTL, TTR, tint );
}

//-----------------------------------------------------------------------------------------------
void AddVertsForOBBWireFrame3D( std::vector< Vertex >& verts, OBB3 const& orientedBox, float lineThickness, Rgba8 const& tint /*= Rgba8::WHITE */ )
{
    Vec3  iBasis = orientedBox.m_iBasisNormal;
    Vec3  jBasis = orientedBox.m_jBasisNormal;
    Vec3  kBasis = orientedBox.m_kBasisNormal;

    float halfX = orientedBox.m_halfDimensions.x;
    float halfY = orientedBox.m_halfDimensions.y;
    float halfZ = orientedBox.m_halfDimensions.z;

    Vec3  TBL = ( iBasis * -halfX ) + ( kBasis * halfZ ) + ( jBasis * halfY );
    Vec3  TBR = ( iBasis * -halfX ) + ( kBasis * halfZ ) + ( jBasis * -halfY );
    Vec3  TTR = ( iBasis * halfX ) + ( kBasis * halfZ ) + ( jBasis * -halfY );
    Vec3  TTL = ( iBasis * halfX ) + ( kBasis * halfZ ) + ( jBasis * halfY );

    Vec3  BBL = ( iBasis * -halfX ) + ( kBasis * -halfZ ) + ( jBasis * halfY );
    Vec3  BBR = ( iBasis * -halfX ) + ( kBasis * -halfZ ) + ( jBasis * -halfY );
    Vec3  BTR = ( iBasis * halfX ) + ( kBasis * -halfZ ) + ( jBasis * -halfY );
    Vec3  BTL = ( iBasis * halfX ) + ( kBasis * -halfZ ) + ( jBasis * +halfY );

    AddVertsForQuadWireFrame( verts, lineThickness, TBL, TBR, TTR, TTL, tint );
    AddVertsForQuadWireFrame( verts, lineThickness, BTL, BTR, BBR, BBL, tint );

    AddVertsForQuadWireFrame( verts, lineThickness, BTL, BBL, TBL, TTL, tint );
    AddVertsForQuadWireFrame( verts, lineThickness, BBR, BTR, TTR, TBR, tint );

    AddVertsForQuadWireFrame( verts, lineThickness, BBL, BBR, TBR, TBL, tint );
    AddVertsForQuadWireFrame( verts, lineThickness, BTR, BTL, TTL, TTR, tint );
}

//-----------------------------------------------------------------------------------------------
void AddVertsForTriangle3D( std::vector< Vertex >& verts, Vec3 ccw0, Vec3 ccw1, Vec3 ccw2, Rgba8 color, Vec2 uv0, Vec2 uv1, Vec2 uvPole )
{
    verts.emplace_back( Vertex{ ccw0, color, uv0 } );
    verts.emplace_back( Vertex{ ccw1, color, uv1 } );
    verts.emplace_back( Vertex{ ccw2, color, uvPole } );
}

//-----------------------------------------------------------------------------------------------
void AddVertsForTriangle3D( std::vector< Vertex >& verts, Vec3 ccw0, Vec3 ccw1, Vec3 ccw2, Rgba8 color )
{
    verts.emplace_back( Vertex{ ccw0, color, Vec2::ZERO } );
    verts.emplace_back( Vertex{ ccw1, color, Vec2::ZERO } );
    verts.emplace_back( Vertex{ ccw2, color, Vec2::ZERO } );
}

//-----------------------------------------------------------------------------------------------
void AddVertsForTriangle3D( std::vector< Vertex >& verts, Vec3 ccw0, Vec3 ccw1, Vec3 ccw2, Rgba8 color, Vec2 uv0, Vec2 uv1, Vec2 uv2, Vec3 normal )
{
    Vec3 tangent( 0, 0, 0 ), bitangent( 0, 0, 0 );
    verts.emplace_back( Vertex{ ccw0, color, uv0, tangent, bitangent, normal } );
    verts.emplace_back( Vertex{ ccw1, color, uv1, tangent, bitangent, normal } );
    verts.emplace_back( Vertex{ ccw2, color, uv2, tangent, bitangent, normal } );
}

//-----------------------------------------------------------------------------------------------
void AddVertsForArrow2D( std::vector< Vertex >& verts, Vec2 const& start, Vec2 const& end, float thickness, float arrowRadius, Rgba8 const& color )
{
    Vec2 fwd = ( end - start ).GetNormalized();
    Vec2 left( -fwd.y, fwd.x );
    Vec2 leftWingDisp     = ( left - fwd ).GetNormalized() * arrowRadius;
    Vec2 rightWingDisp    = ( -left - fwd ).GetNormalized() * arrowRadius;
    Vec2 leftWingTipPos   = end + leftWingDisp;
    Vec2 rightWingTipsPos = end + rightWingDisp;

    AddVertsForLineSegment2D( verts, start, end, Vec2( thickness, thickness ), color );
    AddVertsForLineSegment2D( verts, leftWingTipPos, end, Vec2( thickness, thickness ), color );
    AddVertsForLineSegment2D( verts, rightWingTipsPos, end, Vec2( thickness, thickness ), color );
}

//-----------------------------------------------------------------------------------------------
void TransTextVertexArrayXY3D( std::vector< Vertex >& verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY )
{
    TransformVertexArrayXY3D( static_cast< int >( verts.size() ), verts.data(), uniformScaleXY, rotationDegreesAboutZ, translationXY );
    g_engine->m_render->DrawVertexArray( verts );
}

//-----------------------------------------------------------------------------------------------
void TransformVertexArray3D( std::vector< Vertex >& verts, Mat44 const& transform )
{
    for ( int vertIndex = 0; vertIndex < static_cast< int >( verts.size() ); ++vertIndex )
    {
        verts[ vertIndex ].m_position = transform.TransformPosition3D( verts[ vertIndex ].m_position );
    }
}

//-----------------------------------------------------------------------------------------------
void AddVertsForQuad3D( std::vector< Vertex >& verts, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color /*= Rgba8::WHITE*/, AABB2 const& UVs /*= AABB2::ZERO_TO_ONE */ )
{
    Vec2 uvBottomLeft  = Vec2( UVs.m_mins.x, UVs.m_mins.y );
    Vec2 uvBottomRight = Vec2( UVs.m_maxs.x, UVs.m_mins.y );
    Vec2 uvTopRight    = Vec2( UVs.m_maxs.x, UVs.m_maxs.y );
    Vec2 uvTopLeft     = Vec2( UVs.m_mins.x, UVs.m_maxs.y );

    Vec3 u      = ( bottomRight - bottomLeft ).GetNormalized();
    Vec3 v      = ( topLeft - bottomLeft ).GetNormalized();
    Vec3 normal = CrossProduct3D( u, v ).GetNormalized();

    Vec3 tangent   = u;
    Vec3 bitangent = v;

    // tri1: BL, BR, TR
    verts.emplace_back( bottomLeft, color, uvBottomLeft, tangent, bitangent, normal );
    verts.emplace_back( bottomRight, color, uvBottomRight, tangent, bitangent, normal );
    verts.emplace_back( topRight, color, uvTopRight, tangent, bitangent, normal );

    // tri2: BL, TR, TL
    verts.emplace_back( bottomLeft, color, uvBottomLeft, tangent, bitangent, normal );
    verts.emplace_back( topRight, color, uvTopRight, tangent, bitangent, normal );
    verts.emplace_back( topLeft, color, uvTopLeft, tangent, bitangent, normal );
}

//-----------------------------------------------------------------------------------------------
void AddVertsForQuad3D( std::vector< Vertex >& vertexes, std::vector< unsigned int >& indexes, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color /*= Rgba8::WHITE*/, AABB2 const& UVs /*= AABB2::ZERO_TO_ONE */ )
{
    Vec2         uvBottomLeft  = Vec2( UVs.m_mins.x, UVs.m_mins.y );
    Vec2         uvBottomRight = Vec2( UVs.m_maxs.x, UVs.m_mins.y );
    Vec2         uvTopRight    = Vec2( UVs.m_maxs.x, UVs.m_maxs.y );
    Vec2         uvTopLeft     = Vec2( UVs.m_mins.x, UVs.m_maxs.y );

    unsigned int startIndex = static_cast< unsigned int >( vertexes.size() );
    Vec3         tangent    = ( bottomRight - bottomLeft ).GetNormalized();
    Vec3         bitangent  = ( topLeft - bottomLeft ).GetNormalized();

    Vec3         u      = ( bottomRight - bottomLeft ).GetNormalized();
    Vec3         v      = ( topLeft - bottomLeft ).GetNormalized();
    Vec3         normal = CrossProduct3D( u, v ).GetNormalized();

    vertexes.emplace_back( bottomLeft, color, uvBottomLeft, tangent, bitangent, normal );
    vertexes.emplace_back( bottomRight, color, uvBottomRight, tangent, bitangent, normal );
    vertexes.emplace_back( topRight, color, uvTopRight, tangent, bitangent, normal );
    vertexes.emplace_back( topLeft, color, uvTopLeft, tangent, bitangent, normal );

    indexes.push_back( startIndex + 0 );
    indexes.push_back( startIndex + 1 );
    indexes.push_back( startIndex + 3 );

    indexes.push_back( startIndex + 1 );
    indexes.push_back( startIndex + 2 );
    indexes.push_back( startIndex + 3 );
}

//-----------------------------------------------------------------------------------------------
void AddVertsForUVSphereZ3D( std::vector< Vertex >& verts, Vec3 const& center, float radius, float numSlices, float numStacks, Rgba8 const& tint /*= Rgba8::WHITE*/, AABB2 const& UVs /*= AABB2::ZERO_TO_ONE */ )
{
    float degreesPerStack = 180.f / numStacks;
    float degreesPerSlice = 360.f / numSlices;

    Vec3  startPos = center + ( Vec3::MakeFromPolarDegrees( 0.f, 90.f ) * radius );
    Vec3  endPos   = center + ( Vec3::MakeFromPolarDegrees( 0.f, -90.f ) * radius );

    Vec3  tangent( 0, 0, 0 );
    Vec3  bitangent( 0, 0, 0 );

    Vec3  bl, br, tr, tl;
    for ( int i = 0; i < numStacks; ++i )
    {
        for ( int j = 0; j < numSlices; ++j )
        {
            bl = center + Vec3::MakeFromPolarDegrees( degreesPerSlice * j, 90.f - degreesPerStack * i, radius );
            br = center + Vec3::MakeFromPolarDegrees( degreesPerSlice * ( j + 1.f ), 90.f - degreesPerStack * i, radius );
            tl = center + Vec3::MakeFromPolarDegrees( degreesPerSlice * j, 90.f - degreesPerStack * ( i + 1 ), radius );
            tr = center + Vec3::MakeFromPolarDegrees( degreesPerSlice * ( j + 1.f ), 90.f - degreesPerStack * ( i + 1 ), radius );

            float u0 = static_cast< float >( j ) / numSlices;
            float u1 = ( static_cast< float >( j ) + 1 ) / numSlices;
            float v0 = static_cast< float >( i ) / numStacks;
            float v1 = static_cast< float >( i + 1 ) / numStacks;

            u0 = RangeMap( u0, 0.f, 1.f, UVs.m_mins.x, UVs.m_maxs.x );
            u1 = RangeMap( u1, 0.f, 1.f, UVs.m_mins.x, UVs.m_maxs.x );
            v0 = RangeMap( v0, 0.f, 1.f, UVs.m_mins.y, UVs.m_maxs.y );
            v1 = RangeMap( v1, 0.f, 1.f, UVs.m_mins.y, UVs.m_maxs.y );

            Vec3 normalBL    = ( bl - center ).GetNormalized();
            Vec3 normalBR    = ( br - center ).GetNormalized();
            Vec3 normalTL    = ( tl - center ).GetNormalized();
            Vec3 normalTR    = ( tr - center ).GetNormalized();
            Vec3 normalStart = ( startPos - center ).GetNormalized();
            Vec3 normalEnd   = ( endPos - center ).GetNormalized();

            if ( i == 0 )
            {
                verts.emplace_back( startPos, tint, Vec2( ( u0 + u1 ) * 0.5f, v0 ), tangent, bitangent, normalStart );
                verts.emplace_back( tr, tint, Vec2( u1, v1 ), tangent, bitangent, normalTR );
                verts.emplace_back( tl, tint, Vec2( u0, v1 ), tangent, bitangent, normalTL );
                continue;
            }

            if ( i == numStacks - 1 )
            {
                verts.emplace_back( endPos, tint, Vec2( ( u0 + u1 ) * 0.5f, v1 ), tangent, bitangent, normalEnd );
                verts.emplace_back( bl, tint, Vec2( u0, v0 ), tangent, bitangent, normalBL );
                verts.emplace_back( br, tint, Vec2( u1, v0 ), tangent, bitangent, normalBR );
                continue;
            }

            verts.emplace_back( bl, tint, Vec2( u0, v0 ), tangent, bitangent, normalBL );
            verts.emplace_back( br, tint, Vec2( u1, v0 ), tangent, bitangent, normalBR );
            verts.emplace_back( tr, tint, Vec2( u1, v1 ), tangent, bitangent, normalTR );

            verts.emplace_back( bl, tint, Vec2( u0, v0 ), tangent, bitangent, normalBL );
            verts.emplace_back( tr, tint, Vec2( u1, v1 ), tangent, bitangent, normalTR );
            verts.emplace_back( tl, tint, Vec2( u0, v1 ), tangent, bitangent, normalTL );
        }
    }
}

void AddVertsForUVSphereZ3D( std::vector< Vertex >& verts, std::vector< unsigned int >& indexes, Vec3 const& center, float radius, float numSlices, float numStacks, Rgba8 const& tint /*= Rgba8::WHITE*/, AABB2 const& UVs /*= AABB2::ZERO_TO_ONE */ )
{
    unsigned int startIndex      = static_cast< unsigned int >( verts.size() );
    float        degreesPerStack = 180.f / numStacks;
    float        degreesPerSlice = 360.f / numSlices;

    for ( int i = 0; i <= static_cast< int >( numStacks ); ++i )
    {
        for ( int j = 0; j <= static_cast< int >( numSlices ); ++j )
        {
            float yawDeg   = degreesPerSlice * j;
            float pitchDeg = 90.f - degreesPerStack * i;

            Vec3  pos    = center + Vec3::MakeFromPolarDegrees( yawDeg, pitchDeg, radius );
            Vec3  normal = ( pos - center ).GetNormalized();

            Vec3  tangent   = Vec3( -SinDegrees( yawDeg ), CosDegrees( yawDeg ), 0.f );
            Vec3  bitangent = CrossProduct3D( normal, tangent );

            float u = RangeMap( static_cast< float >( j ) / numSlices, 0.f, 1.f, UVs.m_mins.x, UVs.m_maxs.x );
            float v = RangeMap( static_cast< float >( i ) / numStacks, 0.f, 1.f, UVs.m_mins.y, UVs.m_maxs.y );

            verts.emplace_back( pos, tint, Vec2( u, v ), tangent, bitangent, normal );
        }
    }

    int stride = static_cast< int >( numSlices ) + 1;
    for ( int i = 0; i < static_cast< int >( numStacks ); ++i )
    {
        for ( int j = 0; j < static_cast< int >( numSlices ); ++j )
        {
            unsigned int bl = startIndex + i * stride + j;
            unsigned int br = bl + 1;
            unsigned int tl = bl + stride;
            unsigned int tr = bl + stride + 1;
            indexes.push_back( bl );
            indexes.push_back( br );
            indexes.push_back( tr );
            indexes.push_back( bl );
            indexes.push_back( tr );
            indexes.push_back( tl );
        }
    }
}

//-----------------------------------------------------------------------------------------------
void AddVertsForUVSphereZWireFrame3D( std::vector< Vertex >& verts, Vec3 const& center, float radius, float numSlices /*= 32.f*/, float numStacks /*= 16.f*/, float lineThickness /*= 0.01f*/, Rgba8 const& tint /*= Rgba8::WHITE */ )
{
    float degreesPerStack = 180.f / numStacks;
    float degreesPerSlice = 360.f / numSlices;

    Vec3  startPos = center + ( Vec3::MakeFromPolarDegrees( 0.f, 90.f ) * radius );
    Vec3  endPos   = center + ( Vec3::MakeFromPolarDegrees( 0.f, -90.f ) * radius );

    Vec3  bl, br, tr, tl;
    for ( int i = 0; i < numStacks; ++i )
    {
        for ( int j = 0; j < numSlices; ++j )
        {
            bl = center + Vec3::MakeFromPolarDegrees( degreesPerSlice * j, 90.f - degreesPerStack * i, radius );
            br = center + Vec3::MakeFromPolarDegrees( degreesPerSlice * ( j + 1.f ), 90.f - degreesPerStack * i, radius );
            tl = center + Vec3::MakeFromPolarDegrees( degreesPerSlice * j, 90.f - degreesPerStack * ( i + 1 ), radius );
            tr = center + Vec3::MakeFromPolarDegrees( degreesPerSlice * ( j + 1.f ), 90.f - degreesPerStack * ( i + 1 ), radius );

            if ( i == 0 )
            {
                AddVertsForLineSegment3D( verts, startPos, tl, lineThickness, tint );
                AddVertsForLineSegment3D( verts, startPos, tr, lineThickness, tint );
                continue;
            }

            if ( i == numStacks - 1 )
            {
                AddVertsForLineSegment3D( verts, endPos, bl, lineThickness, tint );
                AddVertsForLineSegment3D( verts, endPos, br, lineThickness, tint );
                continue;
            }

            AddVertsForQuadWireFrame( verts, lineThickness, bl, br, tr, tl, tint );
        }
    }
}

//-----------------------------------------------------------------------------------------------
void AddVertsForArrow3D( std::vector< Vertex >& verts, Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color /*= Rgba8::WHITE*/, int numSlices /*= 32 */ )
{
    Vec3  direction   = ( start - end ).GetNormalized();
    float arrowHeight = GetDistance3D( start, end );
    float coneHeight  = radius * 2.f;
    if ( coneHeight > arrowHeight * 0.5f )
    {
        coneHeight = arrowHeight * 0.5f;
    }
    Vec3  coneStart  = end + direction * coneHeight;
    float coneRadius = radius * 2.f;

    AddVertsForCone3D( verts, coneStart, end, coneRadius, color, AABB2::ZERO_TO_ONE, numSlices );
    AddVertsForCylinder3D( verts, start, coneStart, radius, color, AABB2::ZERO_TO_ONE, numSlices );
}

//-----------------------------------------------------------------------------------------------
void AddVertsForCylinder3D( std::vector< Vertex >& verts, Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color /*= Rgba8::WHITE*/, AABB2 const& UVs /*= AABB2::ZERO_TO_ONE*/, int numSlices /*= 32 */ )
{
    float       degreesPerSlice = 360.f / static_cast< float >( numSlices );
    Mat44       lookAtTransform = Mat44::MakeLookAtTransform( end, start );
    Vec3 const& bottom          = start;
    Vec3 const& top             = end;

    Vec3        tangent( 0, 0, 0 );
    Vec3        bitangent( 0, 0, 0 );

    Vec3        bottomNormal = ( start - end ).GetNormalized();
    Vec3        topNormal    = ( end - start ).GetNormalized();

    for ( int i = 0; i < numSlices; ++i )
    {
        float cos     = CosDegrees( degreesPerSlice * static_cast< float >( i ) );
        float sin     = SinDegrees( degreesPerSlice * static_cast< float >( i ) );
        float nextCos = CosDegrees( degreesPerSlice * static_cast< float >( i + 1 ) );
        float nextSin = SinDegrees( degreesPerSlice * static_cast< float >( i + 1 ) );

        Vec3  right = radius * cos * lookAtTransform.GetJBasis3D() + radius * sin * lookAtTransform.GetKBasis3D();
        Vec3  left  = radius * nextCos * lookAtTransform.GetJBasis3D() + radius * nextSin * lookAtTransform.GetKBasis3D();

        Vec3  bl = bottom + left;
        Vec3  br = bottom + right;
        Vec3  tr = top + right;
        Vec3  tl = top + left;

        float uMin = 1.0f - static_cast< float >( i + 1 ) / static_cast< float >( numSlices );
        float uMax = 1.0f - static_cast< float >( i ) / static_cast< float >( numSlices );

        Vec2  ccw0 = Vec2( UVs.m_maxs.x * 0.5f, UVs.m_maxs.y * 0.5f );
        Vec2  ccw1 = ccw0 + Vec2( cos * 0.5f, sin * 0.5f );
        Vec2  ccw2 = ccw0 + Vec2( nextCos * 0.5f, nextSin * 0.5f );

        verts.emplace_back( bottom, color, ccw0, tangent, bitangent, bottomNormal );
        verts.emplace_back( br, color, ccw1, tangent, bitangent, bottomNormal );
        verts.emplace_back( bl, color, ccw2, tangent, bitangent, bottomNormal );

        Vec3 normalBL = left.GetNormalized();
        Vec3 normalBR = right.GetNormalized();

        verts.emplace_back( bl, color, Vec2( uMin, 0.f ), tangent, bitangent, normalBL );
        verts.emplace_back( br, color, Vec2( uMax, 0.f ), tangent, bitangent, normalBR );
        verts.emplace_back( tr, color, Vec2( uMax, 1.f ), tangent, bitangent, normalBR );

        verts.emplace_back( bl, color, Vec2( uMin, 0.f ), tangent, bitangent, normalBL );
        verts.emplace_back( tr, color, Vec2( uMax, 1.f ), tangent, bitangent, normalBR );
        verts.emplace_back( tl, color, Vec2( uMin, 1.f ), tangent, bitangent, normalBL );

        ccw1 = ccw0 + Vec2( cos * 0.5f, -sin * 0.5f );
        ccw2 = ccw0 + Vec2( nextCos * 0.5f, -nextSin * 0.5f );

        verts.emplace_back( top, color, ccw0, tangent, bitangent, topNormal );
        verts.emplace_back( tl, color, ccw2, tangent, bitangent, topNormal );
        verts.emplace_back( tr, color, ccw1, tangent, bitangent, topNormal );
    }
}

//-----------------------------------------------------------------------------------------------
void AddVertsForCylinder3D( std::vector< Vertex >& verts, Vec2 const& centerXY, FloatRange const& minMaxz, float radius, float numSlices, Rgba8 const& tint /*= Rgba8::WHITE*/, AABB2 const& UVs /*= AABB2::ZERO_TO_ONE */ )
{
    Vec3 start = Vec3( centerXY.x, centerXY.y, minMaxz.m_min );
    Vec3 end   = Vec3( centerXY.x, centerXY.y, minMaxz.m_max );
    AddVertsForCylinder3D( verts, start, end, radius, tint, UVs, static_cast< int >( numSlices ) );
}

//-----------------------------------------------------------------------------------------------
void AddVertsForCylinder3D( std::vector< Vertex >& verts, std::vector< unsigned int >& indexes, Vec2 const& centerXY, FloatRange const& minMaxz, float radius, float numSlices, Rgba8 const& tint /*= Rgba8::WHITE*/, AABB2 const& UVs /*= AABB2::ZERO_TO_ONE */ )
{
    unsigned int startIndex      = static_cast< unsigned int >( verts.size() );
    Vec3         start           = Vec3( centerXY.x, centerXY.y, minMaxz.m_min );
    Vec3         end             = Vec3( centerXY.x, centerXY.y, minMaxz.m_max );
    Vec3 const&  bottom          = start;
    Vec3 const&  top             = end;
    float        degreesPerSlice = 360.f / numSlices;
    Vec3         bottomNormal    = ( start - end ).GetNormalized();
    Vec3         topNormal       = ( end - start ).GetNormalized();

    Vec3         capTangent   = Vec3( 1.f, 0.f, 0.f );
    Vec3         capBitangent = Vec3( 0.f, 1.f, 0.f );

    // Bottom cap
    verts.emplace_back( bottom, tint, Vec2( 0.5f, 0.5f ), capTangent, capBitangent, Vec3( 0.f, 0.f, -1.f ) );
    for ( int i = 0; i < static_cast< int >( numSlices ); ++i )
    {
        float cos  = CosDegrees( degreesPerSlice * static_cast< float >( i ) );
        float sin  = SinDegrees( degreesPerSlice * static_cast< float >( i ) );
        Vec3  left = Vec3( radius * cos, radius * sin, 0.f );
        verts.emplace_back( bottom + left, tint, Vec2( cos * 0.5f + 0.5f, sin * 0.5f + 0.5f ), capTangent, capBitangent, Vec3( 0.f, 0.f, -1.f ) );
    }
    for ( int i = 0; i < static_cast< int >( numSlices ); ++i )
    {
        indexes.push_back( startIndex );
        indexes.push_back( startIndex + 1 + ( i + 1 ) % static_cast< int >( numSlices ) );
        indexes.push_back( startIndex + 1 + i );
    }

    // Top cap
    unsigned int topStartIndex = static_cast< unsigned int >( verts.size() );
    verts.emplace_back( top, tint, Vec2( 0.5f, 0.5f ), capTangent, capBitangent, Vec3( 0.f, 0.f, 1.f ) );
    for ( int i = 0; i < static_cast< int >( numSlices ); ++i )
    {
        float cos  = CosDegrees( degreesPerSlice * static_cast< float >( i ) );
        float sin  = SinDegrees( degreesPerSlice * static_cast< float >( i ) );
        Vec3  left = Vec3( radius * cos, radius * sin, 0.f );
        verts.emplace_back( top + left, tint, Vec2( cos * 0.5f + 0.5f, sin * 0.5f + 0.5f ), capTangent, capBitangent, Vec3( 0.f, 0.f, 1.f ) );
    }
    for ( int i = 0; i < static_cast< int >( numSlices ); ++i )
    {
        indexes.push_back( topStartIndex );
        indexes.push_back( topStartIndex + 1 + i );
        indexes.push_back( topStartIndex + 1 + ( i + 1 ) % static_cast< int >( numSlices ) );
    }

    // Side
    unsigned int sideStartIndex = static_cast< unsigned int >( verts.size() );
    for ( int i = 0; i < numSlices; ++i )
    {
        float cos     = CosDegrees( degreesPerSlice * static_cast< float >( i ) );
        float sin     = SinDegrees( degreesPerSlice * static_cast< float >( i ) );
        float nextCos = CosDegrees( degreesPerSlice * static_cast< float >( i + 1 ) );
        float nextSin = SinDegrees( degreesPerSlice * static_cast< float >( i + 1 ) );

        Vec3  right = Vec3( radius * cos, radius * sin, 0.f );
        Vec3  left  = Vec3( radius * nextCos, radius * nextSin, 0.f );

        Vec3  normalR = right.GetNormalized();
        Vec3  normalL = left.GetNormalized();

        Vec3  tangentR   = CrossProduct3D( Vec3( 0.f, 0.f, 1.f ), normalR ).GetNormalized();
        Vec3  tangentL   = CrossProduct3D( Vec3( 0.f, 0.f, 1.f ), normalL ).GetNormalized();
        Vec3  bitangentR = CrossProduct3D( normalR, tangentR );
        Vec3  bitangentL = CrossProduct3D( normalL, tangentL );

        Vec3  bl = bottom + left;
        Vec3  br = bottom + right;
        Vec3  tr = top + right;
        Vec3  tl = top + left;

        float u       = static_cast< float >( i ) / numSlices;
        float uNext   = static_cast< float >( i + 1 ) / numSlices;
        float vBottom = UVs.m_mins.y;
        float vTop    = UVs.m_maxs.y;

        verts.emplace_back( bl, tint, Vec2( uNext, vBottom ), tangentL, bitangentL, normalL );
        verts.emplace_back( br, tint, Vec2( u, vBottom ), tangentR, bitangentR, normalR );
        verts.emplace_back( tl, tint, Vec2( uNext, vTop ), tangentL, bitangentL, normalL );
        verts.emplace_back( tr, tint, Vec2( u, vTop ), tangentR, bitangentR, normalR );
    }

    for ( int i = 0; i < static_cast< int >( numSlices ); ++i )
    {
        unsigned int base = sideStartIndex + i * 4;
        indexes.push_back( base + 1 );
        indexes.push_back( base + 0 );
        indexes.push_back( base + 2 );
        indexes.push_back( base + 1 );
        indexes.push_back( base + 2 );
        indexes.push_back( base + 3 );
    }
}

//-----------------------------------------------------------------------------------------------
void AddVertsForCylinderZWireFrame3D( std::vector< Vertex >& verts, Vec2 const& centerXY, FloatRange const& minMaxz, float radius, float numSlices, float lineThickness, Rgba8 const& tint /*= Rgba8::WHITE */ )
{
    Vec3        start = Vec3( centerXY.x, centerXY.y, minMaxz.m_min );
    Vec3        end   = Vec3( centerXY.x, centerXY.y, minMaxz.m_max );

    float       degreesPerSlice = 360.f / numSlices;
    Mat44       lookAtTransform = Mat44::MakeLookAtTransform( end, start );
    Vec3 const& bottom          = start;
    Vec3 const& top             = end;

    for ( int i = 0; i < numSlices; ++i )
    {
        float cos     = CosDegrees( degreesPerSlice * static_cast< float >( i ) );
        float sin     = SinDegrees( degreesPerSlice * static_cast< float >( i ) );
        float nextCos = CosDegrees( degreesPerSlice * static_cast< float >( i + 1 ) );
        float nextSin = SinDegrees( degreesPerSlice * static_cast< float >( i + 1 ) );

        Vec3  right = radius * cos * lookAtTransform.GetJBasis3D() + radius * sin * lookAtTransform.GetKBasis3D();
        Vec3  left  = radius * nextCos * lookAtTransform.GetJBasis3D() + radius * nextSin * lookAtTransform.GetKBasis3D();

        Vec3  bl = bottom + left;
        Vec3  br = bottom + right;
        Vec3  tr = top + right;
        Vec3  tl = top + left;

        AddVertsForQuadWireFrame( verts, lineThickness, bl, br, tr, tl, tint );
    }
}

//-----------------------------------------------------------------------------------------------
void AddVertsForCone3D( std::vector< Vertex >& verts, Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color /*= Rgba8::WHITE*/, AABB2 const& UVs /*= AABB2::ZERO_TO_ONE*/, int numSlices /*= 32 */ )
{
    float degreesPerSlice = 360.f / numSlices;
    Mat44 lookAtTransform = Mat44::MakeLookAtTransform( end, start );
    Vec3  bottom          = start;
    Vec3  top             = end;

    for ( int i = 0; i < numSlices; ++i )
    {
        Vec3 right = radius * CosDegrees( degreesPerSlice * static_cast< float >( i ) ) * lookAtTransform.GetJBasis3D() + radius * SinDegrees( degreesPerSlice * static_cast< float >( i ) ) * lookAtTransform.GetKBasis3D();
        Vec3 left  = radius * CosDegrees( degreesPerSlice * static_cast< float >( i + 1 ) ) * lookAtTransform.GetJBasis3D() + radius * SinDegrees( degreesPerSlice * static_cast< float >( i + 1 ) ) * lookAtTransform.GetKBasis3D();

        Vec3 bl = bottom + left;
        Vec3 br = bottom + right;

        Vec2 ccw0 = Vec2( UVs.m_maxs.x * 0.5f, UVs.m_maxs.y * 0.5f );
        Vec2 ccw1 = ccw0 + Vec2( CosDegrees( degreesPerSlice * static_cast< float >( i ) ) * 0.5f, SinDegrees( degreesPerSlice * static_cast< float >( i ) ) * 0.5f );
        Vec2 ccw2 = ccw0 + Vec2( CosDegrees( degreesPerSlice * static_cast< float >( i + 1 ) ) * 0.5f, SinDegrees( degreesPerSlice * static_cast< float >( i + 1 ) ) * 0.5f );

        // Vec2 uv0, Vec2 uv1, Vec2 uvPole
        AddVertsForTriangle3D( verts, bottom, br, bl, color, ccw0, ccw1, ccw2 );
        AddVertsForTriangle3D( verts, top, bl, br, color, ccw0, ccw2, ccw1 );
    }
}

//-----------------------------------------------------------------------------------------------
void AddVertsForAABB3D( std::vector< Vertex >& verts, AABB3 const& bounds, Rgba8 const& color, AABB2 const& UVs )
{
    AddVertsForAABB3D( verts, bounds, color, color, color, color, color, color, UVs );
}

//-----------------------------------------------------------------------------------------------
void AddVertsForAABB3D( std::vector< Vertex >& verts, AABB3 const& bounds, Rgba8 const& colXPos, Rgba8 const& colXNeg, Rgba8 const& colYPos, Rgba8 const& colYNeg, Rgba8 const& colZPos, Rgba8 const& colZNeg, AABB2 const& UVs )
{
    Vec3 posA = Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z );
    Vec3 posB = Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z );
    Vec3 posC = Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z );
    Vec3 posD = Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z );
    Vec3 posE = Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z );
    Vec3 posF = Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z );
    Vec3 posG = Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z );
    Vec3 posH = Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z );

    AddVertsForQuad3D( verts, posA, posB, posC, posD, colXPos, UVs );  // +X
    AddVertsForQuad3D( verts, posF, posE, posH, posG, colXNeg, UVs );  // -X
    AddVertsForQuad3D( verts, posB, posF, posG, posC, colYPos, UVs );  // +Y
    AddVertsForQuad3D( verts, posE, posA, posD, posH, colYNeg, UVs );  // -Y
    AddVertsForQuad3D( verts, posD, posC, posG, posH, colZPos, UVs );  // +Z
    AddVertsForQuad3D( verts, posE, posF, posB, posA, colZNeg, UVs );  // -Z
}

//-----------------------------------------------------------------------------------------------
void AddVertsForAABB3D( std::vector< Vertex >& verts, std::vector< unsigned int >& indexes, AABB3 const& bounds, Rgba8 const& color, AABB2 const& UVs )
{
    Vec3 posA = Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z );
    Vec3 posB = Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z );
    Vec3 posC = Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z );
    Vec3 posD = Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z );
    Vec3 posE = Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z );
    Vec3 posF = Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z );
    Vec3 posG = Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z );
    Vec3 posH = Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z );

    AddVertsForQuad3D( verts, indexes, posA, posB, posC, posD, color, UVs );  // +X
    AddVertsForQuad3D( verts, indexes, posF, posE, posH, posG, color, UVs );  // -X
    AddVertsForQuad3D( verts, indexes, posB, posF, posG, posC, color, UVs );  // +Y
    AddVertsForQuad3D( verts, indexes, posE, posA, posD, posH, color, UVs );  // -Y
    AddVertsForQuad3D( verts, indexes, Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z ), Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z ), Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z ), Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z ), color, UVs );
    AddVertsForQuad3D( verts, indexes, Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ), Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z ), Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z ), Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z ), color, UVs );
}

//-----------------------------------------------------------------------------------------------
AABB2 GetVertexBounds2D( std::vector< Vertex >& verts )
{
    AABB2 bounds;
    bounds.m_mins.x = FLT_MAX;
    bounds.m_maxs.x = -FLT_MAX;
    bounds.m_mins.y = FLT_MAX;
    bounds.m_maxs.y = -FLT_MAX;

    for ( int vertIndex = 0; vertIndex < static_cast< int >( verts.size() ); ++vertIndex )
    {
        if ( verts[ vertIndex ].m_position.x < bounds.m_mins.x )
        {
            bounds.m_mins.x = verts[ vertIndex ].m_position.x;
        }

        if ( verts[ vertIndex ].m_position.x > bounds.m_maxs.x )
        {
            bounds.m_maxs.x = verts[ vertIndex ].m_position.x;
        }

        if ( verts[ vertIndex ].m_position.y < bounds.m_mins.y )
        {
            bounds.m_mins.y = verts[ vertIndex ].m_position.y;
        }

        if ( verts[ vertIndex ].m_position.y > bounds.m_maxs.y )
        {
            bounds.m_maxs.y = verts[ vertIndex ].m_position.y;
        }
    }

    return bounds;
}

//-----------------------------------------------------------------------------------------------
void IndexifyStaticMesh( std::vector< Vertex >& vertexArray_InOut, std::vector< unsigned int >& indexArray_Out )
{
    std::vector< Vertex > oldVerts;
    vertexArray_InOut.swap( oldVerts );
    std::vector< Vertex >&       newVerts   = vertexArray_InOut;
    std::vector< unsigned int >& newIndexes = indexArray_Out;

    newVerts.reserve( oldVerts.size() );
    indexArray_Out.reserve( oldVerts.size() );

    for ( int oldVertsIdx = 0; oldVertsIdx < (int)oldVerts.size(); ++oldVertsIdx )
    {
        Vertex const& oldV = oldVerts[ oldVertsIdx ];

        bool          isDuplicate = false;
        for ( int newVertsIdx = 0; newVertsIdx < (int)newVerts.size(); ++newVertsIdx )
        {
            Vertex const& newV = newVerts[ newVertsIdx ];
            if ( oldV == newV )
            {
                isDuplicate = true;
                indexArray_Out.push_back( newVertsIdx );
                break;
            }
        }

        if ( !isDuplicate )
        {
            newIndexes.push_back( (unsigned int)oldVerts.size() );
            newVerts.push_back( oldV );
        }
    }
}

//-----------------------------------------------------------------------------------------------
void GenerateNormalsPerSurface( std::vector< Vertex >& vertices, std::vector< unsigned int >& indices )
{
    for ( int i = 0; i < static_cast< int >( indices.size() ); i += 3 )
    {
        unsigned int vertexIndex0 = indices[ i + 0 ];
        unsigned int vertexIndex1 = indices[ i + 1 ];
        unsigned int vertexIndex2 = indices[ i + 2 ];

        Vec3         ccw0 = vertices[ vertexIndex0 ].m_position;
        Vec3         ccw1 = vertices[ vertexIndex1 ].m_position;
        Vec3         ccw2 = vertices[ vertexIndex2 ].m_position;

        Vec3         AB     = ccw1 - ccw0;
        Vec3         AC     = ccw2 - ccw0;
        Vec3         normal = CrossProduct3D( AB, AC ).GetNormalized();

        vertices[ vertexIndex0 ].m_normal = normal;
        vertices[ vertexIndex1 ].m_normal = normal;
        vertices[ vertexIndex2 ].m_normal = normal;
    }
}

//-----------------------------------------------------------------------------------------------
void GenerateNormalsPerVertex( std::vector< Vertex >& vertices, std::vector< unsigned int >& indices )
{
    for ( int i = 0; i < static_cast< int >( indices.size() ); i += 3 )
    {
        unsigned int vertexIndex0 = indices[ i + 0 ];
        unsigned int vertexIndex1 = indices[ i + 1 ];
        unsigned int vertexIndex2 = indices[ i + 2 ];

        Vec3         ccw0 = vertices[ vertexIndex0 ].m_position;
        Vec3         ccw1 = vertices[ vertexIndex1 ].m_position;
        Vec3         ccw2 = vertices[ vertexIndex2 ].m_position;

        Vec3         AB     = ccw1 - ccw0;
        Vec3         AC     = ccw2 - ccw0;
        Vec3         normal = CrossProduct3D( AB, AC ).GetNormalized();

        vertices[ vertexIndex0 ].m_normal += normal;
        vertices[ vertexIndex1 ].m_normal += normal;
        vertices[ vertexIndex2 ].m_normal += normal;
    }

    for ( int vertexIndex = 0; vertexIndex < static_cast< int >( vertices.size() ); ++vertexIndex )
    {
        vertices[ vertexIndex ].m_normal.Normalize();
    }
}

//-----------------------------------------------------------------------------------------------
void GenerateTangentsFromUVs( std::vector< Vertex >& vertices, std::vector< unsigned int >& indices, bool hasNormal )
{
    for ( int i = 0; i < static_cast< int >( indices.size() ); i += 3 )
    {
        unsigned int vertexIndex0 = indices[ i + 0 ];
        unsigned int vertexIndex1 = indices[ i + 1 ];
        unsigned int vertexIndex2 = indices[ i + 2 ];

        Vertex&      vertex0 = vertices[ vertexIndex0 ];
        Vertex&      vertex1 = vertices[ vertexIndex1 ];
        Vertex&      vertex2 = vertices[ vertexIndex2 ];

        Vec3         pos0 = vertex0.m_position;
        Vec3         pos1 = vertex1.m_position;
        Vec3         pos2 = vertex2.m_position;

        Vec2         uv0 = vertex0.m_uvTexCoords;
        Vec2         uv1 = vertex1.m_uvTexCoords;
        Vec2         uv2 = vertex2.m_uvTexCoords;

        Vec3         E0 = pos1 - pos0;
        Vec3         E1 = pos2 - pos0;

        if ( !hasNormal )
        {
            Vec3 normal = CrossProduct3D( E0, E1 );

            if ( normal.GetLengthSquared() > 0.000001f )
            {
                normal.Normalize();

                vertex0.m_normal += normal;
                vertex1.m_normal += normal;
                vertex2.m_normal += normal;
            }
        }

        float deltaU0 = uv1.x - uv0.x;
        float deltaU1 = uv2.x - uv0.x;

        float deltaV0 = uv1.y - uv0.y;
        float deltaV1 = uv2.y - uv0.y;

        float determinant = deltaU0 * deltaV1 - deltaU1 * deltaV0;

        if ( fabsf( determinant ) < 0.000001f )
        {
            continue;
        }

        float r = 1.f / determinant;

        Vec3  T = r * ( deltaV1 * E0 - deltaV0 * E1 );
        Vec3  B = r * ( deltaU0 * E1 - deltaU1 * E0 );

        if ( T.GetLengthSquared() > 0.000001f )
        {
            T.Normalize();

            vertex0.m_tangent += T;
            vertex1.m_tangent += T;
            vertex2.m_tangent += T;
        }

        if ( B.GetLengthSquared() > 0.000001f )
        {
            B.Normalize();

            vertex0.m_bitangent += B;
            vertex1.m_bitangent += B;
            vertex2.m_bitangent += B;
        }
    }

    for ( int vertexIndex = 0; vertexIndex < static_cast< int >( vertices.size() ); ++vertexIndex )
    {
        Vertex& vertex = vertices[ vertexIndex ];

        // Make sure normal is valid.
        if ( vertex.m_normal.GetLengthSquared() < 0.000001f )
        {
            vertex.m_normal = Vec3( 0.f, 0.f, 1.f );
        }
        else
        {
            vertex.m_normal.Normalize();
        }

        // Gram-Schmidt: remove the normal component from tangent.
        vertex.m_tangent = vertex.m_tangent - vertex.m_normal * DotProduct3D( vertex.m_normal, vertex.m_tangent );

        // If UV tangent failed, generate arbitrary tangent from normal.
        if ( vertex.m_tangent.GetLengthSquared() < 0.000001f )
        {
            Vec3 arbitrary = fabsf( vertex.m_normal.z ) < 0.999f ? Vec3( 0.f, 0.f, 1.f ) : Vec3( 0.f, 1.f, 0.f );

            vertex.m_tangent = CrossProduct3D( arbitrary, vertex.m_normal );

            if ( vertex.m_tangent.GetLengthSquared() < 0.000001f )
            {
                vertex.m_tangent = Vec3( 1.f, 0.f, 0.f );
            }
            else
            {
                vertex.m_tangent.Normalize();
            }
        }
        else
        {
            vertex.m_tangent.Normalize();
        }

        vertex.m_bitangent = CrossProduct3D( vertex.m_normal, vertex.m_tangent );

        if ( vertex.m_bitangent.GetLengthSquared() < 0.000001f )
        {
            vertex.m_bitangent = Vec3( 0.f, 1.f, 0.f );
        }
        else
        {
            vertex.m_bitangent.Normalize();
        }
    }
}

//-----------------------------------------------------------------------------------------------
void GenerateTangentsArbitrary( std::vector< Vertex >& vertices )
{
    Vec3 worldLeft = Vec3::WORLD_LEFT;
    Vec3 worldUp   = Vec3::WORLD_UP;

    for ( int i = 0; i < static_cast< int >( vertices.size() ); ++i )
    {
        Vertex& vertex = vertices[ i ];
        vertex.m_normal.Normalize();

        if ( fabsf( DotProduct3D( vertex.m_normal, worldUp ) ) >= 0.99 )
        {
            vertex.m_bitangent = CrossProduct3D( worldLeft, vertex.m_normal ).GetNormalized();
            vertex.m_tangent   = CrossProduct3D( vertex.m_bitangent, vertex.m_normal ).GetNormalized();
        }
        else
        {
            vertex.m_tangent   = CrossProduct3D( worldUp, vertex.m_normal ).GetNormalized();
            vertex.m_bitangent = CrossProduct3D( vertex.m_normal, vertex.m_tangent ).GetNormalized();
        }
    }
}