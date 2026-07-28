#include "Engine/Renderer/DebugRenderSystem.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <vector>

//----------------------------------------------------------------------------------------------
struct DebugObject
{
    Rgba8                       m_startColor;
    Rgba8                       m_endColor;
    std::vector< Vertex >       m_verts;
    std::vector< unsigned int > m_indices;
    Timer                       m_timer = Timer( 0.f );
    float                       m_duration;
    Vec3                        m_position;
    bool                        m_isWireframe = false;
    EulerAngles                 m_orientation;
    DebugRenderMode             m_mode;
};

//----------------------------------------------------------------------------------------------
struct DebugText : DebugObject
{
    std::string m_text;
    float       m_cellHeight      = 15.f;
    float       m_cellAspectScale = 1.0f;
    int         m_maxGlyphsToDraw = 999;
};

//----------------------------------------------------------------------------------------------
struct DebugWorldText : DebugText
{
};

//----------------------------------------------------------------------------------------------
struct DebugWorldBillboardText : DebugText
{
    BillboardType m_billboardType = BillboardType::FULL_OPPOSING;
    Vec3          m_origin;
    Vec2          m_alignment;
};

//----------------------------------------------------------------------------------------------
struct DebugScreenText : DebugText
{
};

//----------------------------------------------------------------------------------------------
struct DebugMessage : DebugScreenText
{
    Vec2 m_textMins = Vec2::ZERO;
};

//-----------------------------------------------------------------------------------------------
static std::vector< DebugObject >             s_debugObjects;
static std::vector< DebugScreenText >         s_debugScreenTexts;
static std::vector< DebugMessage >            s_debugMessages;
static std::vector< DebugWorldText >          s_debugWorldTexts;
static std::vector< DebugWorldBillboardText > s_debugWorldBillboardTexts;
static bool                                   s_isVisible = true;
static DebugRenderConfig                      s_config;

//-----------------------------------------------------------------------------------------------
void                                          InitializeDebugObject( DebugObject& debugObject, Rgba8 const& startColor, Rgba8 const& endColor, float duration, DebugRenderMode const& mode = DebugRenderMode::USE_DEPTH )
{
    debugObject.m_startColor = startColor;
    debugObject.m_endColor   = endColor;
    debugObject.m_duration   = duration;
    debugObject.m_mode       = mode;

    if ( duration > 0.f )
    {
        debugObject.m_timer.m_period = duration;
        debugObject.m_timer.Start();
    }
}

//-----------------------------------------------------------------------------------------------
void RestoreDefaultRenderStates()
{
    g_engine->m_render->SetBlendMode( BlendMode::ALPHA );
    g_engine->m_render->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
    g_engine->m_render->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
}

//-----------------------------------------------------------------------------------------------
Mat44 GetModelToWorldTransform( DebugObject const& debugObject )
{
    Mat44 transform;
    Mat44 orientation = debugObject.m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
    transform.AppendTranslation3D( debugObject.m_position );
    transform.Append( orientation );
    return transform;
}

//-----------------------------------------------------------------------------------------------
Rgba8 GetDebugObjectColor( DebugObject const& debugObject )
{
    Rgba8 color;
    if ( debugObject.m_duration <= 0 )
    {
        color = debugObject.m_startColor;
    }
    else
    {
        color = Interpolate( debugObject.m_startColor, debugObject.m_endColor, static_cast< float >( debugObject.m_timer.GetElapsedFraction() ) );
    }

    return color;
}

