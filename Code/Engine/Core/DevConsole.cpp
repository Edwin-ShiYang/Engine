#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Time.hpp"
#include "VertexUtils.hpp"

//-----------------------------------------------------------------------------------------------
Rgba8 const DevConsole::WARNING_COLOR( 255, 255, 255 );
Rgba8 const DevConsole::INFO_MINOR_COLOR( 250, 179, 135 );
Rgba8 const DevConsole::INFO_MAJOR_COLOR( 166, 227, 161 );
Rgba8 const DevConsole::ERROR_COLOR( 243, 139, 168 );
Rgba8 const DevConsole::BACKGROUND_COLOR( 17, 17, 27, 128 );
Rgba8 const DevConsole::INPUT_TEXT_COLOR( 205, 214, 244 );

//-----------------------------------------------------------------------------------------------
DevConsole::DevConsole( DevConsoleConfig const& config )
    : m_config( config )
{
}

//-----------------------------------------------------------------------------------------------
DevConsole::~DevConsole()
{
    delete m_insertionPointBlinkTimer;
    m_insertionPointBlinkTimer = nullptr;

    delete m_config.m_camera;
    m_config.m_camera = nullptr;
}

//-----------------------------------------------------------------------------------------------
void DevConsole::Startup()
{
    IntVec2 dimensions = g_engine->m_window->GetClientDimensions();
    m_config.m_camera  = new Camera( Vec2( 0.f, 0.f ), Vec2( static_cast< float >( dimensions.x ), static_cast< float >( dimensions.y ) ) );
    m_fontWidth        = m_config.m_fontAspect * m_config.m_camera->GetOrthographicTopRight().y / ( m_config.m_linesOnScreen );

    m_insertionPointBlinkTimer = new Timer( 0.5, &Clock::GetSystemClock() );
    m_insertionPointBlinkTimer->Start();

    GUARANTEE_OR_DIE( g_engine && g_engine->m_eventSystem, "DevConsole::Startup - g_engine or m_eventSystem is null" );
    g_engine->m_eventSystem->SubscribeEventCallbackFunction( "KeyPressed", Event_KeyPressed );
    g_engine->m_eventSystem->SubscribeEventCallbackFunction( "CharInput", Event_CharInput );
    g_engine->m_eventSystem->SubscribeEventCallbackFunction( "Help", Command_Help );
    g_engine->m_eventSystem->SubscribeEventCallbackFunction( "Clear", Command_Clear );

    if ( m_config.m_startOpen )
    {
        m_mode = OPEN_FULL;
    }
}

//-----------------------------------------------------------------------------------------------
Strings DevConsole::GetCommands( std::string const& consoleCommandText )
{
    Strings commands = SplitStringOnDelimiter( consoleCommandText, '\n' );
    return commands;
}

//-----------------------------------------------------------------------------------------------
void DevConsole::FireEventsWithArgs( std::string const& command )
{
    Strings   splitCommandBySpace = SplitStringOnDelimiter( command, ' ' );
    EventArgs args;

    for ( int argIndex = 1; argIndex < static_cast< int >( splitCommandBySpace.size() ); ++argIndex )
    {
        Strings arg = SplitStringOnDelimiter( splitCommandBySpace[ argIndex ], '=' );
        args.SetValue( arg[ 0 ], arg.size() > 1 ? arg[ 1 ] : "" );
    }

    std::vector< std::string > eventNames   = g_engine->m_eventSystem->GetSubscribedEvents();
    bool                       isFoundEvent = false;
    for ( int eventIndex = 0; eventIndex < static_cast< int >( eventNames.size() ); ++eventIndex )
    {
        if ( splitCommandBySpace[ 0 ] == eventNames[ eventIndex ] )
        {
            isFoundEvent = true;
            break;
        }
    }

    if ( isFoundEvent )
    {
        if ( static_cast< int >( m_commandHistory.size() ) >= m_config.m_maxCommandHistory )
        {
            m_commandHistory.erase( m_commandHistory.begin() );
        }
        m_commandHistory.push_back( command );

        if ( splitCommandBySpace[ 0 ] != "Clear" && splitCommandBySpace[ 0 ] != "Quit" )
        {
            AddLine( Rgba8( 250, 179, 135 ), Stringf( "[Echo]    %s", command.c_str() ) );
        }

        g_engine->m_eventSystem->FireEvent( splitCommandBySpace[ 0 ], args );
    }
    else
    {
        AddLine( ERROR_COLOR, Stringf( "[Error]   Unknown command: %s", command.c_str() ) );
    }
}

