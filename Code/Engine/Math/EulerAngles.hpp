#pragma once


//-----------------------------------------------------------------------------------------------
struct Vec3;
struct Mat44;


//-----------------------------------------------------------------------------------------------
class EulerAngles 
{
public:
	EulerAngles() = default;
	EulerAngles( float yawDegrees, float pitchDegress, float rollDegress );
	Vec3  GetForwardDir_IFwd_JLeft_KUp() const;
	void  GetAsVectors_IFwd_JLeft_KUp( Vec3& out_forwardIBasis, Vec3& out_leftJBasis, Vec3& out_upKBasis ) const;
	Mat44 GetAsMatrix_IFwd_JLeft_KUp() const;

	void  SetFromText( char const* text );

	void operator += ( EulerAngles const& anglesToAdd );

	friend EulerAngles const Interpolate( EulerAngles const& from, EulerAngles const& to, float lerpFraction );

	/* Static */
	static const EulerAngles ZERO;

public:
	float m_yawDegrees   = 0.f;
	float m_pitchDegrees = 0.f;
	float m_rollDegrees  = 0.f;
};