#pragma once
#include "Engine/Math/Vec3.hpp"


//-----------------------------------------------------------------------------------------------
class Plane3 
{
public:
	Plane3() {};
	~Plane3() {};
	Plane3( Vec3 const& normal, float distAlongNormalFromOrigin );

	Vec3 m_normal;
	float m_distAlongNormalFromOrigin;
};