//-----------------------------------------------------------------------------------------------
bool DevConsole::HandleKeyPressed( unsigned char keyCode )
{
    m_insertionPointVisible = true;
    m_insertionPointBlinkTimer->Start();

    if ( keyCode == KEYCODE_TILDE )
    {
        return false;
    }

    if ( keyCode == KEYCODE_TILDE )
    {
        return false;
    }

    if ( keyCode == KEYCODE_ENTER )
    {
        Execute( m_inputText );
        return true;
    }

    if ( keyCode == KEYCODE_ESC )
    {
        if ( IsOpen() && !m_inputText.empty() )
        {
            m_inputText              = "";
            m_insertionPointPosition = 0;
        }
        else
        {
            SetMode( HIDDEN );
        }

        return true;
    }

    if ( keyCode == KEYCODE_LEFTARROW && m_insertionPointPosition > 0 )
    {
        m_insertionPointPosition--;
        return true;
    }

    if ( keyCode == KEYCODE_RIGHTARROW && m_insertionPointPosition < static_cast< int >( m_inputText.size() ) )
    {
        m_insertionPointPosition++;
        return true;
    }

    if ( keyCode == KEYCODE_BACKSPACE && m_insertionPointPosition > 0 )
    {
        m_inputText.erase( m_insertionPointPosition - 1, 1 );
        m_insertionPointPosition--;
        return true;
    }

    if ( keyCode == KEYCODE_DELETE && m_insertionPointPosition < static_cast< int >( m_inputText.size() ) )
    {
        m_inputText.erase( m_insertionPointPosition, 1 );
        return true;
    }

    if ( keyCode == KEYCODE_HOME )
    {
        m_insertionPointPosition = 0;
        return true;
    }

    if ( keyCode == KEYCODE_END )
    {
        m_insertionPointPosition = static_cast< int >( m_inputText.size() );
        return true;
    }

    if ( keyCode == KEYCODE_UPARROW )
    {
        if ( m_historyIndex > 0 )
        {
            m_historyIndex--;
            m_inputText              = m_commandHistory[ m_historyIndex ];
            m_insertionPointPosition = static_cast< int >( m_inputText.size() );
        }
    }

    if ( keyCode == KEYCODE_DOWNARROW )
    {
        if ( m_commandHistory.size() > 0 && m_historyIndex < static_cast< int >( m_commandHistory.size() ) - 1 )
        {
            m_historyIndex++;
            m_inputText              = m_commandHistory[ m_historyIndex ];
            m_insertionPointPosition = static_cast< int >( m_inputText.size() );
        }
    }

    // Prevent the key state from being changed by the console
    return true;
}

//-----------------------------------------------------------------------------------------------
bool DevConsole::HandleCharInput( unsigned char keyCode )
{
    m_insertionPointVisible = true;
    m_insertionPointBlinkTimer->Start();

    if ( keyCode < 32 || keyCode > 126 || keyCode == '`' || keyCode == '~' )
    {
        return true;
    }

    if ( m_fontWidth * m_inputText.size() < m_config.m_camera->GetOrthographicTopRight().x - m_fontWidth )
    {
        m_inputText.insert( m_insertionPointPosition, 1, keyCode );
        m_insertionPointPosition++;
    }

    return true;
}

//-----------------------------------------------------------------------------------------------
void DevConsole::PrintRegisteredCommands()
{
    std::vector< std::string > registeredCommands = g_engine->m_eventSystem->GetSubscribedEvents();
    AddLine( INFO_MAJOR_COLOR, "Registered Commands" );
    for ( int commandIndex = 0; commandIndex < static_cast< int >( registeredCommands.size() ); ++commandIndex )
    {
        AddLine( Rgba8( 137, 180, 250 ), "|" + registeredCommands[ commandIndex ] );
    }
}

//-----------------------------------------------------------------------------------------------
void DevConsole::Execute( std::string const& consoleCommandText )
{
    Strings commands = GetCommands( consoleCommandText );
    if ( consoleCommandText.empty() )
    {
        return;
    }

    for ( int commandIndex = 0; commandIndex < static_cast< int >( commands.size() ); ++commandIndex )
    {
        FireEventsWithArgs( commands[ commandIndex ] );
        m_inputText              = "";
        m_insertionPointPosition = 0;
    }
    m_historyIndex = static_cast< int >( m_commandHistory.size() );
}

//-----------------------------------------------------------------------------------------------
void DevConsole::AddLine( Rgba8 const& color, std::string const& text )
{
    DevConsoleLine line;
    line.m_color = color;
    line.m_text  = text;

    if ( static_cast< int >( m_lines.size() ) >= m_config.m_linesOnScreen - 1 )
    {
        m_lines.erase( m_lines.begin() );
    }

    m_lines.push_back( line );
}

