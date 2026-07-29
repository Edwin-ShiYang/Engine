#pragma once

//-----------------------------------------------------------------------------------------------
#include <string>

//-----------------------------------------------------------------------------------------------
struct AbilitySystemConfig
{
    bool        m_isEnabled                  = true;
    std::string m_gameplayEffectDefsFilePath = "";
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