#include "Engine/AbilitySystem/AbilitySystemComponent.hpp"
#include "Engine/AbilitySystem/AttributeSet.hpp"
#include "Engine/AbilitySystem/GameplayEffectDefinition.hpp"
#include "AbilitySystemUtils.hpp"

//-----------------------------------------------------------------------------------------------
AbilitySystemComponent::AbilitySystemComponent()
{
    m_attributeSet = new AttributeSet();
}

//-----------------------------------------------------------------------------------------------
AbilitySystemComponent::~AbilitySystemComponent()
{
    DestroyAttributeSet();
    DestroyGrantedAbilities();
}

//-----------------------------------------------------------------------------------------------
void AbilitySystemComponent::InitializeAttributes( std::map< std::string, float > const& attributes )
{
    for ( auto const& [ key, value ] : attributes )
    {
        AttributeData attributeData;
        attributeData.m_baseValue           = value;
        attributeData.m_currentValue        = attributeData.m_baseValue;
        m_attributeSet->m_attributes[ key ] = attributeData;
    }
}

//-----------------------------------------------------------------------------------------------
bool AbilitySystemComponent::CanActivateAbility() const
{
    return false;    
}

//-----------------------------------------------------------------------------------------------
void AbilitySystemComponent::GrantAbility( GameplayAbilityDefinition const* gameplayAbilityDef )
{
    if ( GetGameplayAbilityByDef( gameplayAbilityDef ) ) return;
    
    GameplayAbility* gameplayAbility = new GameplayAbility();  
    gameplayAbility->m_definition    = gameplayAbilityDef;
    gameplayAbility->m_ownerASC      = this;
    
    m_grantedAbilities.push_back( gameplayAbility );
}

//-----------------------------------------------------------------------------------------------
GameplayAbility const* AbilitySystemComponent::GetGameplayAbilityByDef( GameplayAbilityDefinition const* gameplayAbilityDef )
{
    for ( int abilityIndex = 0; abilityIndex < static_cast<int>( m_grantedAbilities.size() ); ++ abilityIndex )
    {
        GameplayAbility const* gameplayAbility = m_grantedAbilities[ abilityIndex ];
        if ( gameplayAbility->m_definition == gameplayAbilityDef )
        {
            return  gameplayAbility;
        }
    }
    return  nullptr;
}

//-----------------------------------------------------------------------------------------------
void AbilitySystemComponent::ApplyGameplayEffectToTarget( GameplayEffect const& effect, AbilitySystemComponent* targetASC )
{
    GameplayEffect gameplayEffect = effect;
    gameplayEffect.m_sourceASC    = this;

    std::vector< GameplayModifierDefinition* > modifiers = gameplayEffect.m_gameplayEffectDef->m_modifiers;
    for ( int modifierIndex = 0; modifierIndex < static_cast< int >( modifiers.size() ); ++modifierIndex )
    {
        GameplayModifierDefinition* gameplayModifierDef = modifiers[ modifierIndex ];
        std::string const&          attributeName       = gameplayModifierDef->m_attributeName;
        GameplayModifierOperation   operation           = gameplayModifierDef->m_operation;
        float                       magnitude           = gameplayModifierDef->m_magnitude;
        AttributeData const&        attributeData       = m_attributeSet->GetAttribute( attributeName );

        float                       newBase = AbilitySystemUtils::ExecuteModifierOnBaseValue( attributeData.m_baseValue, magnitude, operation );
        targetASC->m_attributeSet->SetAttributeBaseValue( attributeName, newBase );
    }
}

//-----------------------------------------------------------------------------------------------
void AbilitySystemComponent::ApplyGameplayEffectToSelf( GameplayEffect const& effect )
{
    GameplayEffect gameplayEffect = effect;
    gameplayEffect.m_targetASC    = this;
    gameplayEffect.m_sourceASC    = this;

    std::vector< GameplayModifierDefinition* > modifiers = gameplayEffect.m_gameplayEffectDef->m_modifiers;
    for ( int modifierIndex = 0; modifierIndex < static_cast< int >( modifiers.size() ); ++modifierIndex )
    {
        GameplayModifierDefinition* gameplayModifierDef = modifiers[ modifierIndex ];
        std::string const&          attributeName       = gameplayModifierDef->m_attributeName;
        GameplayModifierOperation   opreation           = gameplayModifierDef->m_operation;
        float                       magnitude           = gameplayModifierDef->m_magnitude;
        AttributeData const&        attributeData       = m_attributeSet->GetAttribute( attributeName );

        float                       newBase = AbilitySystemUtils::ExecuteModifierOnBaseValue( attributeData.m_baseValue, magnitude, opreation );
        m_attributeSet->SetAttributeBaseValue( attributeName, newBase );
    }
}

//-----------------------------------------------------------------------------------------------
void AbilitySystemComponent::DestroyGrantedAbilities()
{
    for ( int abilityIndex = 0; abilityIndex < static_cast<int>( m_grantedAbilities.size() ); ++ abilityIndex )
    {
        if ( !m_grantedAbilities[ abilityIndex ] ){ continue; }
        delete m_grantedAbilities[ abilityIndex ];
        m_grantedAbilities[ abilityIndex ] = nullptr;
    }    
    m_grantedAbilities.clear();
}

//-----------------------------------------------------------------------------------------------
void AbilitySystemComponent::DestroyAttributeSet()
{
    delete m_attributeSet;
    m_attributeSet = nullptr;
}