//-----------------------------------------------------------------------------------------------
void RenderDebugObject( DebugObject& debugObject )
{
    if ( debugObject.m_isWireframe )
    {
        g_engine->m_render->SetRasterizerMode( RasterizerMode::WIREFRAME_CULL_BACK );
    }

    switch ( debugObject.m_mode )
    {
        case DebugRenderMode::USE_DEPTH:
            {
                g_engine->m_render->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );

                Rgba8 color     = GetDebugObjectColor( debugObject );
                Mat44 transform = GetModelToWorldTransform( debugObject );

                g_engine->m_render->SetModelConstants( transform, color );
                if ( debugObject.m_indices.size() > 0 )
                {
                    g_engine->m_render->DrawIndexedVertexArray( debugObject.m_verts, debugObject.m_indices );
                }
                else
                {
                    g_engine->m_render->DrawVertexArray( debugObject.m_verts );
                }

                break;
            }

        case DebugRenderMode::ALWAYS:
            {
                g_engine->m_render->SetDepthMode( DepthMode::DISABLED );

                Rgba8 color     = GetDebugObjectColor( debugObject );
                Mat44 transform = GetModelToWorldTransform( debugObject );

                g_engine->m_render->SetModelConstants( transform, color );
                g_engine->m_render->DrawVertexArray( debugObject.m_verts );
                break;
            }

        case DebugRenderMode::X_RAY:
            {
                Rgba8 color     = GetDebugObjectColor( debugObject );
                Mat44 transform = GetModelToWorldTransform( debugObject );

                // pass1
                g_engine->m_render->SetBlendMode( BlendMode::ALPHA );
                g_engine->m_render->SetDepthMode( DepthMode::READ_ONLY_ALWAYS );

                Rgba8 xRayColor = color;
                xRayColor.r     = static_cast< unsigned char >( ( xRayColor.r + 255 ) / 2 );
                xRayColor.g     = static_cast< unsigned char >( ( xRayColor.g + 255 ) / 2 );
                xRayColor.b     = static_cast< unsigned char >( ( xRayColor.b + 255 ) / 2 );
                xRayColor.a     = 128;

                g_engine->m_render->SetModelConstants( transform, xRayColor );
                g_engine->m_render->DrawVertexArray( debugObject.m_verts );

                // pass2
                g_engine->m_render->SetBlendMode( BlendMode::OPAQUE );
                g_engine->m_render->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );

                g_engine->m_render->SetModelConstants( transform, color );
                g_engine->m_render->DrawVertexArray( debugObject.m_verts );
                break;
            }
        default:
            ERROR_AND_DIE( "Unknown DebugRenderMode" );
            break;
    }

    RestoreDefaultRenderStates();
}

//-----------------------------------------------------------------------------------------------
void DebugRenderSystemStartup( DebugRenderConfig const& config )
{
    s_config = config;

    std::string fontPathWithoutExtension = config.m_fontPath + config.m_fontName;
    std::string fontPathWithExtension    = fontPathWithoutExtension + ".png";

    g_engine->m_render->CreateOrGetTextureFromFile( fontPathWithExtension.c_str() );
    g_engine->m_render->CreateOrGetBitmapFont( fontPathWithoutExtension.c_str(), config.m_fontName );

    // std::string m_fontPath = "Data/Fonts/";
    // std::string m_fontName = "SquirrelFixedFont";

    SubscribeEventCallbackFunction( "Clear", Command_DebugRenderClear );
    SubscribeEventCallbackFunction( "Toggle", Command_DebugRenderToggle );
}

//-----------------------------------------------------------------------------------------------
void DebugRenderSystemShutdown()
{
    DebugRenderSetHidden();
    DebugRenderClear();
}

//-----------------------------------------------------------------------------------------------
void DebugRenderSetVisible()
{
    s_isVisible = true;
}

//-----------------------------------------------------------------------------------------------
void DebugRenderSetHidden()
{
    s_isVisible = false;
}

//-----------------------------------------------------------------------------------------------
void DebugRenderClear()
{
    s_debugObjects.clear();
    s_debugScreenTexts.clear();
    s_debugMessages.clear();
    s_debugWorldTexts.clear();
    s_debugWorldBillboardTexts.clear();
}

//-----------------------------------------------------------------------------------------------
void RenderDebugMessages( Vec2& startPos )
{
    Vec2        margin = Vec2( 5.f, 5.f );

    BitmapFont* font              = g_engine->m_render->m_loadedFontsByName[ s_config.m_fontName ];
    int         debugMessageIndex = 0;
    Vec2        textMins          = Vec2::ZERO;

    for ( int i = 0; i < static_cast< int >( s_debugMessages.size() ); ++i )
    {
        DebugMessage& debugMessage = s_debugMessages[ i ];
        if ( debugMessage.m_duration == 0.f )
        {
            debugMessage.m_textMins = Vec2( startPos.x + margin.x, startPos.y - ( debugMessage.m_cellHeight + margin.y ) * static_cast< float >( debugMessageIndex + 1 ) );
            font->AddVertsForText2D( debugMessage.m_verts, debugMessage.m_textMins, debugMessage.m_cellHeight, debugMessage.m_text, debugMessage.m_maxGlyphsToDraw, debugMessage.m_startColor, debugMessage.m_cellAspectScale );
            g_engine->m_render->DrawVertexArray( debugMessage.m_verts );
            debugMessageIndex++;
        }
    }

    for ( int i = 0; i < static_cast< int >( s_debugMessages.size() ); ++i )
    {
        DebugMessage& debugMessage = s_debugMessages[ i ];
        if ( debugMessage.m_duration > 0.f )
        {
            debugMessage.m_textMins = Vec2( startPos.x + margin.x, startPos.y - ( debugMessage.m_cellHeight + margin.y ) * static_cast< float >( debugMessageIndex + 1 ) );
            font->AddVertsForText2D( debugMessage.m_verts, debugMessage.m_textMins, debugMessage.m_cellHeight, debugMessage.m_text, debugMessage.m_maxGlyphsToDraw, debugMessage.m_startColor, debugMessage.m_cellAspectScale );
            g_engine->m_render->DrawVertexArray( debugMessage.m_verts );
            debugMessageIndex++;
        }
    }
}

