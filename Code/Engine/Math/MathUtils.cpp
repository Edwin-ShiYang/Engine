#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include <math.h>

//-----------------------------------------------------------------------------------------------
int GetMaxInt( int a, int b )
{
    return a > b ? a : b;
}

//-----------------------------------------------------------------------------------------------
float GetMinFloat( float a, float b )
{
    return a < b ? a : b;
}

//-----------------------------------------------------------------------------------------------
float GetClamped( float value, float minValue, float maxValue )
{
    if ( value < minValue )
    {
        return minValue;
    }
    if ( value > maxValue )
    {
        return maxValue;
    }

    return value;
}

//-----------------------------------------------------------------------------------------------
double GetClamped( double value, double minValue, double maxValue )
{
    if ( value < minValue )
    {
        return minValue;
    }
    if ( value > maxValue )
    {
        return maxValue;
    }

    return value;
}

//-----------------------------------------------------------------------------------------------
int GetClamped( int value, int minValue, int maxValue )
{
    if ( value < minValue )
    {
        return minValue;
    }
    if ( value > maxValue )
    {
        return maxValue;
    }

    return value;
}

//-----------------------------------------------------------------------------------------------
float GetClampedZeroToOne( float value )
{
    if ( value < 0.f )
    {
        return 0.f;
    }
    if ( value > 1.f )
    {
        return 1.f;
    }

    return value;
}

//-----------------------------------------------------------------------------------------------
float Interpolate( float start, float end, float fractionTowardEnd )
{
    return start + fractionTowardEnd * ( end - start );
}

//-----------------------------------------------------------------------------------------------
Vec3 Interpolate( Vec3 const& start, Vec3 const& end, float fractionTowardEnd )
{
    Vec3 result;
    result.x = Interpolate( start.x, end.x, fractionTowardEnd );
    result.y = Interpolate( start.y, end.y, fractionTowardEnd );
    result.z = Interpolate( start.z, end.z, fractionTowardEnd );
    return result;
}

//-----------------------------------------------------------------------------------------------
Vec4 Interpolate( Vec4 const& start, Vec4 const& end, float fractionTowardEnd )
{
    Vec4 result;
    result.x = Interpolate( start.x, end.x, fractionTowardEnd );
    result.y = Interpolate( start.y, end.y, fractionTowardEnd );
    result.z = Interpolate( start.z, end.z, fractionTowardEnd );
    result.w = Interpolate( start.w, end.w, fractionTowardEnd );
    return result;
}

//-----------------------------------------------------------------------------------------------
float GetFractionWithinRange( float value, float rangeStart, float rangeEnd )
{
    if ( rangeStart == rangeEnd )
    {
        return 0.5f;
    }

    return ( value - rangeStart ) / ( rangeEnd - rangeStart );
}

//-----------------------------------------------------------------------------------------------
double GetFractionWithinRange( double value, double rangeStart, double rangeEnd )
{
    if ( rangeStart == rangeEnd )
    {
        return 0.5f;
    }

    return ( value - rangeStart ) / ( rangeEnd - rangeStart );
}

//-----------------------------------------------------------------------------------------------
float RangeMap( float inValue, float inStart, float inEnd, float outStart, float outEnd )
{
    float fraction = GetFractionWithinRange( inValue, inStart, inEnd );
    float lerp     = Interpolate( outStart, outEnd, fraction );

    return lerp;
}

//-----------------------------------------------------------------------------------------------
float RangeMapClamped( float inValue, float inStart, float inEnd, float outStart, float outEnd )
{
    float fraction = GetFractionWithinRange( inValue, inStart, inEnd );
    float lerp     = Interpolate( outStart, outEnd, fraction );

    return outStart > outEnd ? GetClamped( lerp, outEnd, outStart ) : GetClamped( lerp, outStart, outEnd );
}

//-----------------------------------------------------------------------------------------------
int RoundDownToInt( float value )
{
    return static_cast< int >( floorf( value ) );
}

//-----------------------------------------------------------------------------------------------
float ConvertDegreesToRadians( float degrees )
{
    return degrees * ( PI / 180.f );
}

//-----------------------------------------------------------------------------------------------
float ConvertRadiansToDegrees( float radians )
{
    return radians * ( 180.f / PI );
}

//-----------------------------------------------------------------------------------------------
float CosDegrees( float degree )
{
    float radians = ConvertDegreesToRadians( degree );

    return cosf( radians );
}

//-----------------------------------------------------------------------------------------------
float SinDegrees( float degree )
{
    float radians = ConvertDegreesToRadians( degree );

    return sinf( radians );
}

//-----------------------------------------------------------------------------------------------
float Atan2Degrees( float y, float x )
{
    return ConvertRadiansToDegrees( atan2f( y, x ) );
}

//-----------------------------------------------------------------------------------------------
float GetShortestAngularDispDegrees( float startDegrees, float endDegrees )
{
    float displacement = endDegrees - startDegrees;

    while ( displacement > 180 )
    {
        displacement -= 360;
    }
    while ( displacement < -180 )
    {
        displacement += 360;
    }

    return displacement;
}

//-----------------------------------------------------------------------------------------------
float GetTurnedTowardDegrees( float currentDegrees, float goalDegrees, float maxDeltaDegrees )
{
    float angularDisplacement = GetShortestAngularDispDegrees( currentDegrees, goalDegrees );

    if ( angularDisplacement > -maxDeltaDegrees && angularDisplacement < maxDeltaDegrees )
    {
        return goalDegrees;
    }

    return angularDisplacement > 0.0f ? ( currentDegrees + maxDeltaDegrees ) : ( currentDegrees - maxDeltaDegrees );
}

//-----------------------------------------------------------------------------------------------
float DotProduct2D( Vec2 const& a, Vec2 const& b )
{
    return ( a.x * b.x ) + ( a.y * b.y );
}

//-----------------------------------------------------------------------------------------------
float DotProduct3D( Vec3 const& a, Vec3 const& b )
{
    return ( a.x * b.x ) + ( a.y * b.y ) + ( a.z * b.z );
}

//-----------------------------------------------------------------------------------------------
float DotProduct4D( Vec4 const& a, Vec4 const& b )
{
    return ( a.x * b.x ) + ( a.y * b.y ) + ( a.z * b.z ) + ( a.w * b.w );
}

//-----------------------------------------------------------------------------------------------
float CrossProduct2D( Vec2 const& a, Vec2 const& b )
{
    return ( a.x * b.y ) - ( b.x * a.y );
}

//-----------------------------------------------------------------------------------------------
Vec3 CrossProduct3D( Vec3 const& a, Vec3 const& b )
{
    float i = ( a.y * b.z ) - ( a.z * b.y );
    float j = -( ( a.x * b.z ) - ( a.z * b.x ) );
    float k = ( a.x * b.y ) - ( a.y * b.x );

    return Vec3( i, j, k );
}

//-----------------------------------------------------------------------------------------------
float NormalizeByte( unsigned char byteValue )
{
    return static_cast< float >( byteValue ) / 255.f;
}

//-----------------------------------------------------------------------------------------------
unsigned char DenormalizeByte( float zeroToOne )
{
    float byteValue = zeroToOne * 256.f;
    return static_cast< unsigned char >( GetClamped( byteValue, 0.f, 255.f ) );
}

//-----------------------------------------------------------------------------------------------
float GetAngleDegreesBetweenVectors2D( Vec2 const& a, Vec2 const& b )
{
    float aLength = a.GetLength();
    float bLength = b.GetLength();

    float projectionLength  = DotProduct2D( a, b );
    float cosRadiansBetween = projectionLength / ( aLength * bLength );
    cosRadiansBetween       = GetClamped( cosRadiansBetween, -1.f, 1.f );

    float radiansBetween               = acosf( cosRadiansBetween );
    float angleDegreesBetweenVectors2D = ConvertRadiansToDegrees( radiansBetween );

    return angleDegreesBetweenVectors2D;
}

//-----------------------------------------------------------------------------------------------
float GetDistance2D( Vec2 const& positionA, Vec2 const& positionB )
{
    float xDistance = positionB.x - positionA.x;
    float yDistance = positionB.y - positionA.y;

    return sqrtf( ( xDistance * xDistance ) + ( yDistance * yDistance ) );
}

//-----------------------------------------------------------------------------------------------
float GetDistanceSquared2D( Vec2 const& positionA, Vec2 const& positionB )
{
    float xDistance = positionB.x - positionA.x;
    float yDistance = positionB.y - positionA.y;

    return ( xDistance * xDistance ) + ( yDistance * yDistance );
}

//-----------------------------------------------------------------------------------------------
float GetDistance3D( Vec3 const& positionA, Vec3 const& positionB )
{
    float xDistance = positionB.x - positionA.x;
    float yDistance = positionB.y - positionA.y;
    float zDistance = positionB.z - positionA.z;

    return sqrtf( ( xDistance * xDistance ) + ( yDistance * yDistance ) + ( zDistance * zDistance ) );
}

//-----------------------------------------------------------------------------------------------
float GetDistanceSquared3D( Vec3 const& positionA, Vec3 const& positionB )
{
    float xDistance = positionB.x - positionA.x;
    float yDistance = positionB.y - positionA.y;
    float zDistance = positionB.z - positionA.z;

    return ( xDistance * xDistance ) + ( yDistance * yDistance ) + ( zDistance * zDistance );
}

//-----------------------------------------------------------------------------------------------
float GetDistanceXY3D( Vec3 const& positionA, Vec3 const& positionB )
{
    float xDistance = positionB.x - positionA.x;
    float yDistance = positionB.y - positionA.y;

    return sqrtf( ( xDistance * xDistance ) + ( yDistance * yDistance ) );
}

