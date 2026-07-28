#pragma once
#include "Vec3.hpp"


//-----------------------------------------------------------------------------------------------
class OBB3 
{
public:
	OBB3() {};
	~OBB3() {};
	OBB3( Vec3 const& center, Vec3 const& iBasisNormal, Vec3 const& jBasisNormal, Vec3 const& kBasisNormal, Vec3 const& halfDimensions );

	Vec3 m_iBasisNormal;
	Vec3 m_jBasisNormal;
	Vec3 m_kBasisNormal;
	Vec3 m_halfDimensions;
	Vec3 m_center;
};