//-----------------------------------------------------------------------------------------------
template < typename T >
void RemoveExpiredDebugObjects( T& debugObjects )
{
    for ( auto it = debugObjects.begin(); it != debugObjects.end(); )
    {
        if ( it->m_duration == 0.f || it->m_timer.HasPeriodElapsed() )
        {
            it = debugObjects.erase( it );
        }
        else
        {
            ++it;
        }
    }
}

//-----------------------------------------------------------------------------------------------
void DebugRenderBeginFrame()
{
    RemoveExpiredDebugObjects( s_debugObjects );
    RemoveExpiredDebugObjects( s_debugScreenTexts );
    RemoveExpiredDebugObjects( s_debugMessages );
    RemoveExpiredDebugObjects( s_debugWorldTexts );
    RemoveExpiredDebugObjects( s_debugWorldBillboardTexts );
}

//-----------------------------------------------------------------------------------------------
void DebugRenderEndFrame()
{
    for ( int i = 0; i < static_cast< int >( s_debugMessages.size() ); ++i )
    {
        s_debugMessages[ i ].m_verts.clear();
        s_debugMessages[ i ].m_maxGlyphsToDraw = 999;
    }

    for ( int i = 0; i < static_cast< int >( s_debugWorldBillboardTexts.size() ); ++i )
    {
        s_debugWorldBillboardTexts[ i ].m_verts.clear();
        s_debugWorldBillboardTexts[ i ].m_maxGlyphsToDraw = 999;
    }
}

//-----------------------------------------------------------------------------------------------
void DebugRenderWorld( Camera const& camera )
{
    if ( !s_isVisible )
    {
        return;
    }

    g_engine->m_render->BeginCamera( camera );

    for ( int debugObjectIndex = 0; debugObjectIndex < static_cast< int >( s_debugObjects.size() ); ++debugObjectIndex )
    {
        DebugObject& debugObject = s_debugObjects[ debugObjectIndex ];
        RenderDebugObject( debugObject );
    }

    BitmapFont* font        = g_engine->m_render->m_loadedFontsByName[ s_config.m_fontName ];
    Texture&    fontTexture = font->GetTexture();
    g_engine->m_render->BindTexture( &fontTexture );

    for ( int debugWorldTextIndex = 0; debugWorldTextIndex < static_cast< int >( s_debugWorldTexts.size() ); ++debugWorldTextIndex )
    {
        DebugWorldText& debugWorldText = s_debugWorldTexts[ debugWorldTextIndex ];
        g_engine->m_render->SetRasterizerMode( RasterizerMode::SOLID_CULL_NONE );
        RenderDebugObject( debugWorldText );
    }

    for ( int debugWorldBillboardTextIndex = 0; debugWorldBillboardTextIndex < static_cast< int >( s_debugWorldBillboardTexts.size() ); ++debugWorldBillboardTextIndex )
    {
        DebugWorldBillboardText& debugWorldBillboardText = s_debugWorldBillboardTexts[ debugWorldBillboardTextIndex ];
        font->AddVertsForText3DAtOriginXForward( debugWorldBillboardText.m_verts, debugWorldBillboardText.m_cellHeight, debugWorldBillboardText.m_text, debugWorldBillboardText.m_startColor, debugWorldBillboardText.m_cellAspectScale, debugWorldBillboardText.m_alignment );

        Mat44 billboardTransform = GetBillboard( debugWorldBillboardText.m_billboardType, camera.GetCameraToWorldTransform(), debugWorldBillboardText.m_origin );
        TransformVertexArray3D( debugWorldBillboardText.m_verts, billboardTransform );
        g_engine->m_render->SetRasterizerMode( RasterizerMode::SOLID_CULL_NONE );
        RenderDebugObject( debugWorldBillboardText );
    }

    g_engine->m_render->BindTexture( nullptr );
    g_engine->m_render->EndCamera( camera );
}

