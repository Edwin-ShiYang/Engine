#include "Engine/AbilitySystem/AttributeSet.hpp"

//-----------------------------------------------------------------------------------------------
void AttributeSet::SetAttributeBaseValue( std::string const& name, float value )
{
    AttributeData& attributeData = m_attributes.at( name );
    attributeData.m_baseValue    = value;
    attributeData.m_currentValue = value;
}