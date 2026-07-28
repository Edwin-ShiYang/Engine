#pragma once


//-----------------------------------------------------------------------------------------------
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/CubicHermiteCurve2D.hpp"

#include <vector>


//-----------------------------------------------------------------------------------------------
class CubicHermiteSpline
{
public:
	CubicHermiteSpline() = default;
	CubicHermiteSpline( std::vector< Vec2 > const& positions );

	float GetApproximateLength();
	Vec2  EvaluateAtDistance( float distanceAlongSpline, int numSubdivisions = 64 );
	void  Construct();

	
	std::vector< Vec2 >                m_positions;
	std::vector< CubicHermiteCurve2D > m_cubicHermiteCurves;
};