#include "Engine/AbilitySystem/AbilitySystem.hpp"
#include "Engine/AbilitySystem/GameplayEffectDefinition.hpp"
#include "Engine//Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
#include <string>

//-----------------------------------------------------------------------------------------------
AbilitySystem::AbilitySystem( AbilitySystemConfig const& config )
    : m_config( config )
{
}

//-----------------------------------------------------------------------------------------------
void AbilitySystem::Startup()
{
    GUARANTEE_OR_DIE( !m_config.m_gameplayEffectDefsFilePath.empty(), Stringf( "AbilitySystemConfig missing gameplay effect definitions file path" ) );
    GameplayEffectDefinition::InitializeDefinitions( m_config.m_gameplayEffectDefsFilePath );
}

//-----------------------------------------------------------------------------------------------
void AbilitySystem::Shutdown()
{
    GameplayEffectDefinition::ClearDefinitions();
}