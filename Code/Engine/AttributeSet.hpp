#pragma once

//-----------------------------------------------------------------------------------------------
#include <map>
#include <string>

typedef std::map< std::string, float > Attributes;

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