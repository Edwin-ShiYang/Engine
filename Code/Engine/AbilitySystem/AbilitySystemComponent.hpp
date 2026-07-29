#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/AbilitySystem/AttributeSet.hpp"
#include <map>
#include <string>

//-----------------------------------------------------------------------------------------------
class AbilitySystemComponent
{
public:
    AbilitySystemComponent();

    virtual ~AbilitySystemComponent();

    void InitializeAttributes( std::map< std::string, float > attributes );

public:
    AttributeSet* m_attributeSet = nullptr;
};