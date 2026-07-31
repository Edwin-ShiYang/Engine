#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/RaycastUtils.hpp"
#include "Engine/Math/Plane3.hpp"

//-----------------------------------------------------------------------------------------------
struct Vec2;
struct Vec3;
struct Vec4;
struct AABB2;
struct Mat44;

//-----------------------------------------------------------------------------------------------
enum class BillboardType
{
    None = -1,
    WORLD_UP_FACING,
    WORLD_UP_OPPOSING,
    FULL_FACING,
    FULL_OPPOSING,
    COUNT
};

//-----------------------------------------------------------------------------------------------
constexpr float PI = 3.14159265f;

//-----------------------------------------------------------------------------------------------
namespace Math
{
    template < typename T >
    T Min( T const& a, T const& b )
    {
        return a < b ? a : b;
    }

    template < typename T >
    T Max( T const& a, T const& b )
    {
        return a > b ? a : b;
    }
}

//-----------------------------------------------------------------------------------------------
template < typename T >
T GetClamped( T const& value, T const& minValue, T const& maxValue )
{
    if ( value < minValue ) return minValue;
    if ( value > maxValue ) return maxValue;
    return value;
}

//-----------------------------------------------------------------------------------------------
template < typename T >
T GetFractionWithinRange( T value, T rangeStart, T rangeEnd )
{
    if ( rangeStart == rangeEnd ) return 0.5f;
    return ( value - rangeStart ) / ( rangeEnd - rangeStart );
}

//-----------------------------------------------------------------------------------------------
float           GetClampedZeroToOne( float value );
float           Interpolate( float start, float end, float fractionTowardEnd );
Vec3            Interpolate( Vec3 const& start, Vec3 const& end, float fractionTowardEnd );
Vec4            Interpolate( Vec4 const& start, Vec4 const& end, float fractionTowardEnd );

float           RangeMap( float inValue, float inStart, float inEnd, float outStart, float outEnd );
float           RangeMapClamped( float inValue, float inStart, float inEnd, float outStart, float outEnd );
int             RoundDownToInt( float value );

float           ConvertDegreesToRadians( float degrees );
float           ConvertRadiansToDegrees( float radians );
float           CosDegrees( float degree );
float           SinDegrees( float degree );
float           Atan2Degrees( float y, float x );
float           GetShortestAngularDispDegrees( float startDegrees, float endDegrees );
float           GetTurnedTowardDegrees( float currentDegrees, float goalDegrees, float maxDeltaDegrees );
int             GetTaxicabDistance2D( IntVec2 const& pointA, IntVec2 const& pointB );
float           GetProjectedLength2D( Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto );
Vec2            GetProjectedVector2D( Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto );
float           GetAngleDegreesBetweenVectors2D( Vec2 const& a, Vec2 const& b );

float           NormalizeByte( unsigned char byteValue );
unsigned char   DenormalizeByte( float zeroToOne );

float           GetDistance2D( Vec2 const& positionA, Vec2 const& positionB );
float           GetDistance3D( Vec3 const& positionA, Vec3 const& positionB );
float           GetDistanceSquared2D( Vec2 const& positionA, Vec2 const& positionB );
float           GetDistanceSquared3D( Vec3 const& positionA, Vec3 const& positionB );
float           GetDistanceXY3D( Vec3 const& positionA, Vec3 const& positionB );
float           GetDistanceXYSquared3D( Vec3 const& positionA, Vec3 const& positionB );

Vec2            GetDirection2D( Vec3 const& direction3D );

float           CrossProduct2D( Vec2 const& a, Vec2 const& b );
Vec3            CrossProduct3D( Vec3 const& a, Vec3 const& b );
float           DotProduct2D( Vec2 const& a, Vec2 const& b );
float           DotProduct3D( Vec3 const& a, Vec3 const& b );
float           DotProduct4D( Vec4 const& a, Vec4 const& b );

bool            IsPointInsideDisc2D( Vec2 const& point, Vec2 const& discCenter, float discRadius );
bool            IsPointInsideOrOnDisc2D( Vec2 const& point, Vec2 const& discCenter, float discRadius );