//-----------------------------------------------------------------------------------------------
float GetDistanceXYSquared3D( Vec3 const& positionA, Vec3 const& positionB )
{
    float xDistance = positionB.x - positionA.x;
    float yDistance = positionB.y - positionA.y;

    return ( xDistance * xDistance ) + ( yDistance * yDistance );
}

//-----------------------------------------------------------------------------------------------
Vec2 GetDirection2D( Vec3 const& direction3D )
{
    return Vec2( direction3D.x, direction3D.y );
}

//-----------------------------------------------------------------------------------------------
int GetTaxicabDistance2D( IntVec2 const& pointA, IntVec2 const& pointB )
{
    return abs( pointA.x - pointB.x ) + abs( pointA.y - pointB.y );
}

//-----------------------------------------------------------------------------------------------
float GetProjectedLength2D( Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto )
{
    Vec2 const  projectionDirection = vectorToProjectOnto.GetNormalized();
    float const projectedLength     = DotProduct2D( vectorToProject, projectionDirection );

    return projectedLength;
}

//-----------------------------------------------------------------------------------------------
Vec2 GetProjectedVector2D( Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto )
{
    Vec2 const  projectionDirection = vectorToProjectOnto.GetNormalized();
    float const projectedLength     = DotProduct2D( vectorToProject, projectionDirection );
    Vec2 const  projectedVector     = projectionDirection * projectedLength;

    return projectedVector;
}

//-----------------------------------------------------------------------------------------------
bool DoDiscsOverlap( Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB )
{
    float distanceBetweenDiscsSquared = GetDistanceSquared2D( centerA, centerB );
    float radiiSumSquared             = ( radiusA + radiusB ) * ( radiusA + radiusB );

    return distanceBetweenDiscsSquared < radiiSumSquared;
}

//-----------------------------------------------------------------------------------------------
bool DoSpheresOverlap( Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB )
{
    float distanceSquared = GetDistanceSquared3D( centerA, centerB );
    float radiiSumSquared = ( radiusA + radiusB ) * ( radiusA + radiusB );

    return distanceSquared < radiiSumSquared;
}

