#pragma once
#include "Model/ModelImporter.hpp"

class Clock;

//-----------------------------------------------------------------------------------------------
class AnimController
{
    AnimController( Clock* parentClock );
    ~AnimController() = default;

    void Update();

public:
    Clock* m_clock = nullptr;
    Pose   m_currentPose;
    Pose   m_previousPose;
    Pose   m_pose;
};