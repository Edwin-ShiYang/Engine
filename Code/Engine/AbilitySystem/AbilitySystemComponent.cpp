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
    delete m_attributeSet;
    m_attributeSet = nullptr;
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