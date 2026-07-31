#pragma once

//-----------------------------------------------------------------------------------------------
#include <string>

//-----------------------------------------------------------------------------------------------
struct AbilitySystemConfig
{
    bool        m_isEnabled                          = false;
    std::string m_gameplayEffectDefsFilePath         = "";
    std::string m_abilitySystemComponentDefsFilePath = "";
    std::string m_gameplayAbilityDefsFilePath        = "";
};

//-----------------------------------------------------------------------------------------------
class AbilitySystem
{
public:
    AbilitySystem( AbilitySystemConfig const& config );
    ~AbilitySystem() = default;

    void                Startup();
    void                Shutdown();

    AbilitySystemConfig m_config;
};