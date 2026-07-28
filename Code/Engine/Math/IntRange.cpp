#include "Engine/Math/IntRange.hpp"


//-----------------------------------------------------------------------------------------------
const IntRange IntRange::ZERO( 0, 0 );
const IntRange IntRange::ONE( 1, 1 );
const IntRange IntRange::ZERO_TO_ONE( 0, 1 );


//-----------------------------------------------------------------------------------------------
IntRange::IntRange() 
	: m_min( 0 )
	, m_max( 0 )
{
}


//-----------------------------------------------------------------------------------------------
IntRange::IntRange( int min, int max )
	: m_min( min )
	, m_max( max )
{

}


//-----------------------------------------------------------------------------------------------
bool IntRange::IsOnRange( int value ) const
{
	return m_min <= value && value <= m_max;
}


//-----------------------------------------------------------------------------------------------
bool IntRange::IsOverlappingWith( IntRange const& intRange ) const
{
	if ( m_max < intRange.m_min || m_min > intRange.m_max )
	{
		return false;
	}
	return true;
}


//-----------------------------------------------------------------------------------------------
void IntRange::operator=( IntRange const& copyFrom )
{
	m_min = copyFrom.m_min;
	m_max = copyFrom.m_max;
}


//-----------------------------------------------------------------------------------------------
bool IntRange::operator==( IntRange const& compare ) const
{
	return m_min == compare.m_min && m_max == compare.m_max;
}


//-----------------------------------------------------------------------------------------------
bool IntRange::operator!=( IntRange const& compare ) const
{
	return m_min != compare.m_min || m_max != compare.m_max;
}