//-----------------------------------------------------------------------------------------------				`
void DevConsole::Render( AABB2 const& bounds ) const
{
    g_engine->m_render->BeginCamera( *m_config.m_camera );
    g_engine->m_render->SetBlendMode( BlendMode::ALPHA );

    // background
    std::vector< Vertex > devConsoleVerts;
    AddVertsForAABB2D( devConsoleVerts, bounds, BACKGROUND_COLOR );
    g_engine->m_render->DrawVertexArray( devConsoleVerts );

    float                 cellHeight = bounds.m_maxs.y / m_config.m_linesOnScreen;
    Vec2                  alignment  = Vec2( 0.f, 0.f );

    // insertionPoint
    float                 offset = static_cast< float >( m_insertionPointPosition ) * m_fontWidth;
    std::vector< Vertex > insertionPointVerts;
    m_insertionPointBlinkTimer->DecrementPeriodIfElapsed();

    if ( m_insertionPointVisible )
    {
        AABB2 insertionPoint = AABB2( Vec2( offset, 0.f ), Vec2( offset + cellHeight * 0.15f, cellHeight ) );
        AddVertsForAABB2D( insertionPointVerts, insertionPoint, Rgba8( 242, 205, 205 ) );
        g_engine->m_render->DrawVertexArray( insertionPointVerts );
    }

    BitmapFont* font = g_engine->m_render->m_loadedFontsByName[ m_config.m_fontName ];
    g_engine->m_render->BindTexture( &font->GetTexture() );

    std::vector< Vertex > textVertexArray;
    font->AddVertsForTextInBox2D( textVertexArray, m_inputText, AABB2( Vec2( 0.f, 0.f ), Vec2( bounds.m_maxs.x, cellHeight ) ), cellHeight, INPUT_TEXT_COLOR, m_config.m_fontAspect, alignment, SHRINK_TO_FIT );
    AABB2 currentBounds = AABB2( Vec2( 0.f, cellHeight ), Vec2( bounds.m_maxs.x, cellHeight * 2.f ) );

    for ( int lineIndex = static_cast< int >( m_lines.size() ) - 1; lineIndex >= 0; --lineIndex )
    {
        font->AddVertsForTextInBox2D( textVertexArray, m_lines[ lineIndex ].m_text, currentBounds, cellHeight, m_lines[ lineIndex ].m_color, m_config.m_fontAspect, alignment, SHRINK_TO_FIT );
        currentBounds.m_mins.y += ( cellHeight );
        currentBounds.m_maxs.y += ( cellHeight );
    }

    g_engine->m_render->DrawVertexArray( textVertexArray );

    g_engine->m_render->BindTexture( nullptr );
    g_engine->m_render->EndCamera( *m_config.m_camera );
}

//-----------------------------------------------------------------------------------------------
DevConsoleMode DevConsole::GetMode() const
{
    return m_mode;
}

//-----------------------------------------------------------------------------------------------
void DevConsole::SetMode( DevConsoleMode mode )
{
    m_mode = mode;
}

//-----------------------------------------------------------------------------------------------
void DevConsole::ToggleOpen( DevConsoleMode mode )
{
    if ( m_mode == mode )
    {
        m_mode                   = HIDDEN;
        m_inputText              = "";
        m_insertionPointPosition = 0;
    }
    else
    {
        m_mode = mode;
    }
}

//-----------------------------------------------------------------------------------------------
bool DevConsole::IsOpen()
{
    return m_mode == OPEN_FULL;
}

//-----------------------------------------------------------------------------------------------
void DevConsole::Shutdown()
{
}

//-----------------------------------------------------------------------------------------------
void DevConsole::BeginFrame()
{
    m_frameNumber++;
    if ( m_insertionPointBlinkTimer->DecrementPeriodIfElapsed() )
    {
        m_insertionPointVisible = !m_insertionPointVisible;
    }
}

//-----------------------------------------------------------------------------------------------
void DevConsole::EndFrame()
{
    IntVec2 dimensions = g_engine->m_window->GetClientDimensions();
    m_config.m_camera->SetOrthographicView( Vec2( 0.f, 0.f ), Vec2( static_cast< float >( dimensions.x ), static_cast< float >( dimensions.y ) ) );
}

//-----------------------------------------------------------------------------------------------
bool DevConsole::Event_KeyPressed( EventArgs& args )
{
    if ( !g_engine->m_devConsole || !g_engine->m_devConsole->IsOpen() || !g_engine->m_input )
    {
        return false;
    }

    unsigned char keyCode = static_cast< unsigned char >( args.GetValue( "KeyCode", -1 ) );
    return g_engine->m_devConsole->HandleKeyPressed( keyCode );
}

//-----------------------------------------------------------------------------------------------
bool DevConsole::Event_CharInput( EventArgs& args )
{
    if ( !g_engine->m_devConsole || !g_engine->m_devConsole->IsOpen() || !g_engine->m_input )
    {
        return false;
    }

    unsigned char keyCode = static_cast< unsigned char >( args.GetValue( "KeyCode", -1 ) );
    return g_engine->m_devConsole->HandleCharInput( keyCode );
}

//-----------------------------------------------------------------------------------------------
bool DevConsole::Command_Clear( [[maybe_unused]] EventArgs& args )
{
    DevConsole* devConsole = g_engine->m_devConsole;

    if ( !devConsole || !g_engine->m_devConsole->IsOpen() )
    {
        return false;
    }
    devConsole->m_lines.clear();
    DebuggerPrintf( "DevConsole::Command_Clear: Consumed\n" );
    return false;
}

//-----------------------------------------------------------------------------------------------
bool DevConsole::Command_Help( [[maybe_unused]] EventArgs& args )
{
    DevConsole* devConsole = g_engine->m_devConsole;
    if ( !devConsole || !g_engine->m_devConsole->IsOpen() || !g_engine->m_eventSystem )
    {
        return false;
    }
    devConsole->PrintRegisteredCommands();
    return true;
}