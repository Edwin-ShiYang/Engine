#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/AbilitySystem/GameplayEffect.hpp"
#include "Engine/AbilitySystem/AttributeSet.hpp"
#include "Engine/AbilitySystem/GameplayAbility.hpp"

//-----------------------------------------------------------------------------------------------
#include <map>
#include <string>
#include <vector>

//-----------------------------------------------------------------------------------------------
class AbilitySystemComponent
{
public:
    AbilitySystemComponent();
    virtual ~AbilitySystemComponent();
    
    void InitializeAttributes( std::map< std::string, float > const& attributes );

  
    bool CanActivateAbility() const;
    bool TryActivateAbility( GameplayAbility* gameplayAbility );
    void GrantAbility( GameplayAbilityDefinition const* gameplayAbilityDef );
    GameplayAbility const* GetGameplayAbilityByDef( GameplayAbilityDefinition const* gameplayAbilityDef  );
    
    void ApplyGameplayEffectToTarget( GameplayEffect const& effect, AbilitySystemComponent* targetASC );
    void ApplyGameplayEffectToSelf( GameplayEffect const& effect );
    
private:
    void DestroyGrantedAbilities();
    void DestroyAttributeSet();

public:
    AttributeSet* m_attributeSet = nullptr;
    std::vector< GameplayAbility* > m_grantedAbilities;
};