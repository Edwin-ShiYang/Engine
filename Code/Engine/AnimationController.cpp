#include "Engine/AnimationController.hpp"
#include "Engine/Core/Clock.hpp"

//-----------------------------------------------------------------------------------------------
AnimationController::AnimationController( Clock* parentClock )
    : m_clock( new Clock( *parentClock ) )
{
}

//-----------------------------------------------------------------------------------------------
AnimationController::~AnimationController()
{
    delete m_clock;
    m_clock = nullptr;
}

//-----------------------------------------------------------------------------------------------
void AnimationController::Update()
{
}