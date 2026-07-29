#pragma once

//-----------------------------------------------------------------------------------------------
#include <map>
#include <string>

//-----------------------------------------------------------------------------------------------
struct AttributeData
{
    float m_baseValue    = 0.f;
    float m_currentValue = 0.f;
};

//-----------------------------------------------------------------------------------------------
using Attributes = std::map< std::string, AttributeData >;

//-----------------------------------------------------------------------------------------------
class AttributeSet
{
    friend class AbilitySystemComponent;

public:
    AttributeSet()          = default;
    virtual ~AttributeSet() = default;

    bool       HasAttribute( std::string const& name ) const;
    void       SetAttribute( std::string const& name, float value );

    float      GetAttribute( std::string const& name ) const;
    Attributes GetAttributes() const;

protected:
    Attributes m_attributes;
};