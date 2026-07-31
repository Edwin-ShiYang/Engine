#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/XmlUtils.hpp"

//-----------------------------------------------------------------------------------------------
#include <vector>
#include <string>
#include "GameplayModifierDefinition.hpp"

//-----------------------------------------------------------------------------------------------
enum class GameplayEffectDurationPolicy
{
    Unknown = -1,
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
    GameplayEffectDurationPolicy                    ParseDurationPolicy( std::string durationPolicy );
    char const*                                     GetDurationPolicyString() const;

    static GameplayEffectDefinition const*          GetDefinitionByName( std::string const& name );
    static void                                     InitializeDefinitions( std::string const& definitionFilePath );
    static void                                     ClearDefinitions();
    static std::vector< GameplayEffectDefinition* > s_definitions;

    std::string                                     m_name;
    GameplayEffectDurationPolicy                    m_durationPolicy  = GameplayEffectDurationPolicy::Instant;
    float                                           m_durationSeconds = 0.f;
    float                                           m_periodSeconds   = 0.f;
    std::vector< GameplayModifierDefinition* >      m_modifiers;
};