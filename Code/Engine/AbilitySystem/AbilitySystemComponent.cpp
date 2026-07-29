#include "Engine/AbilitySystem/AbilitySystemComponent.hpp"
#include "Engine/AbilitySystem/AttributeSet.hpp"

//-----------------------------------------------------------------------------------------------
AbilitySystemComponent::AbilitySystemComponent()
{
    m_attributeSet = new AttributeSet();
}

//-----------------------------------------------------------------------------------------------
AbilitySystemComponent::~AbilitySystemComponent()
{
    delete m_attributeSet;
    m_attributeSet = nullptr;
}

//-----------------------------------------------------------------------------------------------
void AbilitySystemComponent::InitializeAttributes( std::map< std::string, float > attributes )
{
    for ( auto const& [ key, value ] : attributes )
    {
        AttributeData attributeData;
        attributeData.m_baseValue           = value;
        attributeData.m_currentValue        = attributeData.m_baseValue;
        m_attributeSet->m_attributes[ key ] = attributeData;
    }
}