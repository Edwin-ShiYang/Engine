#pragma once
#include "Model/ModelImporter.hpp"

//-----------------------------------------------------------------------------------------------
class Clock;

//-----------------------------------------------------------------------------------------------
class AnimationController
{
public:
    AnimationController( Clock* parentClock );
    virtual ~AnimationController();
    virtual void Update();

public:
    Clock* m_clock = nullptr;
    Pose   m_currentPose;
    Pose   m_previousPose;
    Pose   m_pose;
};