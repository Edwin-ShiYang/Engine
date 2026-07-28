#pragma once


//-----------------------------------------------------------------------------------------------
struct FloatRange
{
public:
	float m_min;
	float m_max;

	static const FloatRange ZERO;
	static const FloatRange ONE;
	static const FloatRange ZERO_TO_ONE;

	FloatRange();
	explicit FloatRange( float min, float max );

	void  SetFromText( char const* text, char delimiter = ',' );

	bool IsOnRange( float value ) const;
	bool IsOverlappingWith( FloatRange const& floatRange ) const;

	void operator=( FloatRange const& copyFrom );
	bool operator==( FloatRange const& compare ) const;
	bool operator!=( FloatRange const& compare ) const;
};