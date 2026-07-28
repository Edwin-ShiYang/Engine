#include "Engine/Math/CubicHermiteCurve2D.hpp"
#include "Engine/Math/CubicBezierCurve2D.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
CubicHermiteCurve2D::CubicHermiteCurve2D( Vec2 startPos, Vec2 startVel, Vec2 endPos, Vec2 endVel )
	: m_startPos( startPos )
	, m_startVel( startVel )
	, m_endPos( endPos )
	, m_endVel( endVel )
{
}


//-----------------------------------------------------------------------------------------------
CubicHermiteCurve2D::CubicHermiteCurve2D( CubicBezierCurve2D const& fromCubicBezier )
{
	m_startPos = fromCubicBezier.m_startPos;
	m_startVel = 3.0f * ( fromCubicBezier.m_guidePos1 - fromCubicBezier.m_startPos );
	m_endVel   = 3.0f * ( fromCubicBezier.m_endPos - fromCubicBezier.m_guidePos2 );
	m_endPos   = fromCubicBezier.m_endPos;
}


//-----------------------------------------------------------------------------------------------
Vec2 CubicHermiteCurve2D::EvaluateAtParametric( float parametricZeroToOne ) const
{
	Vec2 guidePos1 = m_startPos + ( m_startVel / 3.0f );
	Vec2 guidePos2 = m_endPos - ( m_endVel / 3.0f );

	float posX = ComputeCubicBezier1D( m_startPos.x, guidePos1.x, guidePos2.x, m_endPos.x, parametricZeroToOne );
	float posY = ComputeCubicBezier1D( m_startPos.y, guidePos1.y, guidePos2.y, m_endPos.y, parametricZeroToOne );

	return Vec2( posX, posY );
}


//-----------------------------------------------------------------------------------------------
float CubicHermiteCurve2D::GetApproximateLength( int numSubdivisions /*= 64 */ ) const
{
	float totalLength = 0.f;

	float step = 1.0f / static_cast< float >( numSubdivisions );
	Vec2  currentPos = m_startPos;
	float t = step;

	for ( int i = 0; i < static_cast< int >( numSubdivisions ); ++ i )
	{
		Vec2  nextPos = EvaluateAtParametric( t );

		totalLength += GetDistance2D( currentPos, nextPos );

		currentPos = nextPos;
		t += step;
	}

	return totalLength;
}


//-----------------------------------------------------------------------------------------------
Vec2 CubicHermiteCurve2D::EvaluateAtApproximateDistance( float distanceAlongCurve, int numSubdivisions /*= 64 */ ) const
{
	float totalLength = 0.f;
	float step = 1.0f / static_cast< float >( numSubdivisions );
	Vec2  currentPos = m_startPos;
	float t = step;

	for ( int i = 0; i < static_cast< int >( numSubdivisions ); ++ i )
	{
		Vec2  nextPos = EvaluateAtParametric( t );
		float currentLength = GetDistance2D( currentPos, nextPos );

		if ( totalLength + currentLength >= distanceAlongCurve )
		{
			float distanceRemaining = distanceAlongCurve - totalLength;
			float fraction = distanceRemaining / currentLength;
			return currentPos + ( nextPos - currentPos ) * fraction;
		}

		totalLength += currentLength;

		currentPos = nextPos;
		t += step;
	}
	return m_endPos;
}