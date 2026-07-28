#include "Engine/Core/XmlUtils.hpp"


//-----------------------------------------------------------------------------------------------
int ParseXmlAttribute( XmlElement const& element, char const* attributeName, int defaultValue )
{
    int       result    = defaultValue;
    XmlResult xmlResult = element.QueryIntAttribute( attributeName, &result );

    return ( xmlResult == XML_SUCCESS ) ? result : defaultValue;
}


//-----------------------------------------------------------------------------------------------
char ParseXmlAttribute( XmlElement const& element, char const* attributeName, char defaultValue )
{
    char const* strValue = nullptr;

    if( element.QueryStringAttribute( attributeName, &strValue ) == XML_SUCCESS && strValue != nullptr )
    {
        if( strValue [ 0 ] != '\0' )
        {
            return strValue [ 0 ];
        }
    }

    return defaultValue;
}


//-----------------------------------------------------------------------------------------------
float ParseXmlAttribute( XmlElement const& element, char const* attributeName, float defaultValue )
{
    float     result    = defaultValue;
    XmlResult xmlResult = element.QueryFloatAttribute( attributeName, &result );

    return ( xmlResult == XML_SUCCESS ) ? result : defaultValue;
}


//-----------------------------------------------------------------------------------------------
bool ParseXmlAttribute( XmlElement const& element, char const* attributeName, bool defaultValue )
{
    bool      result    = defaultValue;
    XmlResult xmlResult = element.QueryBoolAttribute( attributeName, &result );

    return ( xmlResult == XML_SUCCESS ) ? result : defaultValue;
}


//-----------------------------------------------------------------------------------------------
Rgba8 ParseXmlAttribute( XmlElement const& element, char const* attributeName, Rgba8 const& defaultValue )
{
    Rgba8       result    = defaultValue;
    char const* value     = nullptr;
    XmlResult   xmlResult = element.QueryStringAttribute( attributeName, &value );
    if( xmlResult == XML_SUCCESS )
    {
        result.SetFromText( value );
    }

    return result;
}


//-----------------------------------------------------------------------------------------------
Vec2 ParseXmlAttribute( XmlElement const& element, char const* attributeName, Vec2 const& defaultValue )
{
    Vec2        result    = defaultValue;
    char const* value     = nullptr;
    XmlResult   xmlResult = element.QueryStringAttribute( attributeName, &value );
    if( xmlResult == XML_SUCCESS )
    {
        result.SetFromText( value );
    }
    return result;
}


//-----------------------------------------------------------------------------------------------
IntVec2 ParseXmlAttribute( XmlElement const& element, char const* attributeName, IntVec2 const& defaultValue )
{
    IntVec2     result    = defaultValue;
    char const* value     = nullptr;
    XmlResult   xmlResult = element.QueryStringAttribute( attributeName, &value );
    if( xmlResult == XML_SUCCESS )
    {
        result.SetFromText( value );
    }

    return result;
}


//-----------------------------------------------------------------------------------------------
Vec3 ParseXmlAttribute( XmlElement const& element, char const* attributeName, Vec3 const& defaultValue )
{
    Vec3        result    = defaultValue;
    char const* value     = nullptr;
    XmlResult   xmlResult = element.QueryStringAttribute( attributeName, &value );
    if( xmlResult == XML_SUCCESS )
    {
        result.SetFromText( value );
    }
    return result;
}


//-----------------------------------------------------------------------------------------------
EulerAngles ParseXmlAttribute( XmlElement const& element, char const* attributeName, EulerAngles const& defaultValue )
{
    EulerAngles result    = defaultValue;
    char const* value     = nullptr;
    XmlResult   xmlResult = element.QueryStringAttribute( attributeName, &value );
    if( xmlResult == XML_SUCCESS )
    {
        result.SetFromText( value );
    }
    return result;
}


//-----------------------------------------------------------------------------------------------
FloatRange ParseXmlAttribute( XmlElement const& element, char const* attributeName, FloatRange const& defaultValue, char delimiter )
{
    FloatRange  result    = defaultValue;
    char const* value     = nullptr;
    XmlResult   xmlResult = element.QueryStringAttribute( attributeName, &value );
    if( xmlResult == XML_SUCCESS )
    {
        result.SetFromText( value, delimiter );
    }
    return result;
}

//-----------------------------------------------------------------------------------------------
AABB2 ParseXmlAttribute( XmlElement const& element, char const* attributeName, AABB2 const& defaultValue )
{
    AABB2       result    = defaultValue;
    char const* value     = nullptr;
    XmlResult   xmlResult = element.QueryStringAttribute( attributeName, &value );
    if( xmlResult == XML_SUCCESS )
    {
        result.SetFromText( value );
    }

    return result;
}


//-----------------------------------------------------------------------------------------------
std::string ParseXmlAttribute( XmlElement const& element, char const* attributeName, char const* defaultValue )
{
    char const* value  = nullptr;
    XmlResult   result = element.QueryStringAttribute( attributeName, &value );

    if( result == XML_SUCCESS )
    {
        return value;
    }

    return defaultValue;
}


//-----------------------------------------------------------------------------------------------
std::string ParseXmlAttribute( XmlElement const& element, char const* attributeName, std::string const& defaultValue )
{
    char const* value  = nullptr;
    XmlResult   result = element.QueryStringAttribute( attributeName, &value );

    if( result == XML_SUCCESS )
    {
        return std::string( value );
    }

    return defaultValue;
}


//-----------------------------------------------------------------------------------------------
Strings ParseXmlAttribute( XmlElement const& element, char const* attributeName, Strings const& defaultValues, char delimiter /*= ',' */ )
{
    Strings     results   = defaultValues;
    char const* value     = nullptr;
    XmlResult   xmlResult = element.QueryStringAttribute( attributeName, &value );
    if( xmlResult == XML_SUCCESS )
    {
        results = SplitStringOnDelimiter( value, delimiter );
    }
    return results;
}