bool            IsPointInsideAABB2D( Vec2 point, AABB2 const& alignedBox );
bool            IsPointInsideOBB2D( Vec2 point, OBB2 const& orientedBox );
bool            IsPointInsideCapsule2D( Vec2 point, Vec2 boneStart, Vec2 boneEnd, float radius );
bool            IsPointInsideTriangle2D( Vec2 point, Vec2 ccw0, Vec2 ccw1, Vec2 ccw2 );
bool            IsPointInsideOrientedSector2D( Vec2 const& point, Vec2 const& sectorTip, float sectorFwdDegrees, float sectorApertureDegrees, float sectorRadius );
bool            IsPointInsideDirectedSector2D( Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorFwdNormal, float sectorApertureDegrees, float sectorRadius );
bool            IsPointInsideSphere3D( Vec3 const& point, Vec3 const& discCenter, float discRadius );
bool            IsPointInsideAABB3D( Vec3 const& point, AABB3 const& alignedBox );
bool            IsPointInsideCylinderZ( Vec3 const& point, Vec2 const& centerXY, float radius, FloatRange zMinMax );
bool            IsPointInsideOBB3D( Vec3 point, OBB3 const& orientedBox );

Vec2            GetNearestPointOnDisc2D( Vec2 const& referencePos, Vec2 const& discCenter, float discRadius );
Vec2            GetNearestPointOnAABB2D( Vec2 referencePos, AABB2 const& alignedBox );
Vec2            GetNearestPointOnInfiniteLine2D( Vec2 referencePos, Vec2 pointOnLine, Vec2 anotherPointOnLine );
Vec2            GetNearestPointOnLineSegment2D( Vec2 referencePos, Vec2 start, Vec2 end );
Vec2            GetNearestPointOnCapsule2D( Vec2 referencePos, Vec2 boneStart, Vec2 boneEnd, float radius );
Vec2            GetNearestPointOnTriangle2D( Vec2 referencePos, Vec2 ccw0, Vec2 ccw1, Vec2 ccw2 );
Vec3            GetNearestPointOnSphere3D( Vec3 const& referencePos, Vec3 const& sphereCenter, float sphereRadius );
Vec3            GetNearestPointOnAABB3D( Vec3 const& referencePos, AABB3 const& alignedBox );
Vec3            GetNearestPointOnCylinderZ( Vec3 const& referencePos, Vec2 const& centerXY, float radius, FloatRange zMinMax );

Vec2            GetNearestPointOnOBB2D( Vec2 const& referencePos, OBB2 const& orientedBox );
Vec3            GetNearestPointOnOBB3D( Vec3 const& referencePos, OBB3 const& orientedBox );

Vec3            GetNearestPointOnPlane3( Vec3 const& referencePos, Plane3 const& plane );

bool            DoDiscsOverlap( Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB );
bool            DoAABB2DsOverlap( AABB2 const& alignedBoxA, AABB2 const& alignedBoxB );
bool            DoDiscAndAABBOverlap2D( Vec2 const& center, float radius, AABB2 const& box );
bool            DoAABBsOverlap3D( AABB3 const& first, AABB3 const& second );
bool            DoSpheresOverlap( Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB );
bool            DoZAlignedCapsulesOverlap3D( Vec3 centerA, float radiusA, float boneHalfHeightA, Vec3 centerB, float radiusB, float boneHalfHeightB );

bool            DoSphereAndOBB3Overlap( Vec3 const& centerA, float radiusA, OBB3 const& orientedBox );
bool            DoSphereAndPlane3Overlap( Vec3 const& centerA, float radiusA, Plane3 const& plane );

bool            DoAABBAndPlaneOverlap3D( AABB3 const& alignedBox, Plane3 const& plane );
bool            DoOBBAndPlaneOverlap3D( OBB3 const& orientedBox, Plane3 const& plane );

bool            DiscBounceOffDisc( Vec2& discACenter, float discARadius, Vec2& discAVelocity, float discAElasticity, Vec2& discBCenter, float discBRadius, Vec2& discBVelocity, float discBElasticity );
bool            DoSphereAndAABBOverlap3D( Vec3 sphereCenter, float sphereRadius, AABB3 box );
bool            DoZCylindersOverlap3D( Vec2 cylinder1CenterXY, float cylinder1Radius, FloatRange cylinder1MinMaxZ, Vec2 cylinder2CenterXY, float cylinder2Radius, FloatRange cylinder2MinMaxZ );
bool            DoZCylinderAndAABBOverlap3D( Vec2 cylinderCenterXY, float cylinderRadius, FloatRange cylinderMinMaxZ, AABB3 box );
bool            DoZCylinderAndSphereOverlap3D( Vec2 cylinderCenterXY, float cylinderRadius, FloatRange cylinderMinMaxZ, Vec3 sphereCenter, float sphereRadius );

