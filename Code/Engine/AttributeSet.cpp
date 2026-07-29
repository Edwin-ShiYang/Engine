#include "Engine/AttributeSet.hpp"

//-----------------------------------------------------------------------------------------------
void AttributeSet::SetAttribute( std::string const& name, float value )
{
    m_attributes[ name ] = value;
}

//-----------------------------------------------------------------------------------------------
float AttributeSet::GetAttribute( std::string const& name ) const
{
    return m_attributes.at( name );
}

//-----------------------------------------------------------------------------------------------
bool AttributeSet::HasAttribute( std::string const& name ) const
{
    return m_attributes.find( name ) != m_attributes.end();
}