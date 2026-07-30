#pragma once


//----------------------------------------------------------------------------------------------
#include <vector>
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Math/Mat44.hpp"

//----------------------------------------------------------------------------------------------
struct Vertex;

//----------------------------------------------------------------------------------------------
struct DebugObject
{
	std::vector<Vertex> m_verts;
	Rgba8               m_startColor;
	Rgba8               m_endColor;
	Timer               m_timer;
	float               m_duration;
	Mat44               m_transform;
};