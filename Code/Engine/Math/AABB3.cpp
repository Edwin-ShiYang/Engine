#include "Engine/Math/AABB3.hpp"


//-----------------------------------------------------------------------------------------------
const AABB3 AABB3::ZERO_TO_ONE( Vec3( 0.f, 0.f, 0.f ), Vec3( 1.f, 1.f, 1.f ) );


//-----------------------------------------------------------------------------------------------
AABB3::AABB3( AABB3 const& copyFrom )
	: m_mins( copyFrom.m_mins )
	, m_maxs( copyFrom.m_maxs )
{
}


//-----------------------------------------------------------------------------------------------
AABB3::AABB3( float minX, float minY, float minZ, float maxX, float maxY, float maxZ )
	: m_mins( minX, minY, minZ )
	, m_maxs( maxX, maxY, maxZ )
{
}


//-----------------------------------------------------------------------------------------------
AABB3::AABB3( int minX, int minY, int minZ, int maxX, int maxY, int maxZ )
	: m_mins( static_cast< float >( minX ), static_cast< float >( minY ), static_cast< float >( minZ ) )
	, m_maxs( static_cast< float >( maxX ), static_cast< float >( maxY ), static_cast< float >( maxZ ) )
{
}


//-----------------------------------------------------------------------------------------------
AABB3::AABB3( Vec3 const& mins, Vec3 const& maxs )
	: m_mins( mins )
	, m_maxs( maxs )
{
}


//-----------------------------------------------------------------------------------------------
Vec3 const AABB3::GetCenter() const
{
	float centerX = ( m_mins.x + m_maxs.x ) * 0.5f;
	float centerY = ( m_mins.y + m_maxs.y ) * 0.5f;
	float centerZ = ( m_mins.z + m_maxs.z ) * 0.5f;

	return Vec3( centerX, centerY, centerZ );
}
