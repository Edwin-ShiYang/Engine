#include "Engine/Math/OBB2.hpp"


//-----------------------------------------------------------------------------------------------
OBB2::OBB2( OBB2 const& copyFrom )
	: m_center( copyFrom.m_center )
	, m_iBasisNormal( copyFrom.m_iBasisNormal )
	, m_halfDimensions( copyFrom.m_halfDimensions )
{
}


//-----------------------------------------------------------------------------------------------
OBB2::OBB2( Vec2 const& center, Vec2 const& iBasisNormal, Vec2 const& halfDimensions )
	: m_center( center )
	, m_iBasisNormal( iBasisNormal )
	, m_halfDimensions( halfDimensions )
{
}

