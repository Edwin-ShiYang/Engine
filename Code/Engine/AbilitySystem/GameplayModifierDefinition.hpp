#pragma once

//-----------------------------------------------------------------------------------------------
#include <string>

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/XmlUtils.hpp"

//-----------------------------------------------------------------------------------------------
enum class GameplayModifierOperation
{
    Unknown = -1,
    Add,
    Multiply,
    Divide,
    Override
};

//-----------------------------------------------------------------------------------------------
class GameplayModifierDefinition
{
public:
    GameplayModifierDefinition()  = default;
    ~GameplayModifierDefinition() = default;

    char const*               GetOperationString();
    void                      LoadFromXmlElement( XmlElement const& element );
    GameplayModifierOperation ParseGameplayModifierOperation( std::string operation );

    std::string               m_attributeName;
    GameplayModifierOperation m_operation = GameplayModifierOperation::Add;
    float                     m_magnitude = 0.f;
};