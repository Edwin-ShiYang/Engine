#pragma once
#include "GameplayModifierDefinition.hpp"

//-----------------------------------------------------------------------------------------------
class AbilitySystemUtils
{
public:
    static float ExecuteModifierOnBaseValue( float baseValue, float magnitude, GameplayModifierOperation modifierOperation );
};