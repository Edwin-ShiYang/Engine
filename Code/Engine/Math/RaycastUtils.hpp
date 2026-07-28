#pragma once


//-----------------------------------------------------------------------------------------------
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"


//-----------------------------------------------------------------------------------------------
struct RaycastResult2D
{
	bool	m_didImpact  = false;
	float	m_impactDist = 0.f;
	float   m_exitDist   = 0.f;
	Vec2	m_impactPos;
	Vec2	m_impactNormal;
	Vec2	m_rayStartPos;
	Vec2	m_rayFwdNormal;
	float	m_rayMaxLength = 1.f;
};


//-----------------------------------------------------------------------------------------------
struct RaycastResult3D
{
	Vec3    m_rayStartPosition;
	Vec3    m_rayDirection;
	float   m_rayLength      = 1.0f;
	bool    m_didImpact      = false;
	float   m_impactDistance = 0.0f;
	Vec3    m_impactPosition;
	Vec3    m_impactNormal;
};