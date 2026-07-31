#include "Engine/AbilitySystem/AbilitySystemComponentDefinition.hpp"

#include "GameplayAbilityDefinition.hpp"
#include "Engine/AbilitySystem/AttributeSetDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
AbilitySystemComponentDefinition::~AbilitySystemComponentDefinition()
{
    delete m_attributeSetDef;
    m_attributeSetDef = nullptr;
}

//-----------------------------------------------------------------------------------------------
void AbilitySystemComponentDefinition::LoadFromXmlElement( XmlElement const& element )
{
    m_name = ParseXmlAttribute( element, "name", m_name );
    GUARANTEE_OR_DIE( !m_name.empty(), "AbilitySystemComponentDefinition::LoadFromXmlElement - name is missing" )
}

//-----------------------------------------------------------------------------------------------
std::map< std::string, float > AbilitySystemComponentDefinition::GetAttributes() const
{
    return m_attributeSetDef->m_attributes;
}

//-----------------------------------------------------------------------------------------------
void AbilitySystemComponentDefinition::InitializeDefinitions( std::string const& definitionFilePath )
{
    XmlDocument ascDefsXml;
    char const* file   = definitionFilePath.c_str();
    XmlResult   result = ascDefsXml.LoadFile( file );
    GUARANTEE_OR_DIE( result == XML_SUCCESS, Stringf( "Failed to load AbilitySystemComponentDefinitions: %s", file ) )

    XmlElement* rootElement = ascDefsXml.RootElement();
    GUARANTEE_OR_DIE( rootElement, Stringf( "Root Element is not found!" ) )
    XmlElement* ascDefElement = rootElement->FirstChildElement();
    while ( ascDefElement )
    {
        std::string ascDefElementName = ascDefElement->Name();
        GUARANTEE_OR_DIE( ascDefElementName == "AbilitySystemComponentDefinition", Stringf( "AbilitySystemComponentDefinition is not Found" ) )

        AbilitySystemComponentDefinition* ascDef = new AbilitySystemComponentDefinition();
        ascDef->LoadFromXmlElement( *ascDefElement );

        if ( XmlElement const* attributeSetElement = ascDefElement->FirstChildElement( "AttributeSet" ) )
        {
            ascDef->m_attributeSetDef = new AttributeSetDefinition();
            ascDef->m_attributeSetDef->LoadFromXmlElement( *attributeSetElement );
        }
        
        if ( XmlElement const* abilitiesElement = ascDefElement->FirstChildElement( "Abilities" ) )
        {
            ascDef->LoadAbilitiesFromXml( abilitiesElement->FirstChildElement("Ability" ) );
        }

        s_definitions.push_back( ascDef );
        ascDefElement = ascDefElement->NextSiblingElement();
    }
}

//-----------------------------------------------------------------------------------------------
void AbilitySystemComponentDefinition::LoadAbilitiesFromXml( XmlElement const* element )
{
    while ( element )
    {
        std::string abilityElementName = element->Name();
        GUARANTEE_OR_DIE( abilityElementName == "Ability", Stringf( "Ability is not found" ) )
        
        std::string abilitiesName = ParseXmlAttribute( *element, "name", "" );
        GUARANTEE_OR_DIE( !abilitiesName.empty(), "AbilityName name is not found" )
        
        GameplayAbilityDefinition const* abilityDef = GameplayAbilityDefinition::GetDefinitionByName( abilitiesName );
        GUARANTEE_OR_DIE( abilityDef, "AbilityDef is not Found" )
        
        m_abilityDefs.push_back( abilityDef );
        
        element = element->NextSiblingElement();
    }    
}


//-----------------------------------------------------------------------------------------------
AbilitySystemComponentDefinition const* AbilitySystemComponentDefinition::GetDefinitionByName( std::string const& name )
{
    for ( int ascDefIndex = 0; ascDefIndex < static_cast< int >( s_definitions.size() ); ++ascDefIndex )
    {
        AbilitySystemComponentDefinition* ascDef = s_definitions[ ascDefIndex ];
        if ( ascDef->m_name == name )
        {
            return ascDef;
        }
    }
    return nullptr;
}

//-----------------------------------------------------------------------------------------------
void AbilitySystemComponentDefinition::ClearDefinitions()
{
    for ( int ascDefIndex = 0; ascDefIndex < static_cast< int >( s_definitions.size() ); ++ascDefIndex )
    {
        AbilitySystemComponentDefinition* ascDef = s_definitions[ ascDefIndex ];
        if ( ascDef )
        {
            delete ascDef;
            ascDef = nullptr;
        }
    }
    s_definitions.clear();
}

//-----------------------------------------------------------------------------------------------
std::vector< AbilitySystemComponentDefinition* > AbilitySystemComponentDefinition::s_definitions;