bool            PushDiscOutOfFixedPoint2D( Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedPoint );
bool            PushDiscOutOfFixedDisc2D( Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius );
bool            PushDiscsOutOfEachOther2D( Vec2& aCenter, float aRadius, Vec2& bCenter, float bRadius );
bool            PushDiscOutOfFixedAABB2D( Vec2& mobileDiscCenter, float discRadius, AABB2 const& fixedBox );
bool            PushZCylinderOutofEachOther3D( Vec2& zCylinderACenterXY, float zCylinderARadiusXY, FloatRange zCylinderAMinMaxZ, Vec2& zCylinderBCenterXY, float zCylinderBRadiusXY, FloatRange zCylinderBMinMaxZ );
bool            PushZCylinderOutofFixedZCylinder3D( Vec2& mobileZCylinderCenterXY, float mobileZCylinderRadiusXY, FloatRange mobileZCylinderMinMaxZ, Vec2 const& fixedZCylinderCenterXY, float fixedZCylinderRadiusXY, FloatRange fixedZCylinderMinMaxZ );
bool            PushCapsuleOutOfFixedCapsule3D( Vec3& centerA, float radiusA, float boneHalfHeightA, Vec3 const& centerB, float radiusB, float boneHalfHeightB );

RaycastResult2D RaycastVsDisc2D( Vec2 rayStart, Vec2 rayFwdNormal, float rayLength, Vec2 discCenter, float discRadius );
RaycastResult2D RaycastVsLineSegments2D( Vec2 rayStart, Vec2 rayFwdNormal, float rayLength, Vec2 segStart, Vec2 segEnd );
RaycastResult2D RaycastVsAABB2D( Vec2 rayStart, Vec2 rayEnd, Vec2 rayFwdNormal, float rayLength, AABB2 const& alignedBox );

RaycastResult3D RaycastVsAABB3D( Vec3 rayStart, Vec3 rayForwardNormal, float rayLength, AABB3 box );
RaycastResult3D RaycastVsSphere3D( Vec3 rayStart, Vec3 rayForwardNormal, float rayLength, Vec3 sphereCenter, float sphereRadius );
RaycastResult3D RaycastVsCylinderZ3D( Vec3 rayStart, Vec3 rayForwardNormal, float rayLength, Vec2 const& centerXY, FloatRange const& minMaxZ, float radiusXY );
RaycastResult3D RaycastVsCylinderZ3D_BruteForce( Vec3 const& start, Vec3 const& direction, float distance, Vec2 const& center, float minZ, float maxZ, float radius );

RaycastResult3D RaycastVsPlane3D( Vec3 rayStart, Vec3 rayForwardNormal, float rayLength, Plane3 plane );
RaycastResult3D RaycastVsOBB3( Vec3 rayStart, Vec3 rayForwardNormal, float rayLength, OBB3 orientedBox );

void            TransformPosition2D( Vec2& posToTransform, float uniformScale, float rotationDegrees, Vec2 const& translation );
void            TransformPositionXY3D( Vec3& posToTransform, float xyScale, float zRotationDegrees, Vec2 const& xyTranslation );
void            TransformPosition2D( Vec2& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation );
void            TransformPositionXY3D( Vec3& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation );

Mat44 const     GetBillboard( BillboardType billboardType, Mat44 const& targetTransform, Vec3 const& billboardPosition, Vec2 const& billboardScale = Vec2( 1.0f, 1.0f ) );

float           ComputeCubicBezier1D( float A, float B, float C, float D, float t );
float           ComputeQuinticBezier1D( float A, float B, float C, float D, float E, float F, float t );

float           EaseLinear( float t );
float           SmoothStart2( float t );
float           SmoothStart3( float t );
float           SmoothStart4( float t );
float           SmoothStart5( float t );
float           SmoothStart6( float t );

float           SmoothStop2( float t );
float           SmoothStop3( float t );
float           SmoothStop4( float t );
float           SmoothStop5( float t );
float           SmoothStop6( float t );

float           SmoothStep3( float t );
float           SmoothStep5( float t );

float           Hesitate3( float t );
float           Hesitate5( float t );

float           SmoothStep3_SmoothStart2( float t );
float           SmoothStep3_SmoothStop2( float t );
float           SmoothStep5_SmoothStart2( float t );
float           SmoothStep5_SmoothStop2( float t );