#include "Engine/AbilitySystem/AbilitySystemUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
float AbilitySystemUtils::ExecuteModifierOnBaseValue( float baseValue, float magnitude, GameplayModifierOperation modifierOperation )
{
    switch ( modifierOperation )
    {
        case GameplayModifierOperation::Add:
            baseValue += magnitude;
            break;
        case GameplayModifierOperation::Multiply:
            baseValue *= magnitude;
            break;
        case GameplayModifierOperation::Divide:
            GUARANTEE_OR_DIE( magnitude != 0.f, "Gameplay modifier divide by zero" );
            baseValue /= magnitude;
            break;
        case GameplayModifierOperation::Override:
            baseValue = magnitude;
            break;
        default:
            break;
    }

    return baseValue;
}