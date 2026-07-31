#include "Engine/AbilitySystem/AbilitySystem.hpp"
#include "Engine/AbilitySystem/GameplayEffectDefinition.hpp"
#include "Engine/AbilitySystem/AbilitySystemComponentDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
#include <string>

#include "GameplayAbilityDefinition.hpp"

//-----------------------------------------------------------------------------------------------
AbilitySystem::AbilitySystem( AbilitySystemConfig const& config )
    : m_config( config )
{
}

//-----------------------------------------------------------------------------------------------
void AbilitySystem::Startup()
{
    if ( !m_config.m_isEnabled )
    {
        return;
    }
    
    GUARANTEE_OR_DIE( !m_config.m_gameplayEffectDefsFilePath.empty(), Stringf( "AbilitySystemConfig missing GameplayEffect definitions file path" ) )
    GameplayEffectDefinition::InitializeDefinitions( m_config.m_gameplayEffectDefsFilePath );   
    
    GUARANTEE_OR_DIE( !m_config.m_gameplayEffectDefsFilePath.empty(), Stringf( "AbilitySystemConfig missing GameplayAbility definitions file path" ) )
    GameplayAbilityDefinition::InitializeDefinitions( m_config.m_gameplayAbilityDefsFilePath );

    GUARANTEE_OR_DIE( !m_config.m_abilitySystemComponentDefsFilePath.empty(), Stringf( "AbilitySystemConfig missing AbilitySystemComponent definitions file path" ) )
    AbilitySystemComponentDefinition::InitializeDefinitions( m_config.m_abilitySystemComponentDefsFilePath );
}

//-----------------------------------------------------------------------------------------------
void AbilitySystem::Shutdown()
{
    GameplayEffectDefinition::ClearDefinitions();
    AbilitySystemComponentDefinition::ClearDefinitions();
    GameplayAbilityDefinition::ClearDefinitions();
}