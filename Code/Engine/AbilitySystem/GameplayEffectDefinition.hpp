#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/XmlUtils.hpp"

//-----------------------------------------------------------------------------------------------
#include <vector>
#include <string>
#include "GameplayModifierDefinition.hpp"

//-----------------------------------------------------------------------------------------------
enum class GameplayEffectDurationType
{
    Instant,
    Infinite,
    HasDuration
};

//-----------------------------------------------------------------------------------------------
class GameplayEffectDefinition
{
public:
    GameplayEffectDefinition()  = default;
    ~GameplayEffectDefinition() = default;

    void                                            LoadFromXmlElement( XmlElement const& element );
    GameplayEffectDurationType                      ParseDurationPolicy( std::string durationPolicy );

    static void                                     InitializeDefinitions( std::string const& definitionFilePath );
    static void                                     ClearDefinitions();
    static std::vector< GameplayEffectDefinition* > s_definitions;

    std::string                                     m_name;
    GameplayEffectDurationType                      m_durationPolicy  = GameplayEffectDurationType::Instant;
    float                                           m_durationSeconds = 0.f;
    float                                           m_periodSeconds   = 0.f;
    std::vector< GameplayModifierDefinition* >      m_modifiers;
};