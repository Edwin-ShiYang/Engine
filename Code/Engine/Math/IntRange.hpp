#pragma once


//-----------------------------------------------------------------------------------------------
struct IntRange
{
public:
	int m_min;
	int m_max;

	static const IntRange ZERO;
	static const IntRange ONE;
	static const IntRange ZERO_TO_ONE;

	IntRange();
	explicit IntRange( int min, int max );

	bool IsOnRange( int value ) const;
	bool IsOverlappingWith( IntRange const& intRange ) const;

	void operator=( IntRange const& copyFrom );
	bool operator==( IntRange const& compare ) const;
	bool operator!=( IntRange const& compare ) const;
};