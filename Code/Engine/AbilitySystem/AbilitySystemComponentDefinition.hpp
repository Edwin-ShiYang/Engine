#pragma once

//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include <map>

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/XmlUtils.hpp"

class GameplayAbilityDefinition;
class AttributeSetDefinition;

//-----------------------------------------------------------------------------------------------
class AbilitySystemComponentDefinition
{
public:
    AbilitySystemComponentDefinition() = default;
    ~AbilitySystemComponentDefinition();

    void                                                    LoadFromXmlElement( XmlElement const& element );
    void                                                    LoadAbilitiesFromXml( XmlElement const* element  );
    
    std::map< std::string, float >                          GetAttributes() const;

    static void                                             InitializeDefinitions( std::string const& definitionFilePath );
    static AbilitySystemComponentDefinition const*          GetDefinitionByName( std::string const& name );
    static void                                             ClearDefinitions();
    static std::vector< AbilitySystemComponentDefinition* > s_definitions;

    std::string                                             m_name { "Unknown" };
    AttributeSetDefinition*                                 m_attributeSetDef { nullptr };
    std::vector< GameplayAbilityDefinition const* >               m_abilityDefs;
};