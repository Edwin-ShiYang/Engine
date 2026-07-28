#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"


//-----------------------------------------------------------------------------------------------
const AABB2 AABB2::ZERO_TO_ONE( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) );


//-----------------------------------------------------------------------------------------------
AABB2::AABB2(AABB2 const& copyFrom)
	: m_mins ( copyFrom.m_mins )
	, m_maxs ( copyFrom.m_maxs )
{
}


//-----------------------------------------------------------------------------------------------
AABB2::AABB2( float minX, float minY, float maxX, float maxY )
	: m_mins( Vec2( minX, minY ) )
	, m_maxs( Vec2( maxX, maxY ) )
{
}


//-----------------------------------------------------------------------------------------------
AABB2::AABB2( Vec2 const& mins, Vec2 const& maxs )
	: m_mins( mins )
	, m_maxs( maxs )
{
}


//-----------------------------------------------------------------------------------------------
bool AABB2::IsPointInside( Vec2 const& point ) const
{
	bool isInsideX = ( m_mins.x < point.x ) && ( point.x < m_maxs.x );
	bool isInsideY = ( m_mins.y < point.y ) && ( point.y < m_maxs.y );

	return isInsideX && isInsideY;
}	


//-----------------------------------------------------------------------------------------------
Vec2 const AABB2::GetCenter() const
{
	float centerX = ( m_mins.x + m_maxs.x ) * 0.5f;
	float centerY = ( m_mins.y + m_maxs.y ) * 0.5f;

	return Vec2( centerX, centerY );
}


//-----------------------------------------------------------------------------------------------
Vec2 const AABB2::GetDimensions() const
{
	float width  = m_maxs.x - m_mins.x;
	float height = m_maxs.y - m_mins.y;

	return Vec2( width, height );
}


//-----------------------------------------------------------------------------------------------
Vec2 const AABB2::GetNearestPoint( Vec2 const& referencePosition ) const
{
	float nearestX = GetClamped( referencePosition.x, m_mins.x, m_maxs.x );
	float nearestY = GetClamped( referencePosition.y, m_mins.y, m_maxs.y );

	const Vec2 nearestPoint = Vec2( nearestX, nearestY );

	return nearestPoint;
}


//-----------------------------------------------------------------------------------------------
Vec2 const AABB2::GetPointAtUV( Vec2 const& uv ) const
{
	float x = Interpolate( m_mins.x, m_maxs.x, uv.x );
	float y = Interpolate( m_mins.y, m_maxs.y, uv.y );

	return Vec2( x, y );
}


//-----------------------------------------------------------------------------------------------
Vec2 const AABB2::GetUVForPoint( Vec2 const& point ) const
{
	float u = GetFractionWithinRange( point.x, m_mins.x, m_maxs.x );
	float v = GetFractionWithinRange( point.y, m_mins.y, m_maxs.y );

	return Vec2( u, v );
}


//-----------------------------------------------------------------------------------------------
void AABB2::SetFromText( char const* text )
{
	Strings result = SplitStringOnDelimiter( text, ',' );
	if ( static_cast < int >( result.size() ) != 4 )
	{
		m_mins.x = 0.f;
		m_mins.y = 0.f;
		m_maxs.x = 0.f;
		m_maxs.y = 0.f;

		return;
	}

	m_mins.x = static_cast< float > ( atof( result [ 0 ].c_str() ) );
	m_mins.y = static_cast< float > ( atof( result [ 1 ].c_str() ) );
	m_maxs.x = static_cast< float > ( atof( result [ 2 ].c_str() ) );
	m_maxs.y = static_cast< float > ( atof( result [ 3 ].c_str() ) );
}


//-----------------------------------------------------------------------------------------------
void AABB2::SetPadding( float topBottom, float leftRight )
{
	m_mins.x += leftRight;
	m_maxs.x -= leftRight;

	m_mins.y += topBottom;
	m_maxs.y -= topBottom;
}


//-----------------------------------------------------------------------------------------------
void AABB2::SetPadding( float top, float bottom, float left, float right )
{
	m_mins.x += left;
	m_maxs.x -= right;

	m_mins.y += bottom;
	m_maxs.y -= top;
}


//-----------------------------------------------------------------------------------------------
void AABB2::Translate( Vec2 const& translationToApply )
{
	m_mins += translationToApply;
	m_maxs += translationToApply;
}


//-----------------------------------------------------------------------------------------------
void AABB2::SetCenter( Vec2 const& newCenter )
{
	const Vec2 dimensions = GetDimensions();
	const Vec2 halfDimension = dimensions * 0.5f;

	m_mins = Vec2( newCenter.x - halfDimension.x, newCenter.y - halfDimension.y );
	m_maxs = Vec2( newCenter.x + halfDimension.x, newCenter.y + halfDimension.y );
}


//-----------------------------------------------------------------------------------------------
void AABB2::SetDimensions( Vec2 const& newDimensions )
{
	const Vec2 center = GetCenter();
	const Vec2 halfNewDimensions = newDimensions * 0.5f;

	m_mins = Vec2( center.x - halfNewDimensions.x, center.y - halfNewDimensions.y );
	m_maxs = Vec2( center.x + halfNewDimensions.x, center.y + halfNewDimensions.y );
}


//-----------------------------------------------------------------------------------------------
void AABB2::StretchToIncludePoint( Vec2 const& point )
{
	m_mins.x = ( point.x < m_mins.x ) ? point.x : m_mins.x;
	m_mins.y = ( point.y < m_mins.y ) ? point.y : m_mins.y;
	m_maxs.x = ( point.x > m_maxs.x ) ? point.x : m_maxs.x;
	m_maxs.y = ( point.y > m_maxs.y ) ? point.y : m_maxs.y;
}