#include "Engine/Core/NamedStrings.hpp"
#include "ErrorWarningAssert.hpp"
#include <string>

//-----------------------------------------------------------------------------------------------
NamedStrings::NamedStrings()
{
}

//-----------------------------------------------------------------------------------------------
NamedStrings::~NamedStrings()
{
}

//-----------------------------------------------------------------------------------------------
void NamedStrings::PopulateFromXmlElementAttributes( XmlElement const& element )
{
    XmlAttribute const* attribute = element.FirstAttribute();
    while ( attribute != nullptr )
    {
        m_keyValuePairs[ attribute->Name() ] = attribute->Value();
        attribute                            = attribute->Next();
    }
}

//-----------------------------------------------------------------------------------------------
float NamedStrings::GetValue( std::string const& keyName, float defaultValue ) const
{
    std::map< std::string, std::string >::const_iterator iter = m_keyValuePairs.find( keyName );

    if ( iter != m_keyValuePairs.end() && !iter->second.empty() )
    {
        return static_cast< float >( atof( iter->second.c_str() ) );
    }

    return defaultValue;
}

//-----------------------------------------------------------------------------------------------
IntVec2 NamedStrings::GetValue( std::string const& keyName, IntVec2 const& defaultValue ) const
{
    std::map< std::string, std::string >::const_iterator iter = m_keyValuePairs.find( keyName );
    IntVec2                                              result;

    if ( iter != m_keyValuePairs.end() )
    {
        std::string value = iter->second;
        result.SetFromText( value.c_str() );
        return result;
    }

    return defaultValue;
}

//-----------------------------------------------------------------------------------------------
Vec2 NamedStrings::GetValue( std::string const& keyName, Vec2 const& defaultValue ) const
{
    std::map< std::string, std::string >::const_iterator iter = m_keyValuePairs.find( keyName );
    Vec2                                                 result;

    if ( iter != m_keyValuePairs.end() )
    {
        std::string value = iter->second;
        result.SetFromText( value.c_str() );
        return result;
    }

    return defaultValue;
}

//-----------------------------------------------------------------------------------------------
int NamedStrings::GetValue( std::string const& keyName, int defaultValue ) const
{
    std::map< std::string, std::string >::const_iterator iter = m_keyValuePairs.find( keyName );
    if ( iter != m_keyValuePairs.end() )
    {
        return atoi( iter->second.c_str() );
    }

    return defaultValue;
}

//-----------------------------------------------------------------------------------------------
std::string NamedStrings::GetValue( std::string const& keyName, std::string const& defaultValue ) const
{
    std::map< std::string, std::string >::const_iterator iter = m_keyValuePairs.find( keyName );
    if ( iter != m_keyValuePairs.end() )
    {
        return iter->second;
    }

    return defaultValue;
}

//-----------------------------------------------------------------------------------------------
std::string NamedStrings::GetValue( std::string const& keyName, char const* defaultValue ) const
{
    std::map< std::string, std::string >::const_iterator iter = m_keyValuePairs.find( keyName );
    if ( iter != m_keyValuePairs.end() )
    {
        return iter->second;
    }

    return defaultValue;
}

//-----------------------------------------------------------------------------------------------
Rgba8 NamedStrings::GetValue( std::string const& keyName, Rgba8 const& defaultValue ) const
{
    Rgba8                                                result = defaultValue;
    std::map< std::string, std::string >::const_iterator iter   = m_keyValuePairs.find( keyName );
    if ( iter != m_keyValuePairs.end() )
    {
        result.SetFromText( iter->second.c_str() );
        return result;
    }

    return defaultValue;
}

//-----------------------------------------------------------------------------------------------
bool NamedStrings::GetValue( std::string const& keyName, bool defaultValue ) const
{
    std::map< std::string, std::string >::const_iterator iter = m_keyValuePairs.find( keyName );
    if ( iter != m_keyValuePairs.end() )
    {
        if ( iter->second == "true" )
        {
            return true;
        }
    }
    return defaultValue;
}

//-----------------------------------------------------------------------------------------------
AABB2 NamedStrings::GetValue( std::string const& keyName, AABB2 const& defaultValue ) const
{
    AABB2                                                result = defaultValue;
    std::map< std::string, std::string >::const_iterator iter   = m_keyValuePairs.find( keyName );
    if ( iter != m_keyValuePairs.end() )
    {
        result.SetFromText( iter->second.c_str() );
        return result;
    }

    return defaultValue;
}

//-----------------------------------------------------------------------------------------------
void NamedStrings::SetValue( std::string const& keyName, std::string const& newValue )
{
    m_keyValuePairs[ keyName ] = newValue;
}

//-----------------------------------------------------------------------------------------------
std::map< std::string, std::string > const& NamedStrings::GetKeyValuePairs() const
{
    return m_keyValuePairs;
}