//-----------------------------------------------------------------------------------------------
void DebugRenderWorld( Camera const& camera, Viewport viewport )
{
    if ( !s_isVisible )
    {
        return;
    }

    g_engine->m_render->BeginCamera( camera, viewport );

    for ( int debugObjectIndex = 0; debugObjectIndex < static_cast< int >( s_debugObjects.size() ); ++debugObjectIndex )
    {
        DebugObject& debugObject = s_debugObjects[ debugObjectIndex ];
        RenderDebugObject( debugObject );
    }

    BitmapFont* font        = g_engine->m_render->m_loadedFontsByName[ s_config.m_fontName ];
    Texture&    fontTexture = font->GetTexture();
    g_engine->m_render->BindTexture( &fontTexture );

    for ( int debugWorldTextIndex = 0; debugWorldTextIndex < static_cast< int >( s_debugWorldTexts.size() ); ++debugWorldTextIndex )
    {
        DebugWorldText& debugWorldText = s_debugWorldTexts[ debugWorldTextIndex ];
        g_engine->m_render->SetRasterizerMode( RasterizerMode::SOLID_CULL_NONE );
        RenderDebugObject( debugWorldText );
    }

    for ( int debugWorldBillboardTextIndex = 0; debugWorldBillboardTextIndex < static_cast< int >( s_debugWorldBillboardTexts.size() ); ++debugWorldBillboardTextIndex )
    {
        DebugWorldBillboardText& debugWorldBillboardText = s_debugWorldBillboardTexts[ debugWorldBillboardTextIndex ];
        font->AddVertsForText3DAtOriginXForward( debugWorldBillboardText.m_verts, debugWorldBillboardText.m_cellHeight, debugWorldBillboardText.m_text, debugWorldBillboardText.m_startColor, debugWorldBillboardText.m_cellAspectScale, debugWorldBillboardText.m_alignment );

        Mat44 billboardTransform = GetBillboard( debugWorldBillboardText.m_billboardType, camera.GetCameraToWorldTransform(), debugWorldBillboardText.m_origin );
        TransformVertexArray3D( debugWorldBillboardText.m_verts, billboardTransform );
        g_engine->m_render->SetRasterizerMode( RasterizerMode::SOLID_CULL_NONE );
        RenderDebugObject( debugWorldBillboardText );
    }

    g_engine->m_render->BindTexture( nullptr );
    g_engine->m_render->EndCamera( camera );
}

//-----------------------------------------------------------------------------------------------
void DebugRenderScreen( Camera const& camera )
{
    if ( !s_isVisible )
    {
        return;
    }

    g_engine->m_render->BeginCamera( camera );

    BitmapFont* font        = g_engine->m_render->m_loadedFontsByName[ s_config.m_fontName ];
    Texture&    fontTexture = font->GetTexture();
    g_engine->m_render->BindTexture( &fontTexture );

    for ( int debugScreenTextIndex = 0; debugScreenTextIndex < static_cast< int >( s_debugScreenTexts.size() ); ++debugScreenTextIndex )
    {
        DebugScreenText& debugScreenText = s_debugScreenTexts[ debugScreenTextIndex ];
        g_engine->m_render->DrawVertexArray( debugScreenText.m_verts );
    }

    Vec2 startPos = Vec2( camera.GetOrthographicBottomLeft().x, camera.GetOrthographicTopRight().y );
    RenderDebugMessages( startPos );

    g_engine->m_render->BindTexture( nullptr );
    g_engine->m_render->EndCamera( camera );
}

//-----------------------------------------------------------------------------------------------
void DebugAddWorldSphere( Vec3 const& center, float radius, float duration, Rgba8 const& startColor /*= Rgba8::WHITE*/, Rgba8 const& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
    DebugObject debugObject;
    InitializeDebugObject( debugObject, startColor, endColor, duration, mode );

    AddVertsForUVSphereZ3D( debugObject.m_verts, center, radius );
    s_debugObjects.push_back( debugObject );
}

//-----------------------------------------------------------------------------------------------
void DebugAddWorldWireSphere( Vec3 const& center, float radius, float duration, Rgba8 const& startColor /*= Rgba8::WHITE*/, Rgba8 const& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
    DebugObject debugObject;
    InitializeDebugObject( debugObject, startColor, endColor, duration, mode );
    debugObject.m_isWireframe = true;

    AddVertsForUVSphereZ3D( debugObject.m_verts, center, radius );
    s_debugObjects.push_back( debugObject );
}

