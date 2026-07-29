#pragma once

//-----------------------------------------------------------------------------------------------
#include "AttributeSet.hpp"

//-----------------------------------------------------------------------------------------------
class AbilitySystemComponent
{
public:
    AbilitySystemComponent();

    virtual ~AbilitySystemComponent();

    void SetAttributes( Attributes attributes );

public:
    AttributeSet* m_attributeSet = nullptr;
};