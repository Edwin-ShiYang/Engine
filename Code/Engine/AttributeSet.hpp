#pragma once

//-----------------------------------------------------------------------------------------------
#include <map>
#include <string>

using Attributes = std::map< std::string, float >;

//-----------------------------------------------------------------------------------------------
class AttributeSet
{
    friend class AbilitySystemComponent;

public:
    AttributeSet()          = default;
    virtual ~AttributeSet() = default;

    void  SetAttribute( std::string const& name, float value );
    float GetAttribute( std::string const& name ) const;
    bool  HasAttribute( std::string const& name ) const;

protected:
    Attributes m_attributes;
};