//-----------------------------------------------------------------------------------------------
void DebugAddWorldCylinder( Vec3 const& start, Vec3 const& end, float radius, float duration, Rgba8 const& startColor /*= Rgba8::WHITE*/, Rgba8 const& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
    DebugObject debugObject;
    InitializeDebugObject( debugObject, startColor, endColor, duration, mode );

    AddVertsForCylinder3D( debugObject.m_verts, start, end, radius );
    s_debugObjects.push_back( debugObject );
}

//-----------------------------------------------------------------------------------------------
void DebugAddWorldWireCapsule3D( Vec2 const& centerXY, FloatRange const& minMaxz, float radius, float duration, Rgba8 const& startColor /*= Rgba8::WHITE*/, Rgba8 const& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
    DebugObject debugObject;
    InitializeDebugObject( debugObject, startColor, endColor, duration, mode );
    debugObject.m_isWireframe = true;

    AddVertsForCapsule3D( debugObject.m_verts, debugObject.m_indices, centerXY, minMaxz, radius );
    s_debugObjects.push_back( debugObject );
}

//-----------------------------------------------------------------------------------------------
void DebugAddWorldWireCylinder( Vec3 const& start, Vec3 const& end, float radius, float duration, Rgba8 const& startColor /*= Rgba8::WHITE*/, Rgba8 const& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
    DebugObject debugObject;
    InitializeDebugObject( debugObject, startColor, endColor, duration, mode );
    debugObject.m_isWireframe = true;

    AddVertsForCylinder3D( debugObject.m_verts, start, end, radius );
    s_debugObjects.push_back( debugObject );
}

//-----------------------------------------------------------------------------------------------
void DebugAddWorldLine3D( Vec3 const& start, Vec3 const& end, float thickness, float duration, Rgba8 const& startColor /*= Rgba8::WHITE*/, Rgba8 const& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
    DebugObject debugObject;
    InitializeDebugObject( debugObject, startColor, endColor, duration, mode );
    AddVertsForLineSegment3D( debugObject.m_verts, start, end, thickness, startColor );
    s_debugObjects.push_back( debugObject );
}

//-----------------------------------------------------------------------------------------------
void DebugAddWorldArrow( Vec3 const& start, Vec3 const& end, float radius, float duration, Rgba8 const& startColor /*= Rgba8::WHITE*/, Rgba8 const& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
    DebugObject debugObject;
    InitializeDebugObject( debugObject, startColor, endColor, duration, mode );
    AddVertsForArrow3D( debugObject.m_verts, start, end, radius );
    s_debugObjects.push_back( debugObject );
}

//-----------------------------------------------------------------------------------------------
void DebugAddWorldWireArrow( Vec3 const& start, Vec3 const& end, float radius, float duration, Rgba8 const& startColor /*= Rgba8::WHITE*/, Rgba8 const& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
    DebugObject debugObject;
    InitializeDebugObject( debugObject, startColor, endColor, duration, mode );
    debugObject.m_isWireframe = true;

    AddVertsForArrow3D( debugObject.m_verts, start, end, radius );
    s_debugObjects.push_back( debugObject );
}

