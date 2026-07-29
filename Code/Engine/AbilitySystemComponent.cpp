#include "Engine/AbilitySystemComponent.hpp"
#include "Engine/AttributeSet.hpp"

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
void AbilitySystemComponent::SetAttributes( Attributes attributes )
{
    m_attributeSet->m_attributes = attributes;
}