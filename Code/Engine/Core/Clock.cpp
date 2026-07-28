#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/MathUtils.hpp"

#include <thread>

//-----------------------------------------------------------------------------------------------
static Clock* s_systemClock = nullptr;

//-----------------------------------------------------------------------------------------------
Clock::Clock()
{
    if ( s_systemClock == nullptr )
    {
        s_systemClock             = this;
        m_lastUpdateTimeInSeconds = GetCurrentTimeSeconds();
    }
    else
    {
        m_parent = s_systemClock;
        s_systemClock->AddChild( this );
    }
}

//-----------------------------------------------------------------------------------------------
Clock::Clock( Clock& parent )
    : m_parent( &parent )
{
    parent.AddChild( this );
}

//-----------------------------------------------------------------------------------------------
void Clock::Reset()
{
    m_lastUpdateTimeInSeconds = GetCurrentTimeSeconds();
    m_totalSeconds            = 0.0;
    m_deltaSeconds            = 0.0;
    m_frameCount              = 0;
}

//-----------------------------------------------------------------------------------------------
Clock::~Clock()
{
    if ( m_parent )
    {
        m_parent->RemoveChild( this );
        m_parent = nullptr;
    }
}

//-----------------------------------------------------------------------------------------------
bool Clock::IsPaused() const
{
    return m_isPaused;
}

//-----------------------------------------------------------------------------------------------
void Clock::Unpause()
{
    m_isPaused = false;
}

//-----------------------------------------------------------------------------------------------
void Clock::TogglePause()
{
    m_isPaused = !m_isPaused;
}

//-----------------------------------------------------------------------------------------------
void Clock::StepSingleFrame()
{
    m_isPaused        = false;
    m_stepSingleFrame = true;
}

//-----------------------------------------------------------------------------------------------
void Clock::SetTimeScale( double timeScale )
{
    m_timeScale = timeScale;
}

//-----------------------------------------------------------------------------------------------
double Clock::GetTimeScale() const
{
    return m_timeScale;
}

//-----------------------------------------------------------------------------------------------
double Clock::GetDeltaSeconds() const
{
    return m_deltaSeconds;
}

//-----------------------------------------------------------------------------------------------
double Clock::GetTotalSeconds() const
{
    return m_totalSeconds;
}

//-----------------------------------------------------------------------------------------------
double Clock::GetFrameRate() const
{
    if ( m_deltaSeconds == 0.0 )
    {
        return 0.0;
    }

    return 1.0 / m_deltaSeconds;
}

//-----------------------------------------------------------------------------------------------
int Clock::GetFrameCount() const
{
    return m_frameCount;
}

//-----------------------------------------------------------------------------------------------
Clock& Clock::GetSystemClock()
{
    if ( s_systemClock == nullptr )
    {
        CreateSystemClock();
    }

    return *s_systemClock;
}

//-----------------------------------------------------------------------------------------------
void Clock::CreateSystemClock()
{
    s_systemClock = new Clock();
}

//-----------------------------------------------------------------------------------------------
void Clock::TickSystemClock()
{
    s_systemClock->Tick();
}

//-----------------------------------------------------------------------------------------------
void Clock::DestroySystemClock()
{
    delete s_systemClock;
    s_systemClock = nullptr;
}

//-----------------------------------------------------------------------------------------------
void Clock::Tick()
{
    /*
        double currentTimeSeconds = GetCurrentTimeSeconds();
        double deltaSeconds       = currentTimeSeconds - m_lastUpdateTimeInSeconds;
        while ( deltaSeconds < m_minDeltaSeconds )
        {
            std::this_thread::yield();
            currentTimeSeconds = GetCurrentTimeSeconds();
            deltaSeconds       = currentTimeSeconds - m_lastUpdateTimeInSeconds;
        }

        if ( deltaSeconds > m_maxDeltaSeconds )
        {
            deltaSeconds = m_maxDeltaSeconds;
        }

        m_lastUpdateTimeInSeconds = currentTimeSeconds;

        Advance( deltaSeconds );
    */
    /*
    double timeNow = GetCurrentTimeSeconds();
    while ( timeNow - m_lastUpdateTimeInSeconds < m_maxDeltaSeconds )
    {
        timeNow = GetCurrentTimeSeconds();
    }
    m_deltaSeconds            = timeNow - m_lastUpdateTimeInSeconds;
    m_lastUpdateTimeInSeconds = timeNow;
    Advance( m_deltaSeconds );
    */

    double timeNow            = GetCurrentTimeSeconds();
    m_deltaSeconds            = GetClamped( timeNow - m_lastUpdateTimeInSeconds, 0.0, m_maxDeltaSeconds );
    m_lastUpdateTimeInSeconds = timeNow;
    Advance( m_deltaSeconds );
}

//-----------------------------------------------------------------------------------------------
void Clock::Advance( double deltaTimeSeconds )
{
    if ( m_isPaused && !m_stepSingleFrame )
    {
        deltaTimeSeconds = 0.0;
    }
    else
    {
        deltaTimeSeconds *= m_timeScale;
    }

    m_deltaSeconds = deltaTimeSeconds;
    m_totalSeconds += deltaTimeSeconds;
    m_frameCount++;

    int numOfChildren = static_cast< int >( m_children.size() );
    if ( numOfChildren > 0 )
    {
        for ( int clockIndex = 0; clockIndex < numOfChildren; ++clockIndex )
        {
            if ( m_children[ clockIndex ] )
            {
                m_children[ clockIndex ]->Advance( deltaTimeSeconds );
            }
        }
    }

    if ( m_stepSingleFrame )
    {
        m_isPaused        = true;
        m_stepSingleFrame = false;
    }
}

//-----------------------------------------------------------------------------------------------
void Clock::AddChild( Clock* childClock )
{
    if ( !childClock )
    {
        return;
    }

    for ( int clockIndex = 0; clockIndex < static_cast< int >( m_children.size() ); ++clockIndex )
    {
        if ( m_children[ clockIndex ] == nullptr )
        {
            m_children[ clockIndex ] = childClock;
            return;
        }
    }

    m_children.push_back( childClock );
}

//-----------------------------------------------------------------------------------------------
void Clock::RemoveChild( Clock* childClock )
{
    if ( !childClock )
    {
        return;
    }

    for ( int childIndex = 0; childIndex < static_cast< int >( m_children.size() ); ++childIndex )
    {
        if ( m_children[ childIndex ] == childClock )
        {
            m_children[ childIndex ] = nullptr;
            childClock->m_parent     = nullptr;
            return;
        }
    }
}