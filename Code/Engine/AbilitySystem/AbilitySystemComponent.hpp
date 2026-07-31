#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/AbilitySystem/AttributeSet.hpp"
#include <map>
#include <string>
#include "GameplayEffect.hpp"

//-----------------------------------------------------------------------------------------------
class AbilitySystemComponent
{
public:
    AbilitySystemComponent();

    virtual ~AbilitySystemComponent();

    void InitializeAttributes( std::map< std::string, float > const& attributes );

    /*
        Add GameplayEffectHandler
        void ApplyGameplayEffectToTarget( GameplayEffect const& effect, AbilitySystemComponent* targetASC );
    */
    void ApplyGameplayEffectToSelf( GameplayEffect const& effect );

public:
    AttributeSet* m_attributeSet = nullptr;
};