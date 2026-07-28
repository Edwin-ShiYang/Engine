#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "../Core/StringUtils.hpp"


//-----------------------------------------------------------------------------------------------
const EulerAngles EulerAngles::ZERO( 0.f, 0.f, 0.f );


//-----------------------------------------------------------------------------------------------
EulerAngles::EulerAngles( float yawDegrees, float pitchDegress, float rollDegress )
	: m_yawDegrees( yawDegrees )
	, m_pitchDegrees( pitchDegress )
	, m_rollDegrees( rollDegress )
{
}


//-----------------------------------------------------------------------------------------------
Vec3 EulerAngles::GetForwardDir_IFwd_JLeft_KUp() const
{
	float cy = CosDegrees( m_yawDegrees );
	float sy = SinDegrees( m_yawDegrees );
	float cp = CosDegrees( m_pitchDegrees );
	float sp = SinDegrees( m_pitchDegrees );

	return Vec3( cy * cp, sy * cp, -sp );
}


//-----------------------------------------------------------------------------------------------
void EulerAngles::GetAsVectors_IFwd_JLeft_KUp( Vec3& out_forwardIBasis, Vec3& out_leftJBasis, Vec3& out_upKBasis ) const
{
	float cy = CosDegrees( m_yawDegrees );
	float sy = SinDegrees( m_yawDegrees );
	float cp = CosDegrees( m_pitchDegrees );
	float sp = SinDegrees( m_pitchDegrees );
	float cr = CosDegrees( m_rollDegrees );
	float sr = SinDegrees( m_rollDegrees );

	out_forwardIBasis.x = cy * cp;
	out_forwardIBasis.y = sy * cp;
	out_forwardIBasis.z = -sp;

	out_leftJBasis.x = ( cy * sp * sr ) - ( sy * cr );
	out_leftJBasis.y = ( sy * sp * sr ) + ( cy * cr );
	out_leftJBasis.z = cp * sr;

	out_upKBasis.x = ( cy * sp * cr ) + ( sy * sr );
	out_upKBasis.y = ( sy * sp * cr ) - ( cy * sr );
	out_upKBasis.z = cp * cr;

}


//-----------------------------------------------------------------------------------------------
Mat44 EulerAngles::GetAsMatrix_IFwd_JLeft_KUp() const
{
	float cy = CosDegrees( m_yawDegrees );
	float sy = SinDegrees( m_yawDegrees );
	float cp = CosDegrees( m_pitchDegrees );
	float sp = SinDegrees( m_pitchDegrees );
	float cr = CosDegrees( m_rollDegrees );
	float sr = SinDegrees( m_rollDegrees );
	
	Mat44 result;
	Vec3 iBasis = Vec3( cy * cp, sy * cp, -sp );
	Vec3 jBasis = Vec3( ( cy * sp * sr ) - ( sy * cr ), ( sy * sp * sr ) + ( cy * cr ), cp * sr );
	Vec3 kBasis = Vec3( ( cy * sp * cr ) + ( sy * sr ), ( sy * sp * cr ) - ( cy * sr ), cp * cr );
	result.SetIJK3D( iBasis, jBasis, kBasis );

	return result;
}


//-----------------------------------------------------------------------------------------------
void EulerAngles::SetFromText( char const* text )
{
	Strings result = SplitStringOnDelimiter( text, ',' );
	if ( static_cast < int >( result.size() ) != 3 )
	{
		m_yawDegrees   = 0.f;
		m_pitchDegrees = 0.f;
		m_rollDegrees  = 0.f;
		return;
	}

	m_yawDegrees   = static_cast< float > ( atof( result [ 0 ].c_str() ) );
	m_pitchDegrees = static_cast< float > ( atof( result [ 1 ].c_str() ) );
	m_rollDegrees  = static_cast< float > ( atof( result [ 2 ].c_str() ) );
}


//-----------------------------------------------------------------------------------------------
void EulerAngles::operator+=( EulerAngles const& anglesToAdd )
{
	m_yawDegrees   += anglesToAdd.m_yawDegrees;
	m_pitchDegrees += anglesToAdd.m_pitchDegrees;
	m_rollDegrees += anglesToAdd.m_rollDegrees;
}


//-----------------------------------------------------------------------------------------------
EulerAngles const Interpolate( EulerAngles const& from, EulerAngles const& to, float lerpFraction )
{
	EulerAngles lerped;
	float yawDistance = GetShortestAngularDispDegrees( from.m_yawDegrees, to.m_yawDegrees );
	lerped.m_yawDegrees = from.m_yawDegrees + ( lerpFraction * yawDistance );
	lerped.m_pitchDegrees = Interpolate( from.m_pitchDegrees, to.m_pitchDegrees, lerpFraction );
	lerped.m_rollDegrees = Interpolate( from.m_rollDegrees, to.m_rollDegrees, lerpFraction );
	return lerped;
}