//-----------------------------------------------------------------------------------------------
void DebugAddBasis( Mat44 const& transform, float duration, float length, float radius, float colorScale /*= 1.0f*/, float alphaScale /*= 1.0f*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
    DebugObject debugObject;
    debugObject.m_mode     = mode;
    debugObject.m_duration = duration;

    Rgba8 forwardVectorColor = Rgba8::RED;
    forwardVectorColor.ScaleRGBA( colorScale, alphaScale );

    Rgba8 leftVectorColor = Rgba8::GREEN;
    leftVectorColor.ScaleRGBA( colorScale, alphaScale );

    Rgba8 upVectorColor = Rgba8::BLUE;
    upVectorColor.ScaleRGBA( colorScale, alphaScale );

    AddVertsForArrow3D( debugObject.m_verts, Vec3::ZERO, Vec3::WORLD_FORWARD * length, radius, forwardVectorColor );
    AddVertsForArrow3D( debugObject.m_verts, Vec3::ZERO, Vec3::WORLD_LEFT * length, radius, leftVectorColor );
    AddVertsForArrow3D( debugObject.m_verts, Vec3::ZERO, Vec3::WORLD_UP * length, radius, upVectorColor );
    TransformVertexArray3D( debugObject.m_verts, transform );

    s_debugObjects.push_back( debugObject );
}

//-----------------------------------------------------------------------------------------------
void DebugAddWorldBasis( Mat44 const& transform, float duration, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
    DebugObject debugObject;
    debugObject.m_mode     = mode;
    debugObject.m_duration = duration;

    AddVertsForArrow3D( debugObject.m_verts, Vec3::ZERO, Vec3::WORLD_FORWARD * 2.0f, 0.2f, Rgba8::RED );
    AddVertsForArrow3D( debugObject.m_verts, Vec3::ZERO, Vec3::WORLD_LEFT * 2.0f, 0.2f, Rgba8::GREEN );
    AddVertsForArrow3D( debugObject.m_verts, Vec3::ZERO, Vec3::WORLD_UP * 2.0f, 0.2f, Rgba8::BLUE );
    TransformVertexArray3D( debugObject.m_verts, transform );

    s_debugObjects.push_back( debugObject );
}

//-----------------------------------------------------------------------------------------------
void DebugAddWorldText( std::string const& text, Mat44 const& transform, float textHeight, Vec2 const& alignment, float duration, Rgba8 const& startColor /*= Rgba8::WHITE*/, Rgba8 const& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
    DebugWorldText debugWorldText;
    InitializeDebugObject( debugWorldText, startColor, endColor, duration, mode );

    BitmapFont* font = g_engine->m_render->m_loadedFontsByName[ s_config.m_fontName ];
    font->AddVertsForText3DAtOriginXForward( debugWorldText.m_verts, textHeight, text, startColor, debugWorldText.m_cellAspectScale, alignment );
    TransformVertexArray3D( debugWorldText.m_verts, transform );

    s_debugWorldTexts.push_back( debugWorldText );
}

//-----------------------------------------------------------------------------------------------
void DebugAddWorldBillboardText( std::string const& text, Vec3 const& origin, float textHeight, Vec2 const& alignment, float duration, Rgba8 const& startColor /*= Rgba8::WHITE*/, Rgba8 const& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
    DebugWorldBillboardText debugWorldBillboardText;
    InitializeDebugObject( debugWorldBillboardText, startColor, endColor, duration, mode );

    debugWorldBillboardText.m_origin     = origin;
    debugWorldBillboardText.m_text       = text;
    debugWorldBillboardText.m_cellHeight = textHeight;
    debugWorldBillboardText.m_alignment  = alignment;

    s_debugWorldBillboardTexts.push_back( debugWorldBillboardText );
}

//-----------------------------------------------------------------------------------------------
void DebugAddScreenText( std::string const& text, AABB2 const& box, float cellHeight, Vec2 const& alignment, float duration, Rgba8 const& startColor /*= Rgba8::WHITE*/, Rgba8 const& endColor /*= Rgba8::WHITE */ )
{
    DebugScreenText debugScreenText;
    InitializeDebugObject( debugScreenText, startColor, endColor, duration );

    BitmapFont* font = g_engine->m_render->m_loadedFontsByName[ s_config.m_fontName ];
    font->AddVertsForTextInBox2D( debugScreenText.m_verts, text, box, cellHeight, debugScreenText.m_startColor, debugScreenText.m_cellAspectScale, alignment );
    s_debugScreenTexts.push_back( debugScreenText );
}

//-----------------------------------------------------------------------------------------------
void DebugAddMessage( std::string const& text, float duration, Rgba8 const& startColor /*= Rgba8::WHITE*/, Rgba8 const& endColor /*= Rgba8::WHITE */ )
{
    DebugMessage debugMessage;
    InitializeDebugObject( debugMessage, startColor, endColor, duration );
    debugMessage.m_text = text;

    s_debugMessages.push_back( debugMessage );
}

//-----------------------------------------------------------------------------------------------
bool Command_DebugRenderClear( [[maybe_unused]] EventArgs& args )
{
    DebugRenderClear();
    DebuggerPrintf( "Command_DebugRenderClear: Consumed\n" );
    return true;
}

//-----------------------------------------------------------------------------------------------
bool Command_DebugRenderToggle( [[maybe_unused]] EventArgs& args )
{
    if ( s_isVisible )
    {
        DebugRenderSetHidden();
    }
    else
    {
        DebugRenderSetVisible();
    }

    return true;
}