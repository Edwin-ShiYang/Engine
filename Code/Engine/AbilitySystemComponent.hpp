#pragma once

//-----------------------------------------------------------------------------------------------
#include "AttributeSet.hpp"

//-----------------------------------------------------------------------------------------------
class AbilitySystemComponent
{
public:
    AbilitySystemComponent();

    virtual ~AbilitySystemComponent();

    void InitializeAttributes( Attributes const& attributes );

public:
    AttributeSet* m_attributeSet = nullptr;
};