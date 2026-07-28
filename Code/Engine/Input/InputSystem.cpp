#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <Windows.h>

//-----------------------------------------------------------------------------------------------
unsigned char const KEYCODE_F1 = VK_F1;
unsigned char const KEYCODE_F2 = VK_F2;
unsigned char const KEYCODE_F3 = VK_F3;
unsigned char const KEYCODE_F4 = VK_F4;
unsigned char const KEYCODE_F5 = VK_F5;
unsigned char const KEYCODE_F6 = VK_F6;
unsigned char const KEYCODE_F7 = VK_F7;
unsigned char const KEYCODE_F8 = VK_F8;
unsigned char const KEYCODE_F9 = VK_F9;
unsigned char const KEYCODE_F10 = VK_F10;
unsigned char const KEYCODE_F11 = VK_F11;
unsigned char const KEYCODE_ESC = VK_ESCAPE;

unsigned char const KEYCODE_TILDE = 0xC0; //VK_OEM_3
unsigned char const KEYCODE_UPARROW = VK_UP;
unsigned char const KEYCODE_DOWNARROW = VK_DOWN;
unsigned char const KEYCODE_LEFTARROW = VK_LEFT;
unsigned char const KEYCODE_RIGHTARROW = VK_RIGHT;
unsigned char const KEYCODE_ENTER = VK_RETURN;
unsigned char const KEYCODE_BACKSPACE = VK_BACK;
unsigned char const KEYCODE_INSERT = VK_INSERT;
unsigned char const KEYCODE_DELETE = VK_DELETE;
unsigned char const KEYCODE_HOME = VK_HOME;
unsigned char const KEYCODE_END = VK_END;
unsigned char const KEYCODE_SLASH = VK_OEM_2;

unsigned char const KEYCODE_LEFT_MOUSE = VK_LBUTTON;
unsigned char const KEYCODE_RIGHT_MOUSE = VK_RBUTTON;
unsigned char const KEYCODE_SPACE = VK_SPACE;
unsigned char const KEYCODE_SHIFT = VK_SHIFT;

unsigned char const KEYCODE_LEFTBRACKET = VK_OEM_4;
unsigned char const KEYCODE_RIGHTBRACKET = VK_OEM_6;

unsigned char const KEYCODE_CONTROL = VK_CONTROL;

//-----------------------------------------------------------------------------------------------
InputSystem::InputSystem( InputConfig const& config )
	:m_config( config )
{
}

//-----------------------------------------------------------------------------------------------
InputSystem::~InputSystem()
{
}

//-----------------------------------------------------------------------------------------------
void InputSystem::Startup()
{
	GUARANTEE_OR_DIE( g_engine && g_engine->m_eventSystem, "InputSystem::Startup - g_engine or m_eventSystem is null" );
	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "KeyPressed", InputSystem::Event_KeyPressed );
	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "KeyReleased", InputSystem::Event_KeyReleased );

	m_currentCursorPos = CursorScreenToClient();
	m_previousCursorPos = m_currentCursorPos;
}

//-----------------------------------------------------------------------------------------------
void InputSystem::Shutdown()
{
}

//-----------------------------------------------------------------------------------------------
void InputSystem::BeginFrame()
{
	for ( int controllerIndex = 0; controllerIndex < NUM_XBOX_CONTROLLERS; ++ controllerIndex )
	{
		m_controllers [ controllerIndex ].RefreshStatus();
	}

	CorrectCursorVisibility();

	m_previousCursorPos = m_currentCursorPos;
	m_currentCursorPos = CursorScreenToClient();

	if ( m_cursorState.m_cursorMode == CursorMode::FPS && g_engine->m_window->HasFocus() )
	{
		m_cursorState.m_cursorClientDelta = m_currentCursorPos - m_previousCursorPos;
		IntVec2 clientCenter = g_engine->m_window->GetClientCenter();
		POINT centerPoint = { clientCenter.x, clientCenter.y };
		HWND hwnd = static_cast< HWND >( g_engine->m_window->m_windowHandle );
		ClientToScreen( hwnd, &centerPoint );
		SetCursorPos( centerPoint.x, centerPoint.y );
		m_currentCursorPos = CursorScreenToClient();
	}
	else
	{
		m_cursorState.m_cursorClientDelta = IntVec2( 0, 0 );
	}
}

//-----------------------------------------------------------------------------------------------
void InputSystem::EndFrame()
{
	UpdateLastFrameKeys();
	UpdateLastFrameControllersButtons();
}

//-----------------------------------------------------------------------------------------------
bool InputSystem::WasKeyJustPressed( unsigned char keyCode )
{
	return !m_keyStates [ keyCode ].m_wasPressedLastFrame && m_keyStates [ keyCode ].m_isPressed;
}

