#pragma once


//-----------------------------------------------------------------------------------------------
#include "Engine/Core/XmlUtils.hpp"
#include "SpriteAnimDefinition.hpp"


//-----------------------------------------------------------------------------------------------
class SpriteAnimationGroupDefinition 
{
public:
	bool LoadFromXmlElement( XmlElement const& element );

	const SpriteAnimDefinition& GetAnimationForDirection( Vec3 const& direction ) const;
	float GetDuration() const;

	std::string m_name;
	bool m_scaleBySpeed = false;
	std::vector<Vec3> m_directions;
	std::vector< SpriteAnimDefinition > m_spriteAnimationDefinitions;
};