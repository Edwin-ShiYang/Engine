#include "CubicHermiteSpline.hpp"


//-----------------------------------------------------------------------------------------------
CubicHermiteSpline::CubicHermiteSpline( std::vector< Vec2 > const& positions )
	: m_positions( positions )
{
}


//-----------------------------------------------------------------------------------------------
float CubicHermiteSpline::GetApproximateLength()
{
	float totalLength = 0.f;
	for ( int i = 0; i < static_cast< int >( m_cubicHermiteCurves.size() ); ++ i )
	{
		totalLength += m_cubicHermiteCurves [ i ].GetApproximateLength();
	}
	return totalLength;
}


//-----------------------------------------------------------------------------------------------
Vec2 CubicHermiteSpline::EvaluateAtDistance( float distanceAlongSpline, int numSubdivisions )
{
	if ( m_cubicHermiteCurves.empty() ) {
		return Vec2( 0.f, 0.f );
	}

	if ( distanceAlongSpline <= 0.f ) {
		return m_cubicHermiteCurves [ 0 ].m_startPos;
	}

	float remainingDistance = distanceAlongSpline;

	for ( int i = 0; i < ( int ) m_cubicHermiteCurves.size(); ++ i )
	{
		float segmentLength = m_cubicHermiteCurves [ i ].GetApproximateLength( numSubdivisions );

		if ( remainingDistance <= segmentLength )
		{
			return m_cubicHermiteCurves [ i ].EvaluateAtApproximateDistance( remainingDistance, numSubdivisions );
		}

		remainingDistance -= segmentLength;
	}

	return m_cubicHermiteCurves.back().m_endPos;
}


//-----------------------------------------------------------------------------------------------
void CubicHermiteSpline::Construct()
{
	for ( int i = 0; i < static_cast< int >( m_positions.size() - 1 ); ++ i )
	{
		CubicHermiteCurve2D cubicHermiteCurve;
		Vec2 startPos = m_positions [ i ];
		Vec2 endPos   = m_positions [ i + 1 ];

		Vec2 startVel = Vec2::ZERO;
		Vec2 endVel   = Vec2::ZERO;

		if ( i > 0 )
		{
			startVel = ( m_positions [ i + 1 ] - m_positions [ i - 1 ] ) * .5f;
		}

		if ( i + 1 < static_cast< int >( m_positions.size() ) - 1 )
		{
			endVel = ( m_positions [ i + 2 ] - m_positions [ i ] ) * .5;
		}

		CubicHermiteCurve2D curveSection( startPos, startVel, endPos, endVel );
		m_cubicHermiteCurves.push_back( curveSection );
	}
}