#include "Engine/AbilitySystem/GameplayAbilityDefinition.hpp"
#include "Engine/AbilitySystem/GameplayEffectDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
std::vector< GameplayAbilityDefinition* > GameplayAbilityDefinition::s_definitions;

//-----------------------------------------------------------------------------------------------
void GameplayAbilityDefinition::LoadFromXmlElement( XmlElement const& element )
{
    m_name = ParseXmlAttribute( element, "name", m_name );
    GUARANTEE_OR_DIE( !m_name.empty(), Stringf( "name is not Found" ) )
    
    std::string  gameplayEffectDefName = ParseXmlAttribute( element, "gameplayEffect", "" );
    GUARANTEE_OR_DIE( !gameplayEffectDefName.empty(), Stringf( "gameplayEffect is not Found" ) )
    
    m_gameplayEffectDef = GameplayEffectDefinition::GetDefinitionByName( gameplayEffectDefName );
    GUARANTEE_OR_DIE( m_gameplayEffectDef, Stringf( "gameplayEffectDef is not Found" ) )
}

//-----------------------------------------------------------------------------------------------
void GameplayAbilityDefinition::InitializeDefinitions( std::string const& definitionFilePath )
{
    XmlDocument gameplayAbilityDefsXml;
    char const* file   = definitionFilePath.c_str();
    XmlResult   result = gameplayAbilityDefsXml.LoadFile( file );
    GUARANTEE_OR_DIE( result == XML_SUCCESS, Stringf( "Failed to load GameplayAbilityDefinitions file: %s", file ) )

    XmlElement* rootElement = gameplayAbilityDefsXml.RootElement();
    GUARANTEE_OR_DIE( rootElement, Stringf( "Root Element is not found!" ) )

    XmlElement* gameplayAbilityDefElement = rootElement->FirstChildElement();
    while ( gameplayAbilityDefElement )
    {
        std::string gameplayAbilityDefElementName = gameplayAbilityDefElement->Name();
        GUARANTEE_OR_DIE( gameplayAbilityDefElementName == "GameplayAbilityDefinition", Stringf( "GameplayAbilityDefinition is not Found" ) )

        GameplayAbilityDefinition* gameplayAbilityDef = new GameplayAbilityDefinition();
        gameplayAbilityDef->LoadFromXmlElement( *gameplayAbilityDefElement );

        s_definitions.push_back( gameplayAbilityDef );
        gameplayAbilityDefElement = gameplayAbilityDefElement->NextSiblingElement();
    }
}

//-----------------------------------------------------------------------------------------------
GameplayAbilityDefinition const* GameplayAbilityDefinition::GetDefinitionByName( std::string const& name )
{
    for ( int abilityDefIndex = 0; abilityDefIndex < static_cast< int >( s_definitions.size() ); ++abilityDefIndex )
    {
        GameplayAbilityDefinition* abilityDef = s_definitions[ abilityDefIndex ];
        if ( abilityDef->m_name == name )
        {
            return abilityDef;
        }
    }
    return nullptr;
}

//-----------------------------------------------------------------------------------------------
void GameplayAbilityDefinition::ClearDefinitions()
{
    for ( int gameplayAbilityDefIndex = 0; gameplayAbilityDefIndex < static_cast< int >( s_definitions.size() ); ++gameplayAbilityDefIndex )
    {
        if ( GameplayAbilityDefinition const* gameplayAbilityDef = s_definitions[ gameplayAbilityDefIndex ] )
        {
            delete gameplayAbilityDef;
            gameplayAbilityDef = nullptr;
        }
    }
    s_definitions.clear();
}