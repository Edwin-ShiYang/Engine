#pragma once

//-----------------------------------------------------------------------------------------------
class AbilitySystemComponent;
class GameplayEffectDefinition;

//-----------------------------------------------------------------------------------------------
class GameplayEffect
{
public:
    GameplayEffect()  = default;
    ~GameplayEffect() = default;

public:
    AbilitySystemComponent*         m_sourceASC         = nullptr;
    AbilitySystemComponent*         m_targetASC         = nullptr;
    GameplayEffectDefinition const* m_gameplayEffectDef = nullptr;
};