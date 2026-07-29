#include "Engine/AbilitySystem/AttributeSet.hpp"

//-----------------------------------------------------------------------------------------------
void AttributeSet::SetAttribute( std::string const& name, float value )
{
    // m_attributes[ name ] = value;
}

//-----------------------------------------------------------------------------------------------
float AttributeSet::GetAttribute( std::string const& name ) const
{
    // return m_attributes.at( name );

    return 0.f;
}

//-----------------------------------------------------------------------------------------------
Attributes AttributeSet::GetAttributes() const
{
    return m_attributes;
}

//-----------------------------------------------------------------------------------------------
bool AttributeSet::HasAttribute( std::string const& name ) const
{
    return m_attributes.find( name ) != m_attributes.end();
}