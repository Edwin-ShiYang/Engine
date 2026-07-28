#include "Engine/Math/CubicBezierCurve2D.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
CubicBezierCurve2D::CubicBezierCurve2D( Vec2 startPos, Vec2 guidePos1, Vec2 guidePos2, Vec2 endPos )
	: m_startPos( startPos )
	, m_guidePos1( guidePos1 )
	, m_guidePos2( guidePos2 )
	, m_endPos( endPos )
{
}


//-----------------------------------------------------------------------------------------------
CubicBezierCurve2D::CubicBezierCurve2D( CubicHermiteCurve2D const& fromHermite )
{
	m_startPos  = fromHermite.m_startPos;
	m_guidePos1 = fromHermite.m_startPos + ( fromHermite.m_startVel / 3.0f );
	m_guidePos2 = fromHermite.m_endPos - ( fromHermite.m_endVel / 3.0f );
	m_endPos    = fromHermite.m_endPos;
}


//-----------------------------------------------------------------------------------------------
Vec2 CubicBezierCurve2D::EvaluateAtParametric( float parametricZeroToOne ) const
{
	float posX = ComputeCubicBezier1D( m_startPos.x, m_guidePos1.x, m_guidePos2.x, m_endPos.x, parametricZeroToOne );
	float posY = ComputeCubicBezier1D( m_startPos.y, m_guidePos1.y, m_guidePos2.y, m_endPos.y, parametricZeroToOne );

	return Vec2( posX, posY );
}


//-----------------------------------------------------------------------------------------------
float CubicBezierCurve2D::GetApproximateLength( int numSubdivisions /*= 64 */ ) const
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
Vec2 CubicBezierCurve2D::EvaluateAtApproximateDistance( float distanceAlongCurve, int numSubdivisions /*= 64 */ ) const
{
	float totalLength = 0.f;
	float step = 1.0f / static_cast< float >( numSubdivisions );
	Vec2  currentPos = m_startPos;
	float t = step;

	for ( int i = 0; i < static_cast< int >( numSubdivisions ); ++ i )
	{
		Vec2  nextPos = EvaluateAtParametric( t );
		float distanceBetweenPoints = GetDistance2D( currentPos, nextPos );

		if ( totalLength + distanceBetweenPoints >= distanceAlongCurve )
		{
			float distanceRemaining = distanceAlongCurve - totalLength;
			float fraction = distanceRemaining / distanceBetweenPoints;
			return currentPos + ( nextPos - currentPos ) * fraction;
		}

		totalLength += distanceBetweenPoints;

		currentPos = nextPos;
		t += step;
	}
	return m_endPos;
}