//-----------------------------------------------------------------------------------------------
bool DoZAlignedCapsulesOverlap3D( Vec3 centerA, float radiusA, float halfHeightA, Vec3 centerB, float radiusB, float halfHeightB )
{
    Vec3  boneStartA = Vec3( centerA.x, centerA.y, centerA.z - halfHeightA + radiusA );
    Vec3  boneEndA   = Vec3( centerA.x, centerA.y, centerA.z + halfHeightA - radiusA );

    Vec3  boneStartB = Vec3( centerB.x, centerB.y, centerB.z - halfHeightB + radiusB );
    Vec3  boneEndB   = Vec3( centerB.x, centerB.y, centerB.z + halfHeightB - radiusB );

    float radiiSumSquared = ( radiusA + radiusB ) * ( radiusA + radiusB );
    if ( boneEndA.z <= boneStartB.z )
    {
        if ( GetDistanceSquared3D( boneEndA, boneStartB ) >= radiiSumSquared )
        {
            return false;
        }
    }
    else if ( boneStartA.z >= boneEndB.z )
    {
        if ( GetDistanceSquared3D( boneStartA, boneEndB ) >= radiiSumSquared )
        {
            return false;
        }
    }
    else if ( GetDistanceSquared2D( Vec2( centerA.x, centerA.y ), Vec2( centerB.x, centerB.y ) ) >= radiiSumSquared )
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------------------------
bool PushCapsuleOutOfFixedCapsule3D( Vec3& centerA, float radiusA, float boneHalfHeightA, Vec3 const& centerB, float radiusB, float boneHalfHeightB )
{
    if ( !DoZAlignedCapsulesOverlap3D( centerA, radiusA, boneHalfHeightA, centerB, radiusB, boneHalfHeightB ) )
    {
        return false;
    }

    Vec2       mobileCapsuleCenterXY = Vec2( centerA.x, centerA.y );
    Vec2       fixedCapsuleCenterXY  = Vec2( centerB.x, centerB.y );

    float      distanceBetweenCenters = GetDistance2D( mobileCapsuleCenterXY, fixedCapsuleCenterXY );
    float      sumOfRadii             = radiusA + radiusB;

    float      overlapDistance = sumOfRadii - distanceBetweenCenters;

    Vec2 const fixedToMobileVector     = mobileCapsuleCenterXY - fixedCapsuleCenterXY;
    Vec2 const normalizedFixedToMobile = fixedToMobileVector.GetNormalized();

    mobileCapsuleCenterXY += normalizedFixedToMobile * overlapDistance;
    centerA.x = mobileCapsuleCenterXY.x;
    centerA.y = mobileCapsuleCenterXY.y;

    return true;
}

//-----------------------------------------------------------------------------------------------
bool DoSphereAndOBB3Overlap( Vec3 const& centerA, float radiusA, OBB3 const& orientedBox )
{
    Vec3 nearestPoint = GetNearestPointOnOBB3D( centerA, orientedBox );
    return IsPointInsideSphere3D( nearestPoint, centerA, radiusA );
}

//-----------------------------------------------------------------------------------------------
bool DoSphereAndPlane3Overlap( Vec3 const& centerA, float radiusA, Plane3 const& plane )
{
    Vec3 nearestPoint = GetNearestPointOnPlane3( centerA, plane );
    return IsPointInsideSphere3D( nearestPoint, centerA, radiusA );
}

//-----------------------------------------------------------------------------------------------
bool DoAABBAndPlaneOverlap3D( AABB3 const& alignedBox, Plane3 const& plane )
{
    Vec3 corners[ 8 ] = {
        Vec3( alignedBox.m_mins.x, alignedBox.m_mins.y, alignedBox.m_mins.z ),
        Vec3( alignedBox.m_maxs.x, alignedBox.m_mins.y, alignedBox.m_mins.z ),
        Vec3( alignedBox.m_mins.x, alignedBox.m_maxs.y, alignedBox.m_mins.z ),
        Vec3( alignedBox.m_maxs.x, alignedBox.m_maxs.y, alignedBox.m_mins.z ),

        Vec3( alignedBox.m_mins.x, alignedBox.m_mins.y, alignedBox.m_maxs.z ),
        Vec3( alignedBox.m_maxs.x, alignedBox.m_mins.y, alignedBox.m_maxs.z ),
        Vec3( alignedBox.m_mins.x, alignedBox.m_maxs.y, alignedBox.m_maxs.z ),
        Vec3( alignedBox.m_maxs.x, alignedBox.m_maxs.y, alignedBox.m_maxs.z )
    };

    bool hasPositive = false;
    bool hasNegative = false;

    for ( int i = 0; i < 8; ++i )
    {
        float dist = DotProduct3D( corners[ i ], plane.m_normal ) - plane.m_distAlongNormalFromOrigin;
        if ( dist > 0.f )
        {
            hasPositive = true;
        }
        else
        {
            hasNegative = true;
        }
    }

    return hasPositive && hasNegative;
}

//------------------------------------------------------------------------	-----------------------
bool DoOBBAndPlaneOverlap3D( OBB3 const& orientedBox, Plane3 const& plane )
{
    Vec3  iBasis = orientedBox.m_iBasisNormal;
    Vec3  jBasis = orientedBox.m_jBasisNormal;
    Vec3  kBasis = orientedBox.m_kBasisNormal;

    float halfX = orientedBox.m_halfDimensions.x;
    float halfY = orientedBox.m_halfDimensions.y;
    float halfZ = orientedBox.m_halfDimensions.z;

    Vec3  TBL = orientedBox.m_center + ( iBasis * -halfX ) + ( kBasis * halfZ ) + ( jBasis * halfY );
    Vec3  TBR = orientedBox.m_center + ( iBasis * -halfX ) + ( kBasis * halfZ ) + ( jBasis * -halfY );
    Vec3  TTR = orientedBox.m_center + ( iBasis * halfX ) + ( kBasis * halfZ ) + ( jBasis * -halfY );
    Vec3  TTL = orientedBox.m_center + ( iBasis * halfX ) + ( kBasis * halfZ ) + ( jBasis * halfY );

    Vec3  BBL = orientedBox.m_center + ( iBasis * -halfX ) + ( kBasis * -halfZ ) + ( jBasis * halfY );
    Vec3  BBR = orientedBox.m_center + ( iBasis * -halfX ) + ( kBasis * -halfZ ) + ( jBasis * -halfY );
    Vec3  BTR = orientedBox.m_center + ( iBasis * halfX ) + ( kBasis * -halfZ ) + ( jBasis * -halfY );
    Vec3  BTL = orientedBox.m_center + ( iBasis * halfX ) + ( kBasis * -halfZ ) + ( jBasis * halfY );

    Vec3  corners[ 8 ] = { TBL, TBR, TTR, TTL, BBL, BBR, BTR, BTL };

    bool  hasPositive = false;
    bool  hasNegative = false;

    for ( int i = 0; i < 8; ++i )
    {
        float dist = DotProduct3D( corners[ i ], plane.m_normal ) - plane.m_distAlongNormalFromOrigin;
        if ( dist > 0.f )
        {
            hasPositive = true;
        }
        else
        {
            hasNegative = true;
        }
    }

    return hasPositive && hasNegative;
}

//-----------------------------------------------------------------------------------------------
bool DiscBounceOffDisc( Vec2& discACenter, float discARadius, Vec2& discAVelocity, float discAElasticity, Vec2& discBCenter, float discBRadius, Vec2& discBVelocity, float discBElasticity )
{
    if ( !DoDiscsOverlap( discACenter, discARadius, discBCenter, discBRadius ) )
    {
        return false;
    }

    PushDiscsOutOfEachOther2D( discACenter, discARadius, discBCenter, discBRadius );

    Vec2  normal = ( discACenter - discBCenter ).GetNormalized();

    Vec2  relativeVelocity    = discAVelocity - discBVelocity;
    float relativeNormalSpeed = DotProduct2D( relativeVelocity, normal );

    if ( relativeNormalSpeed > 0.0f )
    {
        return false;
    }

    float totalElasticity = discAElasticity * discBElasticity;

    float aNormalScalar = DotProduct2D( discAVelocity, normal );
    float bNormalScalar = DotProduct2D( discBVelocity, normal );

    Vec2  aNormalVec  = normal * aNormalScalar;
    Vec2  aTangentVec = discAVelocity - aNormalVec;

    Vec2  bNormalVec  = normal * bNormalScalar;
    Vec2  bTangentVec = discBVelocity - bNormalVec;

    float aNewNormalScalar = bNormalScalar * totalElasticity;
    float bNewNormalScalar = aNormalScalar * totalElasticity;

    discAVelocity = aTangentVec + ( normal * aNewNormalScalar );
    discBVelocity = bTangentVec + ( normal * bNewNormalScalar );

    return true;
}

//-----------------------------------------------------------------------------------------------
bool DoAABB2DsOverlap( AABB2 const& alignedBoxA, AABB2 const& alignedBoxB )
{
    if ( alignedBoxA.m_maxs.x < alignedBoxB.m_mins.x || alignedBoxA.m_mins.x > alignedBoxB.m_maxs.x )
    {
        return false;
    }

    if ( alignedBoxA.m_mins.y > alignedBoxB.m_maxs.y || alignedBoxA.m_maxs.y < alignedBoxB.m_mins.y )
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------------------------
bool DoDiscAndAABBOverlap2D( Vec2 const& center, float radius, AABB2 const& box )
{
    Vec2 nearestPos = GetNearestPointOnAABB2D( center, box );
    return IsPointInsideDisc2D( nearestPos, center, radius );
}

//-----------------------------------------------------------------------------------------------
bool DoZCylindersOverlap3D( Vec2 cylinder1CenterXY, float cylinder1Radius, FloatRange cylinder1MinMaxZ, Vec2 cylinder2CenterXY, float cylinder2Radius, FloatRange cylinder2MinMaxZ )
{
    if ( cylinder1MinMaxZ.m_min >= cylinder2MinMaxZ.m_max || cylinder1MinMaxZ.m_max <= cylinder2MinMaxZ.m_min )
    {
        return false;
    }

    if ( !DoDiscsOverlap( cylinder1CenterXY, cylinder1Radius, cylinder2CenterXY, cylinder2Radius ) )
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------------------------
bool DoSphereAndAABBOverlap3D( Vec3 sphereCenter, float sphereRadius, AABB3 box )
{
    Vec3 nearestPoint = GetNearestPointOnAABB3D( sphereCenter, box );
    return IsPointInsideSphere3D( nearestPoint, sphereCenter, sphereRadius );
}

//-----------------------------------------------------------------------------------------------
bool DoZCylinderAndAABBOverlap3D( Vec2 cylinderCenterXY, float cylinderRadius, FloatRange cylinderMinMaxZ, AABB3 box )
{
    if ( cylinderMinMaxZ.m_min >= box.m_maxs.z || cylinderMinMaxZ.m_max <= box.m_mins.z )
    {
        return false;
    }

    AABB2 box2D = AABB2( Vec2( box.m_mins.x, box.m_mins.y ), Vec2( box.m_maxs.x, box.m_maxs.y ) );
    return DoDiscAndAABBOverlap2D( cylinderCenterXY, cylinderRadius, box2D );
}

//-----------------------------------------------------------------------------------------------
bool DoZCylinderAndSphereOverlap3D( Vec2 cylinderCenterXY, float cylinderRadius, FloatRange cylinderMinMaxZ, Vec3 sphereCenter, float sphereRadius )
{
    Vec3 nearestPoint = GetNearestPointOnCylinderZ( sphereCenter, cylinderCenterXY, cylinderRadius, cylinderMinMaxZ );
    return IsPointInsideSphere3D( nearestPoint, sphereCenter, sphereRadius );
}

//-----------------------------------------------------------------------------------------------
bool DoAABBsOverlap3D( AABB3 const& first, AABB3 const& second )
{
    if ( second.m_maxs.x <= first.m_mins.x || second.m_mins.x >= first.m_maxs.x )
    {
        return false;
    }

    if ( second.m_maxs.y <= first.m_mins.y || second.m_mins.y >= first.m_maxs.y )
    {
        return false;
    }

    if ( second.m_maxs.z <= first.m_mins.z || second.m_mins.z >= first.m_maxs.z )
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------------------------
RaycastResult3D RaycastVsCylinderZ3D_BruteForce( Vec3 const& start, Vec3 const& direction, float distance, Vec2 const& center, float minZ, float maxZ, float radius )
{
    RaycastResult3D result;
    Vec2            startXY = Vec2( start.x, start.y );
    if ( IsPointInsideOrOnDisc2D( startXY, center, radius ) && start.z <= maxZ && start.z >= minZ )
    {
        result.m_didImpact      = true;
        result.m_impactPosition = start;
        return result;
    }

    float tMin      = FLT_MAX;
    Vec3  impactPos = Vec3::ZERO;

    float tMaxZ = ( maxZ - start.z ) / direction.z;
    impactPos   = start + direction * tMaxZ;
    if ( IsPointInsideOrOnDisc2D( Vec2( impactPos.x, impactPos.y ), center, radius ) )
    {
        tMin = tMaxZ < tMin ? tMaxZ : tMin;
    }

    float tMinZ = ( minZ - start.z ) / direction.z;
    impactPos   = start + direction * tMinZ;
    if ( IsPointInsideOrOnDisc2D( Vec2( impactPos.x, impactPos.y ), center, radius ) )
    {
        tMin = tMinZ < tMin ? tMinZ : tMin;
    }

    RaycastResult2D xyResult = RaycastVsDisc2D( startXY, Vec2( direction.x, direction.y ), distance, center, radius );
    result.m_impactPosition  = start + ( direction * xyResult.m_impactDist );
    result.m_impactNormal    = ( start - result.m_impactPosition ).GetNormalized();

    return result;
}

//-----------------------------------------------------------------------------------------------
RaycastResult3D RaycastVsPlane3D( Vec3 rayStart, Vec3 rayForwardNormal, float rayLength, Plane3 plane )
{
    RaycastResult3D result;

    // perpendicular - miss
    if ( DotProduct3D( rayForwardNormal, plane.m_normal ) == 0.f )
    {
        return result;
    }

    float distToPlane                = DotProduct3D( rayStart, plane.m_normal );
    float rayDirectionDotPlaneNormal = DotProduct3D( rayForwardNormal, plane.m_normal );
    float remainingDist              = plane.m_distAlongNormalFromOrigin - distToPlane;

    if ( remainingDist > 0.f )  // font
    {
        // opposite direction - miss
        if ( rayDirectionDotPlaneNormal < 0.f )
        {
            return result;
        }
    }
    else if ( remainingDist < 0.f )  // back
    {
        // same direction - miss
        if ( rayDirectionDotPlaneNormal > 0.f )
        {
            return result;
        }
    }

    float impactDistance = remainingDist / rayDirectionDotPlaneNormal;
    if ( impactDistance > rayLength )
    {
        return result;
    }

    result.m_didImpact        = true;
    result.m_impactDistance   = impactDistance;
    result.m_impactPosition   = rayStart + rayForwardNormal * result.m_impactDistance;
    result.m_impactNormal     = rayDirectionDotPlaneNormal < 0.f ? plane.m_normal : -plane.m_normal;
    result.m_rayLength        = rayLength;
    result.m_rayDirection     = rayForwardNormal;
    result.m_rayStartPosition = rayStart;

    return result;
}

//-----------------------------------------------------------------------------------------------
RaycastResult3D RaycastVsOBB3( Vec3 rayStart, Vec3 rayForwardNormal, float rayLength, OBB3 orientedBox )
{
    RaycastResult3D result;
    Vec3            rayStartRelativeToOBB = rayStart - orientedBox.m_center;
    Vec3            localRayStart         = Vec3(
        DotProduct3D( rayStartRelativeToOBB, orientedBox.m_iBasisNormal ),
        DotProduct3D( rayStartRelativeToOBB, orientedBox.m_jBasisNormal ),
        DotProduct3D( rayStartRelativeToOBB, orientedBox.m_kBasisNormal ) );

    Vec3 localRayForward = Vec3(
        DotProduct3D( rayForwardNormal, orientedBox.m_iBasisNormal ),
        DotProduct3D( rayForwardNormal, orientedBox.m_jBasisNormal ),
        DotProduct3D( rayForwardNormal, orientedBox.m_kBasisNormal ) );

    AABB3           localAABB   = AABB3( -orientedBox.m_halfDimensions, orientedBox.m_halfDimensions );
    RaycastResult3D localResult = RaycastVsAABB3D(
        localRayStart,
        localRayForward,
        rayLength,
        localAABB );

    if ( !localResult.m_didImpact )
    {
        return result;
    }

    result.m_impactDistance = localResult.m_impactDistance;
    result.m_impactPosition = orientedBox.m_center +
                              orientedBox.m_iBasisNormal * localResult.m_impactPosition.x +
                              orientedBox.m_jBasisNormal * localResult.m_impactPosition.y +
                              orientedBox.m_kBasisNormal * localResult.m_impactPosition.z;

    result.m_impactNormal =
        orientedBox.m_iBasisNormal * localResult.m_impactNormal.x +
        orientedBox.m_jBasisNormal * localResult.m_impactNormal.y +
        orientedBox.m_kBasisNormal * localResult.m_impactNormal.z;

    result.m_didImpact        = true;
    result.m_rayLength        = rayLength;
    result.m_rayDirection     = rayForwardNormal;
    result.m_rayStartPosition = rayStart;
    return result;
}

//-----------------------------------------------------------------------------------------------
Vec3 GetNearestPointOnAABB3D( Vec3 const& referencePos, AABB3 const& alignedBox )
{
    float      nearestX = GetClamped( referencePos.x, alignedBox.m_mins.x, alignedBox.m_maxs.x );
    float      nearestY = GetClamped( referencePos.y, alignedBox.m_mins.y, alignedBox.m_maxs.y );
    float      nearestZ = GetClamped( referencePos.z, alignedBox.m_mins.z, alignedBox.m_maxs.z );

    Vec3 const nearestPoint = Vec3( nearestX, nearestY, nearestZ );

    return nearestPoint;
}

//-----------------------------------------------------------------------------------------------
Vec3 GetNearestPointOnCylinderZ( Vec3 const& referencePos, Vec2 const& centerXY, float radius, FloatRange zMinMax )
{
    if ( IsPointInsideCylinderZ( referencePos, centerXY, radius, zMinMax ) )
    {
        return referencePos;
    }

    Vec2  referencePosXY = Vec2( referencePos.x, referencePos.y );

    float nearestPointZ  = GetClamped( referencePos.z, zMinMax.m_min, zMinMax.m_max );
    Vec2  nearestPointXY = GetNearestPointOnDisc2D( referencePosXY, centerXY, radius );

    return Vec3( nearestPointXY.x, nearestPointXY.y, nearestPointZ );
}

//-----------------------------------------------------------------------------------------------
bool IsPointInsideOrOnDisc2D( Vec2 const& point, Vec2 const& discCenter, float discRadius )
{
    float displacement = GetDistance2D( point, discCenter );

    if ( displacement <= discRadius )
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------------------------
bool IsPointInsideAABB2D( Vec2 point, AABB2 const& alignedBox )
{
    return alignedBox.IsPointInside( point );
}

bool IsPointInsideOBB2D( Vec2 point, OBB2 const& orientedBox )
{
    Vec2  cp     = point - orientedBox.m_center;
    float localX = DotProduct2D( cp, orientedBox.m_iBasisNormal );
    float localY = DotProduct2D( cp, ( orientedBox.m_iBasisNormal ).GetRotatedByMinus90Degrees() );

    if ( localX >= orientedBox.m_halfDimensions.x )
    {
        return false;
    }

    if ( localX <= -orientedBox.m_halfDimensions.x )
    {
        return false;
    }

    if ( localY >= orientedBox.m_halfDimensions.y )
    {
        return false;
    }

    if ( localY <= -orientedBox.m_halfDimensions.y )
    {
        return false;
    }

    return true;
}

bool IsPointInsideCapsule2D( Vec2 point, Vec2 boneStart, Vec2 boneEnd, float radius )
{
    Vec2  se = boneEnd - boneStart;
    Vec2  sp = point - boneStart;
    Vec2  ep = point - boneEnd;

    float distanceSP = GetDistanceSquared2D( boneStart, point );
    float distanceEP = GetDistanceSquared2D( boneEnd, point );

    float radiusSquared = radius * radius;

    float dot = DotProduct2D( sp, se );

    if ( dot < 0.f && distanceSP > radiusSquared )
    {
        return false;
    }

    dot = DotProduct2D( ep, se );

    if ( dot > 0.f && distanceEP > radiusSquared )
    {
        return false;
    }

    Vec2  projectedPoint = boneStart + GetProjectedVector2D( sp, se );

    float squaredDistance = GetDistanceSquared2D( projectedPoint, point );

    if ( squaredDistance > radiusSquared )
    {
        return false;
    }

    return true;
}

bool IsPointInsideTriangle2D( Vec2 point, Vec2 ccw0, Vec2 ccw1, Vec2 ccw2 )
{
    Vec2  directionAtoP           = point - ccw0;
    Vec2  directionAtoB           = ccw1 - ccw0;
    Vec2  rotatedMinus90DegreesAB = directionAtoB.GetRotatedByMinus90Degrees();

    float dot = DotProduct2D( directionAtoP, rotatedMinus90DegreesAB );

    if ( dot > 0.f )
    {
        return false;
    }

    Vec2 directionBtoP           = point - ccw1;
    Vec2 directionBtoC           = ccw2 - ccw1;
    Vec2 rotatedMinus90DegreesBC = directionBtoC.GetRotatedByMinus90Degrees();
    dot                          = DotProduct2D( directionBtoP, rotatedMinus90DegreesBC );

    if ( dot > 0.f )
    {
        return false;
    }

    Vec2 directionCtoP           = point - ccw2;
    Vec2 directionCtoA           = ccw0 - ccw2;
    Vec2 rotatedMinus90DegreesCA = directionCtoA.GetRotatedByMinus90Degrees();
    dot                          = DotProduct2D( directionCtoP, rotatedMinus90DegreesCA );

    if ( dot > 0.f )
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------------------------
bool IsPointInsideOrientedSector2D( Vec2 const& point, Vec2 const& sectorTip, float sectorFwdDegrees, float sectorApertureDegrees, float sectorRadius )
{
    if ( !IsPointInsideDisc2D( point, sectorTip, sectorRadius ) )
    {
        return false;
    }

    Vec2 const sectorForward    = Vec2::MakeFromPolarDegrees( sectorFwdDegrees );
    Vec2 const vectorToPoint    = point - sectorTip;
    float      angleFromForward = GetAngleDegreesBetweenVectors2D( vectorToPoint, sectorForward );

    return angleFromForward <= sectorApertureDegrees * 0.5f;
}

//-----------------------------------------------------------------------------------------------
bool IsPointInsideDirectedSector2D( Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorFwdNormal, float sectorApertureDegrees, float sectorRadius )
{
    if ( !IsPointInsideDisc2D( point, sectorTip, sectorRadius ) )
    {
        return false;
    }

    Vec2 const vectorToPoint    = point - sectorTip;
    float      angleFromForward = GetAngleDegreesBetweenVectors2D( vectorToPoint, sectorFwdNormal );

    return angleFromForward <= sectorApertureDegrees * 0.5f;
}

//-----------------------------------------------------------------------------------------------
bool IsPointInsideDisc2D( Vec2 const& point, Vec2 const& discCenter, float discRadius )
{
    float displacement = GetDistance2D( point, discCenter );

    if ( displacement < discRadius )
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------------------------
bool IsPointInsideSphere3D( Vec3 const& point, Vec3 const& sphereCenter, float sphereRadius )
{
    float squaredDistance = GetDistanceSquared3D( point, sphereCenter );
    return ( squaredDistance < sphereRadius * sphereRadius );
}

//-----------------------------------------------------------------------------------------------
bool IsPointInsideAABB3D( Vec3 const& point, AABB3 const& alignedBox )
{
    bool isInsideX = ( alignedBox.m_mins.x < point.x ) && ( point.x < alignedBox.m_maxs.x );
    bool isInsideY = ( alignedBox.m_mins.y < point.y ) && ( point.y < alignedBox.m_maxs.y );
    bool isInsideZ = ( alignedBox.m_mins.z < point.z ) && ( point.z < alignedBox.m_maxs.z );

    return isInsideX && isInsideY && isInsideZ;
}

//-----------------------------------------------------------------------------------------------
bool IsPointInsideCylinderZ( Vec3 const& point, Vec2 const& centerXY, float radius, FloatRange zMinMax )
{
    if ( point.z <= zMinMax.m_min || point.z >= zMinMax.m_max )
    {
        return false;
    }

    Vec2 pointXY = Vec2( point.x, point.y );
    return IsPointInsideDisc2D( pointXY, centerXY, radius );
}

//-----------------------------------------------------------------------------------------------
bool IsPointInsideOBB3D( Vec3 point, OBB3 const& orientedBox )
{
    Vec3  cp     = point - orientedBox.m_center;
    float localX = DotProduct3D( cp, orientedBox.m_iBasisNormal );
    float localY = DotProduct3D( cp, orientedBox.m_jBasisNormal );
    float localZ = DotProduct3D( cp, orientedBox.m_kBasisNormal );

    if ( localX >= orientedBox.m_halfDimensions.x )
    {
        return false;
    }

    if ( localX <= -orientedBox.m_halfDimensions.x )
    {
        return false;
    }

    if ( localY >= orientedBox.m_halfDimensions.y )
    {
        return false;
    }

    if ( localY <= -orientedBox.m_halfDimensions.y )
    {
        return false;
    }

    if ( localZ >= orientedBox.m_halfDimensions.z )
    {
        return false;
    }

    if ( localZ <= -orientedBox.m_halfDimensions.z )
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnDisc2D( Vec2 const& referencePos, Vec2 const& discCenter, float discRadius )
{
    float distanceToCenter = GetDistance2D( referencePos, discCenter );

    if ( distanceToCenter <= discRadius )
    {
        return referencePos;
    }

    Vec2 const normalizedCenterToRefVector = ( referencePos - discCenter ).GetNormalized();

    return discCenter + normalizedCenterToRefVector * discRadius;
}

//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnAABB2D( Vec2 referencePos, AABB2 const& alignedBox )
{
    return alignedBox.GetNearestPoint( referencePos );
}

//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnOBB2D( Vec2 const& referencePos, OBB2 const& orientedBox )
{
    if ( IsPointInsideOBB2D( referencePos, orientedBox ) )
    {
        return referencePos;
    }

    Vec2  cp = referencePos - orientedBox.m_center;

    Vec2  iBasis = orientedBox.m_iBasisNormal;
    Vec2  jBasis = iBasis.GetRotatedByMinus90Degrees();

    float localX = DotProduct2D( cp, iBasis );
    float localY = DotProduct2D( cp, jBasis );

    localX = GetClamped( localX, -orientedBox.m_halfDimensions.x, orientedBox.m_halfDimensions.x );
    localY = GetClamped( localY, -orientedBox.m_halfDimensions.y, orientedBox.m_halfDimensions.y );

    return orientedBox.m_center + localX * iBasis + localY * jBasis;
}

//-----------------------------------------------------------------------------------------------
Vec3 GetNearestPointOnOBB3D( Vec3 const& referencePos, OBB3 const& orientedBox )
{
    if ( IsPointInsideOBB3D( referencePos, orientedBox ) )
    {
        return referencePos;
    }

    Vec3  cp = referencePos - orientedBox.m_center;

    Vec3  iBasis = orientedBox.m_iBasisNormal;
    Vec3  jBasis = orientedBox.m_jBasisNormal;
    Vec3  kBasis = orientedBox.m_kBasisNormal;

    float localX = DotProduct3D( cp, iBasis );
    float localY = DotProduct3D( cp, jBasis );
    float localZ = DotProduct3D( cp, kBasis );

    localX = GetClamped( localX, -orientedBox.m_halfDimensions.x, orientedBox.m_halfDimensions.x );
    localY = GetClamped( localY, -orientedBox.m_halfDimensions.y, orientedBox.m_halfDimensions.y );
    localZ = GetClamped( localZ, -orientedBox.m_halfDimensions.z, orientedBox.m_halfDimensions.z );

    return orientedBox.m_center + localX * iBasis + localY * jBasis + localZ * kBasis;
}

//-----------------------------------------------------------------------------------------------
Vec3 GetNearestPointOnPlane3( Vec3 const& referencePos, Plane3 const& plane )
{
    Vec3  originToPoint = referencePos - Vec3::ZERO;
    float distToPlane   = DotProduct3D( originToPoint, plane.m_normal );
    return referencePos + plane.m_normal * ( plane.m_distAlongNormalFromOrigin - distToPlane );
}

//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnInfiniteLine2D( Vec2 referencePos, Vec2 pointOnLine, Vec2 anotherPointOnLine )
{
    Vec2 se = anotherPointOnLine - pointOnLine;
    Vec2 sp = referencePos - pointOnLine;
    Vec2 sn = GetProjectedVector2D( sp, se );

    return pointOnLine + sn;
}

//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnLineSegment2D( Vec2 referencePos, Vec2 start, Vec2 end )
{
    Vec2  deltaSE = end - start;
    Vec2  deltaSP = referencePos - start;
    Vec2  deltaEP = referencePos - end;

    float angleBetweenSEAndSP = DotProduct2D( deltaSE, deltaSP );

    if ( angleBetweenSEAndSP <= 0.f )
    {
        return start;
    }

    float angleBetweenSEndAndEP = DotProduct2D( deltaSE, deltaEP );

    if ( angleBetweenSEndAndEP >= 0.f )
    {
        return end;
    }

    return start + GetProjectedVector2D( deltaSP, deltaSE );
}

Vec2 GetNearestPointOnCapsule2D( Vec2 referencePos, Vec2 boneStart, Vec2 boneEnd, float radius )
{
    if ( IsPointInsideCapsule2D( referencePos, boneStart, boneEnd, radius ) )
    {
        return referencePos;
    }

    Vec2  se = boneEnd - boneStart;
    Vec2  sp = referencePos - boneStart;
    Vec2  ep = referencePos - boneEnd;

    float dot = DotProduct2D( sp, se );

    if ( dot < 0.f )
    {
        Vec2 normalizedSP = sp.GetNormalized();
        return boneStart + normalizedSP * radius;
    }

    dot = DotProduct2D( ep, se );

    if ( dot > 0.f )
    {
        Vec2 normalizedEP = ep.GetNormalized();
        return boneEnd + normalizedEP * radius;
    }

    Vec2 projectedPoint = boneStart + GetProjectedVector2D( sp, se );
    Vec2 dir            = ( referencePos - projectedPoint ).GetNormalized();
    return projectedPoint + dir * radius;
}

//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnTriangle2D( Vec2 referencePos, Vec2 ccw0, Vec2 ccw1, Vec2 ccw2 )
{
    if ( IsPointInsideTriangle2D( referencePos, ccw0, ccw1, ccw2 ) )
    {
        return referencePos;
    }

    Vec2  nearestPointOnAB = GetNearestPointOnLineSegment2D( referencePos, ccw1, ccw0 );
    Vec2  nearestPointOnBC = GetNearestPointOnLineSegment2D( referencePos, ccw2, ccw1 );
    Vec2  nearestPointOnCA = GetNearestPointOnLineSegment2D( referencePos, ccw0, ccw2 );

    float distSqToAB = GetDistanceSquared2D( referencePos, nearestPointOnAB );
    float distSqToBC = GetDistanceSquared2D( referencePos, nearestPointOnBC );
    float distSqToCA = GetDistanceSquared2D( referencePos, nearestPointOnCA );

    if ( distSqToAB <= distSqToBC && distSqToAB <= distSqToCA )
    {
        return nearestPointOnAB;
    }
    else if ( distSqToBC <= distSqToCA && distSqToBC <= distSqToAB )
    {
        return nearestPointOnBC;
    }
    else
    {
        return nearestPointOnCA;
    }
}

//-----------------------------------------------------------------------------------------------
Vec3 GetNearestPointOnSphere3D( Vec3 const& referencePos, Vec3 const& sphereCenter, float sphereRadius )
{
    if ( IsPointInsideSphere3D( referencePos, sphereCenter, sphereRadius ) )
    {
        return referencePos;
    }

    Vec3 directionToPos = ( referencePos - sphereCenter ).GetNormalized();
    Vec3 nearestPoint   = sphereCenter + directionToPos * sphereRadius;

    return nearestPoint;
}

//-----------------------------------------------------------------------------------------------
bool PushDiscOutOfFixedPoint2D( Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedPoint )
{
    if ( !IsPointInsideDisc2D( fixedPoint, mobileDiscCenter, discRadius ) )
    {
        return false;
    }

    Vec2 const fixedToMobileVector     = mobileDiscCenter - fixedPoint;
    Vec2 const normalizedFixedToMobile = fixedToMobileVector.GetNormalized();

    float      fixedToMobileDistance = GetDistance2D( fixedPoint, mobileDiscCenter );
    float      overlapDistance       = discRadius - fixedToMobileDistance;

    mobileDiscCenter += normalizedFixedToMobile * overlapDistance;

    return true;
}

//-----------------------------------------------------------------------------------------------
bool PushDiscOutOfFixedDisc2D( Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius )
{
    if ( !DoDiscsOverlap( mobileDiscCenter, discRadius, fixedDiscCenter, fixedDiscRadius ) )
    {
        return false;
    }

    float      distanceBetweenCenters = GetDistance2D( mobileDiscCenter, fixedDiscCenter );
    float      sumOfRadii             = discRadius + fixedDiscRadius;

    float      overlapDistance = sumOfRadii - distanceBetweenCenters;

    Vec2 const fixedToMobileVector     = mobileDiscCenter - fixedDiscCenter;
    Vec2 const normalizedFixedToMobile = fixedToMobileVector.GetNormalized();

    mobileDiscCenter += normalizedFixedToMobile * overlapDistance;

    return true;
}

//-----------------------------------------------------------------------------------------------
bool PushDiscsOutOfEachOther2D( Vec2& aCenter, float aRadius, Vec2& bCenter, float bRadius )
{
    if ( !DoDiscsOverlap( aCenter, aRadius, bCenter, bRadius ) )
    {
        return false;
    }

    float      distanceBetweenCenters = GetDistance2D( aCenter, bCenter );
    float      sumOfRadii             = aRadius + bRadius;

    float      overlapDistance = sumOfRadii - distanceBetweenCenters;

    Vec2 const bToAVector           = aCenter - bCenter;
    Vec2 const normalizedbToAVector = bToAVector.GetNormalized();

    aCenter += normalizedbToAVector * ( overlapDistance * 0.5f );
    bCenter += -normalizedbToAVector * ( overlapDistance * 0.5f );

    return true;
}

//-----------------------------------------------------------------------------------------------
bool PushDiscOutOfFixedAABB2D( Vec2& mobileDiscCenter, float discRadius, AABB2 const& fixedBox )
{
    Vec2 const nearestPoint = fixedBox.GetNearestPoint( mobileDiscCenter );

    Vec2 const nearestToMobileVec = mobileDiscCenter - nearestPoint;
    float      distance           = nearestToMobileVec.GetLength();

    if ( distance < discRadius )
    {
        Vec2 pushDirection;
        if ( distance > 0.f )
        {
            pushDirection = nearestToMobileVec.GetNormalized();
        }
        float overlapDistance = discRadius - distance;
        mobileDiscCenter += pushDirection * overlapDistance;

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------------------------
bool PushZCylinderOutofEachOther3D( Vec2& zCylinderACenterXY, float zCylinderARadiusXY, FloatRange zCylinderAMinMaxZ, Vec2& zCylinderBCenterXY, float zCylinderBRadiusXY, FloatRange zCylinderBMinMaxZ )
{
    if ( DoZCylindersOverlap3D( zCylinderACenterXY, zCylinderARadiusXY, zCylinderAMinMaxZ, zCylinderBCenterXY, zCylinderBRadiusXY, zCylinderBMinMaxZ ) )
    {
        return PushDiscsOutOfEachOther2D( zCylinderACenterXY, zCylinderARadiusXY, zCylinderBCenterXY, zCylinderBRadiusXY );
    }

    return false;
}

//-----------------------------------------------------------------------------------------------
bool PushZCylinderOutofFixedZCylinder3D( Vec2& mobileZCylinderCenterXY, float mobileZCylinderRadiusXY, FloatRange mobileZCylinderMinMaxZ, Vec2 const& fixedZCylinderCenterXY, float fixedZCylinderRadiusXY, FloatRange fixedZCylinderMinMaxZ )
{
    if ( DoZCylindersOverlap3D( mobileZCylinderCenterXY, mobileZCylinderRadiusXY, mobileZCylinderMinMaxZ, fixedZCylinderCenterXY, fixedZCylinderRadiusXY, fixedZCylinderMinMaxZ ) )
    {
        return PushDiscOutOfFixedDisc2D( mobileZCylinderCenterXY, mobileZCylinderRadiusXY, fixedZCylinderCenterXY, fixedZCylinderRadiusXY );
    }

    return false;
}

//-----------------------------------------------------------------------------------------------
void TransformPosition2D( Vec2& posToTransform, float uniformScale, float rotationDegrees, Vec2 const& translation )
{
    // uniformScale
    posToTransform.x *= uniformScale;
    posToTransform.y *= uniformScale;

    // rotation
    float rotationRadians = ConvertDegreesToRadians( rotationDegrees + Atan2Degrees( posToTransform.y, posToTransform.x ) );
    float length          = posToTransform.GetLength();

    posToTransform.x = cosf( rotationRadians ) * length;  // rotatedX
    posToTransform.y = sinf( rotationRadians ) * length;  // rotatedY

    // translation
    posToTransform.x += translation.x;
    posToTransform.y += translation.y;
}

//-----------------------------------------------------------------------------------------------
void TransformPosition2D( Vec2& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation )
{
    float originalX = posToTransform.x;
    float originalY = posToTransform.y;

    posToTransform = iBasis * originalX + jBasis * originalY + translation;
}

//-----------------------------------------------------------------------------------------------
void TransformPositionXY3D( Vec3& posToTransform, float xyScale, float zRotationDegrees, Vec2 const& xyTranslation )
{
    // uniformScale
    posToTransform.x *= xyScale;
    posToTransform.y *= xyScale;

    // rotation
    float rotationRadians = ConvertDegreesToRadians( zRotationDegrees + Atan2Degrees( posToTransform.y, posToTransform.x ) );
    float lengthXY        = posToTransform.GetLengthXY();

    posToTransform.x = cosf( rotationRadians ) * lengthXY;  // rotatedX
    posToTransform.y = sinf( rotationRadians ) * lengthXY;  // rotatedY

    // translation
    posToTransform.x += xyTranslation.x;
    posToTransform.y += xyTranslation.y;
}

//-----------------------------------------------------------------------------------------------
void TransformPositionXY3D( Vec3& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation )
{
    float originalX = posToTransform.x;
    float originalY = posToTransform.y;

    posToTransform.x = originalX * iBasis.x + originalY * jBasis.x + translation.x;
    posToTransform.y = originalX * iBasis.y + originalY * jBasis.y + translation.y;
}

//-----------------------------------------------------------------------------------------------
RaycastResult2D RaycastVsDisc2D( Vec2 rayStart, Vec2 rayFwdNormal, float rayLength, Vec2 discCenter, float discRadius )
{
    RaycastResult2D result;

    Vec2 const&     i   = rayFwdNormal;
    Vec2            j   = rayFwdNormal.GetRotatedBy90Degrees();
    Vec2            SC  = discCenter - rayStart;
    float           SCj = DotProduct2D( SC, j );

    if ( SCj >= discRadius )
    {
        return result;
    }

    if ( SCj <= -discRadius )
    {
        return result;
    }

    float SCi = DotProduct2D( SC, i );

    if ( SCi >= rayLength + discRadius )
    {
        return result;
    }

    if ( SCi <= -discRadius )
    {
        return result;
    }

    if ( IsPointInsideDisc2D( rayStart, discCenter, discRadius ) )
    {
        result.m_didImpact    = true;
        result.m_impactDist   = 0.f;
        result.m_impactPos    = rayStart;
        result.m_impactNormal = -rayFwdNormal;
        result.m_rayStartPos  = rayStart;
        result.m_rayFwdNormal = rayFwdNormal;

        float adjustmentDistance = sqrtf( ( discRadius * discRadius ) - ( SCj * SCj ) );
        result.m_exitDist        = SCi + adjustmentDistance;

        return result;
    }

    float adjustmentDistance = sqrtf( ( discRadius * discRadius ) - ( SCj * SCj ) );
    float impactDis          = SCi - adjustmentDistance;

    if ( impactDis >= rayLength )
    {
        return result;
    }

    if ( impactDis <= 0.f )
    {
        return result;
    }

    result.m_didImpact    = true;
    result.m_impactDist   = impactDis;
    result.m_impactPos    = rayStart + ( rayFwdNormal * result.m_impactDist );
    result.m_impactNormal = ( result.m_impactPos - discCenter ).GetNormalized();
    result.m_rayStartPos  = rayStart;
    result.m_rayFwdNormal = rayFwdNormal;

    result.m_exitDist = SCi + adjustmentDistance;

    return result;
}

//-----------------------------------------------------------------------------------------------
RaycastResult2D RaycastVsLineSegments2D( Vec2 rayStart, Vec2 rayFwdNormal, float rayLength, Vec2 segStart, Vec2 segEnd )
{
    RaycastResult2D result;
    result.m_rayMaxLength = rayLength;

    Vec2 const& i  = rayFwdNormal;
    Vec2        j  = rayFwdNormal.GetRotatedBy90Degrees();
    Vec2        RA = segEnd - rayStart;
    Vec2        RB = segStart - rayStart;
    Vec2        AB = segStart - segEnd;

    float       RAj = DotProduct2D( RA, j );
    float       RBj = DotProduct2D( RB, j );

    if ( RAj * RBj >= 0 )
    {
        return result;
    }

    float RAi = DotProduct2D( RA, i );
    float RBi = DotProduct2D( RB, i );

    if ( RAi <= 0 && RBi <= 0 )
    {
        return result;
    }

    if ( RAi >= rayLength && RBi >= rayLength )
    {
        return result;
    }

    float lineSegmentHitFraction = RAj / ( RAj - RBj );
    float impactDist             = RAi + ( lineSegmentHitFraction * ( RBi - RAi ) );

    if ( impactDist <= 0 || impactDist >= rayLength )
    {
        return result;
    }

    Vec2 impactPos    = rayStart + ( i * impactDist );
    Vec2 impactNormal = AB.GetRotatedBy90Degrees().GetNormalized();
    if ( DotProduct2D( impactNormal, i ) > 0 )
    {
        impactNormal *= -1;
    }

    result.m_didImpact    = true;
    result.m_impactDist   = impactDist;
    result.m_impactPos    = impactPos;
    result.m_impactNormal = impactNormal;
    result.m_rayStartPos  = rayStart;
    result.m_rayFwdNormal = rayFwdNormal;

    return result;
}

//-----------------------------------------------------------------------------------------------
RaycastResult2D RaycastVsAABB2D( Vec2 rayStart, Vec2 rayEnd, Vec2 rayFwdNormal, float rayLength, AABB2 const& alignedBox )
{
    RaycastResult2D result;
    AABB2           rayBox;
    rayBox.m_mins.x = rayStart.x < rayEnd.x ? rayStart.x : rayEnd.x;
    rayBox.m_maxs.x = rayStart.x > rayEnd.x ? rayStart.x : rayEnd.x;
    rayBox.m_mins.y = rayStart.y < rayEnd.y ? rayStart.y : rayEnd.y;
    rayBox.m_maxs.y = rayStart.y > rayEnd.y ? rayStart.y : rayEnd.y;

    if ( !DoAABB2DsOverlap( rayBox, alignedBox ) )
    {
        return result;
    }

    if ( IsPointInsideAABB2D( rayStart, alignedBox ) )
    {
        result.m_impactPos    = rayStart;
        result.m_impactNormal = -rayFwdNormal;
        result.m_didImpact    = true;
        return result;
    }

    float enterXFraction;
    float exitXFraction;

    float lengthX = rayFwdNormal.x * rayLength;
    if ( lengthX == 0.f )
    {
        if ( rayStart.x <= alignedBox.m_mins.x || rayStart.x >= alignedBox.m_maxs.x )
        {
            return result;
        }
        enterXFraction = -FLT_MAX;
        exitXFraction  = FLT_MAX;
    }
    else
    {
        float enterX = alignedBox.m_mins.x - rayStart.x;
        float exitX  = alignedBox.m_maxs.x - rayStart.x;

        enterXFraction = enterX / lengthX;
        exitXFraction  = exitX / lengthX;
        if ( enterXFraction > exitXFraction )
        {
            float temp     = enterXFraction;
            enterXFraction = exitXFraction;
            exitXFraction  = temp;
        }
    }

    float enterYFraction;
    float exitYFraction;

    float lengthY = rayFwdNormal.y * rayLength;
    if ( lengthY == 0.f )
    {
        if ( rayStart.y <= alignedBox.m_mins.y || rayStart.y >= alignedBox.m_maxs.y )
        {
            return result;
        }

        enterYFraction = -FLT_MAX;
        exitYFraction  = FLT_MAX;
    }
    else
    {
        float enterY   = alignedBox.m_mins.y - rayStart.y;
        float exitY    = alignedBox.m_maxs.y - rayStart.y;
        enterYFraction = enterY / lengthY;
        exitYFraction  = exitY / lengthY;

        if ( enterYFraction > exitYFraction )
        {
            float temp     = enterYFraction;
            enterYFraction = exitYFraction;
            exitYFraction  = temp;
        }
    }
    float tEnter = enterXFraction > enterYFraction ? enterXFraction : enterYFraction;
    float tExit  = exitXFraction < exitYFraction ? exitXFraction : exitYFraction;

    if ( tEnter < 0.f || tEnter > 1.f )
    {
        return result;
    }

    if ( tEnter > tExit )
    {
        return result;
    }

    FloatRange xRange = FloatRange( enterXFraction, exitXFraction );
    FloatRange yRange = FloatRange( enterYFraction, exitYFraction );

    if ( !xRange.IsOverlappingWith( yRange ) )
    {
        return result;
    }

    Vec2  impactNormal;
    Vec2  impactPos;
    float impactDist = 0.f;
    if ( xRange.m_min < yRange.m_min )
    {
        impactNormal = Vec2( 0, 1 );
        if ( DotProduct2D( impactNormal, rayFwdNormal ) > 0 )
        {
            impactNormal *= -1;
        }
        impactDist = rayLength * yRange.m_min;
        impactPos  = rayStart + rayFwdNormal * rayLength * yRange.m_min;
    }
    else if ( xRange.m_min > yRange.m_min )
    {
        impactNormal = Vec2( 1, 0 );
        if ( DotProduct2D( impactNormal, rayFwdNormal ) > 0 )
        {
            impactNormal *= -1;
        }
        impactDist = rayLength * xRange.m_min;
        impactPos  = rayStart + rayFwdNormal * rayLength * xRange.m_min;
    }

    result.m_rayMaxLength = rayLength;
    result.m_rayStartPos  = rayStart;
    result.m_rayFwdNormal = rayFwdNormal;
    result.m_impactDist   = impactDist;
    result.m_impactPos    = impactPos;
    result.m_impactNormal = impactNormal;
    result.m_didImpact    = true;
    return result;
}

//-----------------------------------------------------------------------------------------------
RaycastResult3D RaycastVsAABB3D( Vec3 rayStart, Vec3 rayForwardNormal, float rayLength, AABB3 box )
{
    RaycastResult3D result;
    Vec3            rayEnd = rayStart + rayForwardNormal * rayLength;
    AABB3           rayBox;
    rayBox.m_mins.x = std::min( rayStart.x, rayEnd.x );
    rayBox.m_mins.y = std::min( rayStart.y, rayEnd.y );
    rayBox.m_mins.z = std::min( rayStart.z, rayEnd.z );

    rayBox.m_maxs.x = rayStart.x > rayEnd.x ? rayStart.x : rayEnd.x;
    rayBox.m_maxs.y = rayStart.y > rayEnd.y ? rayStart.y : rayEnd.y;
    rayBox.m_maxs.z = rayStart.z > rayEnd.z ? rayStart.z : rayEnd.z;

    if ( !DoAABBsOverlap3D( box, rayBox ) )
    {
        return result;
    }

    if ( IsPointInsideAABB3D( rayStart, box ) )
    {
        result.m_didImpact        = true;
        result.m_impactDistance   = 0.f;
        result.m_impactPosition   = rayStart;
        result.m_impactNormal     = -rayForwardNormal;
        result.m_rayStartPosition = rayStart;
        result.m_rayDirection     = rayForwardNormal;
        result.m_rayLength        = rayLength;
        return result;
    }

    float      rayLengthX = rayForwardNormal.x * rayLength;
    float      rayLengthY = rayForwardNormal.y * rayLength;
    float      rayLengthZ = rayForwardNormal.z * rayLength;

    float      enterX = ( box.m_mins.x - rayStart.x ) / rayLengthX;
    float      exitX  = ( box.m_maxs.x - rayStart.x ) / rayLengthX;
    FloatRange rangeX = FloatRange( std::min( enterX, exitX ), std::max( enterX, exitX ) );

    float      enterY = ( box.m_mins.y - rayStart.y ) / rayLengthY;
    float      exitY  = ( box.m_maxs.y - rayStart.y ) / rayLengthY;
    FloatRange rangeY = FloatRange( std::min( enterY, exitY ), std::max( enterY, exitY ) );

    float      enterZ = ( box.m_mins.z - rayStart.z ) / rayLengthZ;
    float      exitZ  = ( box.m_maxs.z - rayStart.z ) / rayLengthZ;
    FloatRange rangeZ = FloatRange( std::min( enterZ, exitZ ), std::max( enterZ, exitZ ) );

    float      impactT = std::max( { rangeX.m_min, rangeY.m_min, rangeZ.m_min } );
    float      exitT   = std::min( { rangeX.m_max, rangeY.m_max, rangeZ.m_max } );
    if ( impactT > exitT )
    {
        return result;
    }

    if ( impactT < 0.f || impactT > 1.f )
    {
        return result;
    }
    result.m_didImpact      = true;
    result.m_impactDistance = rayLength * impactT;
    result.m_impactPosition = rayStart + rayForwardNormal * result.m_impactDistance;

    if ( impactT == rangeX.m_min )
    {
        if ( rayForwardNormal.x > 0.f )
        {
            result.m_impactNormal = -Vec3::WORLD_FORWARD;
        }
        else
        {
            result.m_impactNormal = Vec3::WORLD_FORWARD;
        }
    }

    if ( impactT == rangeY.m_min )
    {
        float dot = DotProduct3D( rayForwardNormal, Vec3::WORLD_LEFT );
        if ( dot < 0.f )
        {
            result.m_impactNormal = Vec3::WORLD_LEFT;
        }
        else
        {
            result.m_impactNormal = -Vec3::WORLD_LEFT;
        }
    }

    if ( impactT == rangeZ.m_min )
    {
        float dot = DotProduct3D( rayForwardNormal, Vec3::WORLD_UP );
        if ( dot < 0.f )
        {
            result.m_impactNormal = Vec3::WORLD_UP;
        }
        else
        {
            result.m_impactNormal = -Vec3::WORLD_UP;
        }
    }

    result.m_rayStartPosition = rayStart;
    result.m_rayDirection     = rayForwardNormal;
    result.m_rayLength        = rayLength;
    return result;
}

//-----------------------------------------------------------------------------------------------
RaycastResult3D RaycastVsSphere3D( Vec3 rayStart, Vec3 rayForwardNormal, float rayLength, Vec3 sphereCenter, float sphereRadius )
{
    RaycastResult3D result;
    Vec3            SC               = sphereCenter - rayStart;
    float           projectionLength = DotProduct3D( SC, rayForwardNormal );
    Vec3            SCi              = projectionLength * rayForwardNormal;
    Vec3            SCj              = SC - SCi;

    float           distanceToRay = SCj.GetLengthSquared();
    if ( distanceToRay >= sphereRadius * sphereRadius )
    {
        return result;
    }

    // inside sphere
    if ( IsPointInsideSphere3D( rayStart, sphereCenter, sphereRadius ) )
    {
        result.m_didImpact        = true;
        result.m_impactDistance   = 0.f;
        result.m_impactPosition   = rayStart;
        result.m_impactNormal     = -rayForwardNormal;
        result.m_rayStartPosition = rayStart;
        result.m_rayDirection     = rayForwardNormal;
        result.m_rayLength        = rayLength;
        return result;
    }

    // behind sphere
    if ( projectionLength <= 0 )
    {
        return result;
    }

    float adjustmentDistance = sqrtf( ( sphereRadius * sphereRadius ) - distanceToRay );
    float impactDis          = projectionLength - adjustmentDistance;
    if ( impactDis >= rayLength )
    {
        return result;
    }

    if ( impactDis <= 0.f )
    {
        return result;
    }

    result.m_didImpact        = true;
    result.m_impactDistance   = impactDis;
    result.m_impactPosition   = rayStart + ( rayForwardNormal * result.m_impactDistance );
    result.m_impactNormal     = ( result.m_impactPosition - sphereCenter ).GetNormalized();
    result.m_rayStartPosition = rayStart;
    result.m_rayDirection     = rayForwardNormal;
    result.m_rayLength        = rayLength;

    return result;
}

//-----------------------------------------------------------------------------------------------
RaycastResult3D RaycastVsCylinderZ3D( Vec3 rayStart, Vec3 rayForwardNormal, float rayLength, Vec2 const& centerXY, FloatRange const& minMaxZ, float radiusXY )
{
    RaycastResult3D result;
    Vec3            rayEnd = rayStart + rayForwardNormal * rayLength;
    AABB3           rayBox;
    rayBox.m_mins.x = std::min( rayStart.x, rayEnd.x );
    rayBox.m_mins.y = std::min( rayStart.y, rayEnd.y );
    rayBox.m_mins.z = std::min( rayStart.z, rayEnd.z );

    rayBox.m_maxs.x = std::max( rayStart.x, rayEnd.x );
    rayBox.m_maxs.y = std::max( rayStart.y, rayEnd.y );
    rayBox.m_maxs.z = std::max( rayStart.z, rayEnd.z );

    // rayBox overlap with cylinder
    if ( !DoZCylinderAndAABBOverlap3D( centerXY, radiusXY, minMaxZ, rayBox ) )
    {
        return result;
    }

    // rayStart inside cylinder
    if ( IsPointInsideCylinderZ( rayStart, centerXY, radiusXY, minMaxZ ) )
    {
        result.m_didImpact        = true;
        result.m_impactDistance   = 0.f;
        result.m_impactPosition   = rayStart;
        result.m_impactNormal     = -rayForwardNormal;
        result.m_rayStartPosition = rayStart;
        result.m_rayDirection     = rayForwardNormal;
        result.m_rayLength        = rayLength;
        return result;
    }

    // enterZ and exitZ
    float      rayLengthZ = rayForwardNormal.z * rayLength;
    FloatRange rangeZ;

    // ray is parallel to xy axis
    if ( rayLengthZ == 0.f )
    {
        if ( rayStart.z >= minMaxZ.m_max || rayStart.z <= minMaxZ.m_min )
        {
            return result;
        }
        rangeZ = FloatRange( 0.f, 1.f );
    }
    else
    {
        float enterZ = ( minMaxZ.m_min - rayStart.z ) / rayLengthZ;
        float exitZ  = ( minMaxZ.m_max - rayStart.z ) / rayLengthZ;
        rangeZ       = FloatRange( std::min( enterZ, exitZ ), std::max( enterZ, exitZ ) );
    }

    // enterXY and exitXY
    Vec2       rayStartXY  = Vec2( rayStart.x, rayStart.y );
    Vec2       rayEndXY    = Vec2( rayEnd.x, rayEnd.y );
    float      rayLengthXY = ( rayEndXY - rayStartXY ).GetLength();
    FloatRange rangeXY;

    // ray is parallel to z axis
    if ( rayLengthXY == 0.f )
    {
        if ( !IsPointInsideDisc2D( rayStartXY, centerXY, radiusXY ) )
        {
            return result;
        }
        rangeXY = FloatRange( 0.f, 1.f );
    }
    else
    {
        Vec2            rayForwardNormalXY = ( rayEndXY - rayStartXY ).GetNormalized();
        RaycastResult2D xyResult           = RaycastVsDisc2D( rayStartXY, rayForwardNormalXY, rayLengthXY, centerXY, radiusXY );
        if ( !xyResult.m_didImpact )
        {
            return result;
        }

        float enterXY = xyResult.m_impactDist / rayLengthXY;
        float exitXY  = xyResult.m_exitDist / rayLengthXY;
        rangeXY       = FloatRange( std::min( enterXY, exitXY ), std::max( enterXY, exitXY ) );
    }

    // not hit
    if ( !rangeZ.IsOverlappingWith( rangeXY ) )
    {
        return result;
    }

    float impactT = std::max( rangeZ.m_min, rangeXY.m_min );
    if ( impactT < 0.f || impactT > 1.f )
    {
        return result;
    }

    result.m_impactDistance   = rayLength * impactT;
    result.m_impactPosition   = rayStart + rayForwardNormal * result.m_impactDistance;
    result.m_didImpact        = true;
    result.m_rayStartPosition = rayStart;
    result.m_rayDirection     = rayForwardNormal;
    result.m_rayLength        = rayLength;

    if ( impactT == rangeZ.m_min )
    {
        result.m_impactNormal = ( rayForwardNormal.z < 0.f ) ? Vec3( 0.f, 0.f, 1.f ) : Vec3( 0.f, 0.f, -1.f );
    }
    else
    {
        result.m_impactNormal = ( result.m_impactPosition - Vec3( centerXY.x, centerXY.y, result.m_impactPosition.z ) ).GetNormalized();
    }

    return result;
}

//-----------------------------------------------------------------------------------------------
Mat44 const GetBillboard( BillboardType billboardType, Mat44 const& targetTransform, Vec3 const& billboardPosition, Vec2 const& billboardScale /*= Vec2( 1.0f, 1.0f ) */ )
{
    Mat44       billboardTransform;
    Vec3        iBasis;
    Vec3        kBasis;
    Vec3        jBasis;
    Vec3 const& tBasis = billboardPosition;

    switch ( billboardType )
    {
        case BillboardType::WORLD_UP_FACING:
            {
                Vec3 cameraPosition    = targetTransform.GetTranslation3D();
                Vec3 billboardToCamera = cameraPosition - billboardPosition;
                billboardToCamera.z    = 0.f;
                Vec3 billboardForward  = billboardToCamera.GetNormalized();

                iBasis = billboardForward;
                kBasis = Vec3( 0.f, 0.f, 1.0f );
                jBasis = CrossProduct3D( kBasis, iBasis );

                break;
            }

        case BillboardType::WORLD_UP_OPPOSING:
            {
                iBasis   = -targetTransform.GetIBasis3D();
                iBasis.z = 0.f;
                iBasis   = iBasis.GetNormalized();
                kBasis   = Vec3( 0.f, 0.f, 1.0f );
                jBasis   = CrossProduct3D( kBasis, iBasis );

                break;
            }

        case BillboardType::FULL_FACING:
            {
                Vec3 cameraPosition    = targetTransform.GetTranslation3D();
                Vec3 billboardToCamera = cameraPosition - billboardPosition;
                Vec3 billboardForward  = billboardToCamera.GetNormalized();
                iBasis                 = billboardForward;

                if ( abs( DotProduct3D( billboardForward, Vec3::WORLD_UP ) ) < 0.99999f )
                {
                    jBasis = CrossProduct3D( Vec3::WORLD_UP, iBasis ).GetNormalized();
                    kBasis = CrossProduct3D( iBasis, jBasis );
                }
                else
                {
                    kBasis = CrossProduct3D( iBasis, Vec3::WORLD_LEFT ).GetNormalized();
                    jBasis = CrossProduct3D( kBasis, iBasis );
                }

                break;
            }

        case BillboardType::FULL_OPPOSING:
            {
                iBasis = -targetTransform.GetIBasis3D().GetNormalized();
                jBasis = -targetTransform.GetJBasis3D().GetNormalized();
                kBasis = targetTransform.GetKBasis3D().GetNormalized();

                break;
            }

        default:
            break;
    }

    jBasis *= billboardScale.x;
    kBasis *= billboardScale.y;

    billboardTransform.SetIJKT3D( iBasis, jBasis, kBasis, tBasis );

    return billboardTransform;
}

//-----------------------------------------------------------------------------------------------
float ComputeCubicBezier1D( float A, float B, float C, float D, float t )
{
    float R = Interpolate( A, B, t );
    float Q = Interpolate( B, C, t );
    float S = Interpolate( C, D, t );

    float E = Interpolate( R, Q, t );
    float F = Interpolate( Q, S, t );

    return Interpolate( E, F, t );
}

//-----------------------------------------------------------------------------------------------
float ComputeQuinticBezier1D( float A, float B, float C, float D, float E, float F, float t )
{
    float G = Interpolate( A, B, t );
    float H = Interpolate( B, C, t );
    float I = Interpolate( C, D, t );
    float J = Interpolate( D, E, t );
    float K = Interpolate( E, F, t );

    float L = Interpolate( G, H, t );
    float M = Interpolate( H, I, t );
    float N = Interpolate( I, J, t );
    float O = Interpolate( J, K, t );

    float P = Interpolate( L, M, t );
    float Q = Interpolate( M, N, t );
    float R = Interpolate( N, O, t );

    float S = Interpolate( P, Q, t );
    float T = Interpolate( Q, R, t );

    return Interpolate( S, T, t );
}

//-----------------------------------------------------------------------------------------------
float EaseLinear( float t )
{
    return t;
}

//-----------------------------------------------------------------------------------------------
float SmoothStart2( float t )
{
    return t * t;
}

//-----------------------------------------------------------------------------------------------
float SmoothStart3( float t )
{
    return t * t * t;
}

//-----------------------------------------------------------------------------------------------
float SmoothStart4( float t )
{
    return t * t * t * t;
}

//-----------------------------------------------------------------------------------------------
float SmoothStart5( float t )
{
    return t * t * t * t * t;
}

//-----------------------------------------------------------------------------------------------
float SmoothStart6( float t )
{
    return t * t * t * t * t * t;
}

//-----------------------------------------------------------------------------------------------
float SmoothStop2( float t )
{
    return 2 * t - t * t;
}

//-----------------------------------------------------------------------------------------------
float SmoothStop3( float t )
{
    float tSquared = t * t;
    return ( t * tSquared ) - 3 * tSquared + 3 * t;
}

//-----------------------------------------------------------------------------------------------
float SmoothStop4( float t )
{
    float tSquared = t * t;
    return ( 4 * t ) - ( 6 * tSquared ) + ( 4 * t * tSquared ) - ( tSquared * tSquared );
}

//-----------------------------------------------------------------------------------------------
float SmoothStop5( float t )
{
    float t2 = t * t;
    float t3 = t * t2;
    float t4 = t2 * t2;
    float t5 = t * t4;

    return ( 5 * t ) - ( 10 * t2 ) + ( 10 * t3 ) - ( 5 * t4 ) + t5;
}

//-----------------------------------------------------------------------------------------------
float SmoothStop6( float t )
{
    float t2 = t * t;
    float t3 = t * t2;
    float t4 = t2 * t2;
    float t5 = t * t4;
    float t6 = t3 * t3;

    return ( 6 * t ) - ( 15 * t2 ) + ( 20 * t3 ) - ( 15 * t4 ) + ( 6 * t5 ) - t6;
}

//-----------------------------------------------------------------------------------------------
float SmoothStep3( float t )
{
    return ComputeCubicBezier1D( 0.f, 0.f, 1.f, 1.f, t );
}

//-----------------------------------------------------------------------------------------------
float SmoothStep5( float t )
{
    return ComputeQuinticBezier1D( 0, 0, 0, 1, 1, 1, t );
}

//-----------------------------------------------------------------------------------------------
float Hesitate3( float t )
{
    return ComputeCubicBezier1D( 0, 1, 0, 1, t );
}

//-----------------------------------------------------------------------------------------------
float Hesitate5( float t )
{
    return ComputeQuinticBezier1D( 0, 1, 0, 1, 0, 1, t );
}

//-----------------------------------------------------------------------------------------------
float SmoothStep3_SmoothStart2( float t )
{
    float smoothStep3 = SmoothStep3( t );
    return SmoothStart2( smoothStep3 );
}

//-----------------------------------------------------------------------------------------------
float SmoothStep3_SmoothStop2( float t )
{
    float smoothStep3 = SmoothStep3( t );
    return SmoothStop2( smoothStep3 );
}

//-----------------------------------------------------------------------------------------------
float SmoothStep5_SmoothStart2( float t )
{
    float smoothStep5 = SmoothStep5( t );
    return SmoothStart2( smoothStep5 );
}

//-----------------------------------------------------------------------------------------------
float SmoothStep5_SmoothStop2( float t )
{
    float smoothStep5 = SmoothStep5( t );
    return SmoothStop2( smoothStep5 );
}