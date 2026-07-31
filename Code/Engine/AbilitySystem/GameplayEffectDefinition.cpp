#include "Engine/AbilitySystem/GameplayEffectDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
void GameplayEffectDefinition::LoadFromXmlElement( XmlElement const& element )
{
    m_name = ParseXmlAttribute( element, "name", "" );
    GUARANTEE_OR_DIE( !m_name.empty(), Stringf( "name is not Found" ) )

    std::string durationPolicy = ParseXmlAttribute( element, "durationPolicy", "" );
    GUARANTEE_OR_DIE( !durationPolicy.empty(), Stringf( "durationPolicy is not Found" ) )
    m_durationPolicy = ParseDurationPolicy( durationPolicy );

    XmlElement const* modifierElement = element.FirstChildElement( "Modifier" );
    GUARANTEE_OR_DIE( modifierElement, "GameplayEffectDefinition has no Modifier" );
    while ( modifierElement )
    {
        GameplayModifierDefinition* gameplayModifierDef = new GameplayModifierDefinition();
        gameplayModifierDef->LoadFromXmlElement( *modifierElement );

        m_modifiers.push_back( gameplayModifierDef );
        modifierElement = modifierElement->NextSiblingElement( "Modifier" );
    }
}

//-----------------------------------------------------------------------------------------------
void GameplayEffectDefinition::InitializeDefinitions( std::string const& definitionFilePath )
{
    XmlDocument gameplayEffectDefsXml;
    char const* file   = definitionFilePath.c_str();
    XmlResult   result = gameplayEffectDefsXml.LoadFile( file );
    GUARANTEE_OR_DIE( result == XML_SUCCESS, Stringf( "Failed to load actor defs file: %s", file ) )

    XmlElement* rootElement = gameplayEffectDefsXml.RootElement();
    GUARANTEE_OR_DIE( rootElement, Stringf( "Root Element is not found!" ) );

    XmlElement* gameplayEffectDefElement = rootElement->FirstChildElement();
    while ( gameplayEffectDefElement )
    {
        std::string gameplayEffectDefElementName = gameplayEffectDefElement->Name();
        GUARANTEE_OR_DIE( gameplayEffectDefElementName == "GameplayEffectDefinition", Stringf( "GameplayEffectDefinition is not Found" ) )

        GameplayEffectDefinition* gameplayEffectDef = new GameplayEffectDefinition();
        gameplayEffectDef->LoadFromXmlElement( *gameplayEffectDefElement );

        s_definitions.push_back( gameplayEffectDef );
        gameplayEffectDefElement = gameplayEffectDefElement->NextSiblingElement();
    }
}

//-----------------------------------------------------------------------------------------------
void GameplayEffectDefinition::ClearDefinitions()
{
    for ( int gameplayEffectDefIndex = 0; gameplayEffectDefIndex < static_cast< int >( s_definitions.size() ); ++gameplayEffectDefIndex )
    {
        GameplayEffectDefinition* gameplayEffectDef = s_definitions[ gameplayEffectDefIndex ];
        if ( gameplayEffectDef )
        {
            delete gameplayEffectDef;
            gameplayEffectDef = nullptr;
        }
    }
    s_definitions.clear();
}

//-----------------------------------------------------------------------------------------------
GameplayEffectDurationPolicy GameplayEffectDefinition::ParseDurationPolicy( std::string durationPolicy )
{
    if ( durationPolicy == "Instant" ) return GameplayEffectDurationPolicy::Instant;
    if ( durationPolicy == "Infinite" ) return GameplayEffectDurationPolicy::Infinite;
    if ( durationPolicy == "HasDuration" ) return GameplayEffectDurationPolicy::HasDuration;

    ERROR_AND_DIE( Stringf( "Invalid durationPolicy: %s", durationPolicy.c_str() ) );
}

//-----------------------------------------------------------------------------------------------
char const* GameplayEffectDefinition::GetDurationPolicyString() const
{
    switch ( m_durationPolicy )
    {
        case GameplayEffectDurationPolicy::Instant: return "Instant";
        case GameplayEffectDurationPolicy::Infinite: return "Infinite";
        case GameplayEffectDurationPolicy::HasDuration: return "HasDuration";
    }
    return "Unknown";
}

//-----------------------------------------------------------------------------------------------
GameplayEffectDefinition const* GameplayEffectDefinition::GetDefinitionByName( std::string const& name )
{
    for ( int gameplayEffectDefIndex = 0; gameplayEffectDefIndex < static_cast< int >( s_definitions.size() ); ++gameplayEffectDefIndex )
    {
        GameplayEffectDefinition* gameplayEffectDef = s_definitions[ gameplayEffectDefIndex ];
        if ( gameplayEffectDef->m_name == name )
        {
            return gameplayEffectDef;
        }
    }
    return nullptr;
}

//-----------------------------------------------------------------------------------------------
std::vector< GameplayEffectDefinition* > GameplayEffectDefinition::s_definitions;