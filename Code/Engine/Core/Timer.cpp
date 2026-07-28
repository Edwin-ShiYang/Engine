#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
Timer::Timer( double period, const Clock* clock /*= nullptr */ )
	:m_period( period )
	, m_clock( clock )
{
	if ( !clock )
	{
		m_clock = &Clock::GetSystemClock();
	}
}


//-----------------------------------------------------------------------------------------------
void Timer::Start()
{
	m_startTime = m_clock->GetTotalSeconds();
}


//-----------------------------------------------------------------------------------------------
void Timer::Stop()
{
	m_startTime = -1.0;
}


//-----------------------------------------------------------------------------------------------
double Timer::GetElapsedTime() const
{
	if ( m_startTime < 0 )
	{
		return 0.0;
	}
	return m_clock->GetTotalSeconds() - m_startTime;
}


//-----------------------------------------------------------------------------------------------
double Timer::GetElapsedFraction() const
{
	return GetFractionWithinRange( m_clock->GetTotalSeconds(), m_startTime, m_startTime + m_period );
}


//-----------------------------------------------------------------------------------------------
bool Timer::IsStopped() const
{
	return m_startTime < 0.0;
}


//-----------------------------------------------------------------------------------------------
bool Timer::HasPeriodElapsed() const
{
	return !IsStopped() && ( GetElapsedTime() > m_period );
}


//-----------------------------------------------------------------------------------------------
bool Timer::DecrementPeriodIfElapsed()
{
	if ( HasPeriodElapsed() )
	{
		m_startTime += m_period;

		return true;
	}

	return false;
}