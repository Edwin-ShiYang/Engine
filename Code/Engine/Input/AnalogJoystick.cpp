#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
Vec2 AnalogJoystick::GetPosition() const
{
	return m_correctedPosition;
}


//-----------------------------------------------------------------------------------------------
float AnalogJoystick::GetMagnitude() const
{
	const float magnitude = m_rawPosition.GetLength();

	if ( magnitude <= m_innerDeadZoneFraction )
	{
		return 0.f;
	}


	if ( magnitude >= m_outerDeadZoneFraction )
	{
		return 1.f;
	}

	return RangeMapClamped( magnitude, m_innerDeadZoneFraction, m_outerDeadZoneFraction, 0.f, 1.f );
}


//-----------------------------------------------------------------------------------------------
float AnalogJoystick::GetOrientationDegrees() const
{
	return Atan2Degrees( m_rawPosition.y, m_rawPosition.x );
}


//-----------------------------------------------------------------------------------------------
Vec2 AnalogJoystick::GetRawUncorrectedPosition() const
{
	return m_rawPosition;
}


//-----------------------------------------------------------------------------------------------
float AnalogJoystick::GetInnerDeadZoneFraction() const
{
	return m_innerDeadZoneFraction;
}

float AnalogJoystick::GetOuterDeadZoneFraction() const
{
	return m_outerDeadZoneFraction;
}


//-----------------------------------------------------------------------------------------------
void AnalogJoystick::SetDeadZoneThresholds( float normalizedInnerDeadzoneThreshold, float normalizedOuterDeadzoneThreshold )
{
	m_innerDeadZoneFraction = normalizedInnerDeadzoneThreshold;
	m_outerDeadZoneFraction = normalizedOuterDeadzoneThreshold;
}


//-----------------------------------------------------------------------------------------------
void AnalogJoystick::UpdatePosition(float rawNormalizedX, float rawNormalizedY)
{
	m_rawPosition.x = rawNormalizedX;
	m_rawPosition.y = rawNormalizedY;

	const float orientationDegrees = GetOrientationDegrees();
	const float magnitude = GetMagnitude();
	m_correctedPosition = Vec2::MakeFromPolarDegrees( orientationDegrees, magnitude );
}
