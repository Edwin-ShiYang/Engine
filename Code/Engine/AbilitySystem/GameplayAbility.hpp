#pragma once

//-----------------------------------------------------------------------------------------------
class AbilitySystemComponent;
class GameplayAbilityDefinition;

//-----------------------------------------------------------------------------------------------
class GameplayAbility
{
public:
    GameplayAbility() = default;
    ~GameplayAbility() = default;
    
    GameplayAbilityDefinition const* m_definition = nullptr;
    AbilitySystemComponent const* m_ownerASC = nullptr;
    
};