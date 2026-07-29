#include "Engine/AbilitySystem/GameplayModifierDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
void GameplayModifierDefinition::LoadFromXmlElement( XmlElement const& element )
{
    m_attributeName = ParseXmlAttribute( element, "attributeName", m_attributeName );
    GUARANTEE_OR_DIE( !m_attributeName.empty(), "Modifier missing attributeName" );

    std::string operation = ParseXmlAttribute( element, "operation", "" );
    GUARANTEE_OR_DIE( !operation.empty(), "Modifier missing operation" );
    m_operation = ParseGameplayModifierOperation( operation );

    m_magnitude = ParseXmlAttribute( element, "magnitude", m_magnitude );
}

//-----------------------------------------------------------------------------------------------
GameplayModifierOperation GameplayModifierDefinition::ParseGameplayModifierOperation( std::string operation )
{
    if ( operation == "Add" ) return GameplayModifierOperation::Add;
    if ( operation == "Multiply" ) return GameplayModifierOperation::Multiply;
    if ( operation == "Divide" ) return GameplayModifierOperation::Divide;
    if ( operation == "Override" ) return GameplayModifierOperation::Override;

    ERROR_AND_DIE( Stringf( "Invalid Operation: %s", operation.c_str() ) );
}