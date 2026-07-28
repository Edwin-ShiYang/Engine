#include "Engine/Math/Plane3.hpp"


//-----------------------------------------------------------------------------------------------
Plane3::Plane3( Vec3 const& normal, float distAlongNormalFromOrigin )
	: m_normal( normal )
	, m_distAlongNormalFromOrigin( distAlongNormalFromOrigin )
{
}