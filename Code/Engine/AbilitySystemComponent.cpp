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
void AbilitySystemComponent::InitializeAttributes( Attributes const& attributes )
{
    m_attributeSet->m_attributes = attributes;
}