//-----------------------------------------------------------------------------------------------
bool InputSystem::WasKeyJustReleased( unsigned char keyCode )
{
	return m_keyStates [ keyCode ].m_wasPressedLastFrame && !m_keyStates [ keyCode ].m_isPressed;
}

//-----------------------------------------------------------------------------------------------

void InputSystem::UpdateLastFrameKeys()
{
	for ( int keyIndex = 0; keyIndex < NUM_KEYCODES; ++ keyIndex )
	{
		m_keyStates [ keyIndex ].m_wasPressedLastFrame = m_keyStates [ keyIndex ].m_isPressed;
	}
}

//-----------------------------------------------------------------------------------------------
void InputSystem::UpdateLastFrameControllersButtons()
{
	for ( int controllerIndex = 0; controllerIndex < NUM_XBOX_CONTROLLERS; ++ controllerIndex )
	{
		if ( m_controllers [ controllerIndex ].IsConnected() )
		{
			m_controllers [ controllerIndex ].UpdateLastFrameButtons();
		}
	}
}

//-----------------------------------------------------------------------------------------------
void InputSystem::SetCursorMode( CursorMode currentMode )
{
	m_cursorState.m_cursorMode = currentMode;
}

//-----------------------------------------------------------------------------------------------
Vec2 InputSystem::GetCursorClientDelta() const
{
	int x = m_cursorState.m_cursorClientDelta.x;
	int y = m_cursorState.m_cursorClientDelta.y;

	return Vec2( static_cast< float > ( x ), static_cast< float > ( y ) );
}

//-----------------------------------------------------------------------------------------------
Vec2 InputSystem::GetCursorClientPosition() const
{
	int x = m_cursorState.m_cursorClientPosition.x;
	int y = m_cursorState.m_cursorClientPosition.y;

	return Vec2( static_cast< float > ( x ), static_cast< float > ( y ) );
}

IntVec2 InputSystem::CursorScreenToClient()
{
	POINT cursorPos;
	HWND hwnd = static_cast< HWND >( g_engine->m_window->m_windowHandle );
	if ( !GetCursorPos( &cursorPos ) )
	{
		return IntVec2::ZERO;
	}

	if ( !ScreenToClient( hwnd, &cursorPos ) )
	{
		return IntVec2::ZERO;
	}

	return IntVec2( cursorPos.x, cursorPos.y );
}

//-----------------------------------------------------------------------------------------------
XboxController const& InputSystem::GetController( int controllerID )
{
	return m_controllers [ controllerID ];
}

//-----------------------------------------------------------------------------------------------
bool InputSystem::IsKeyDown( unsigned char keyCode )
{
	return m_keyStates [ keyCode ].m_isPressed;
}

//-----------------------------------------------------------------------------------------------
void InputSystem::HandleKeyPressed( unsigned char keyCode )
{
	m_keyStates [ keyCode ].m_isPressed = true;
}

//-----------------------------------------------------------------------------------------------
void InputSystem::HandleKeyReleased( unsigned char keyCode )
{
	m_keyStates [ keyCode ].m_isPressed = false;
}

//-----------------------------------------------------------------------------------------------
bool InputSystem::Event_KeyPressed( EventArgs& args )
{
	if ( !g_engine->m_input )
	{
		return false;
	}
	unsigned char keyCode = static_cast< unsigned char >( args.GetValue( "KeyCode", -1 ) );
	g_engine->m_input->HandleKeyPressed( keyCode );

	return true;
}

//-----------------------------------------------------------------------------------------------
bool InputSystem::Event_KeyReleased( EventArgs& args )
{
	if ( !g_engine->m_input )
	{
		return false;
	}
	unsigned char keyCode = static_cast< unsigned char >( args.GetValue( "KeyCode", -1 ) );
	g_engine->m_input->HandleKeyReleased( keyCode );

	return true;
}

//-----------------------------------------------------------------------------------------------
void InputSystem::CorrectCursorVisibility()
{
	if ( m_cursorState.m_cursorMode == CursorMode::POINTER && !IsCursorVisible() )
	{
		while ( ShowCursor( TRUE ) < 0 );
	}

	if ( m_cursorState.m_cursorMode == CursorMode::FPS && IsCursorVisible() )
	{
		while ( ShowCursor( FALSE ) >= 0 );
	}
}

//-----------------------------------------------------------------------------------------------
bool InputSystem::IsCursorVisible()
{
	CURSORINFO cursorInfo = { 0 };
	cursorInfo.cbSize = sizeof( CURSORINFO );

	if ( !GetCursorInfo( &cursorInfo ) )
	{
		return false;
	}

	return cursorInfo.flags & CURSOR_SHOWING;
}