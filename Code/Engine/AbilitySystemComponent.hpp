#pragma once

class AttributeSet;

//-----------------------------------------------------------------------------------------------
class AbilitySystemComponent
{
public:
    AbilitySystemComponent()  = default;
    ~AbilitySystemComponent() = default;

public:
    AttributeSet* m_attributeSet = nullptr;
};