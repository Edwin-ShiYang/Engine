#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/XmlUtils.hpp"

//-----------------------------------------------------------------------------------------------
class GameplayEffectDefinition;

//-----------------------------------------------------------------------------------------------
class GameplayAbilityDefinition
{
public:
    GameplayAbilityDefinition() = default;
    ~GameplayAbilityDefinition() = default;;
    
    void LoadFromXmlElement( XmlElement const& element );
    
    static void InitializeDefinitions( std::string const& definitionFilePath );
    static GameplayAbilityDefinition const*           GetDefinitionByName( std::string const& name );
    static void ClearDefinitions();
    static std::vector< GameplayAbilityDefinition* > s_definitions;
    
    std::string m_name;
    GameplayEffectDefinition const* m_gameplayEffectDef= nullptr;
};