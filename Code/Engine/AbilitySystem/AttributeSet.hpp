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

    void                        SetAttributeBaseValue( std::string const& name, float value );
    inline AttributeData const& GetAttribute( std::string const& name ) const
    {
        return m_attributes.at( name );
    };

    inline Attributes const& GetAttributes() const
    {
        return m_attributes;
    };

protected:
    Attributes m_attributes;
};