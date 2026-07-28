#include "Engine/Window/Window.hpp"

#define WIN32_LEAN_AND_MEAN
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Vec2.hpp"
#include <Windows.h>

#include "ThirdParty/imGUI/imgui.h"
#include "ThirdParty/imGUI/imgui_impl_win32.h"

//----------------------------------------------------------------------------------------------
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

//----------------------------------------------------------------------------------------------
Window::Window( WindowConfig const& config )
    : m_config( config )
{
}

//----------------------------------------------------------------------------------------------
Window::~Window()
{
}

//----------------------------------------------------------------------------------------------
void Window::Startup()
{
    CreateOSWindow();
}

//----------------------------------------------------------------------------------------------
void Window::Shutdown()
{
}

//----------------------------------------------------------------------------------------------
void Window::BeginFrame()
{
    RunMessagePump();
}

//----------------------------------------------------------------------------------------------
void Window::EndFrame()
{
}

//----------------------------------------------------------------------------------------------
Vec2 Window::GetNormalizedMouseUV() const
{
    HWND  windowHandle = static_cast< HWND >( m_windowHandle );
    POINT cursorCoords;
    RECT  clientRect;
    ::GetCursorPos( &cursorCoords );
    ::ScreenToClient( windowHandle, &cursorCoords );
    ::GetClientRect( windowHandle, &clientRect );
    float cursorX = static_cast< float >( cursorCoords.x ) / static_cast< float >( clientRect.right );
    float cursorY = static_cast< float >( cursorCoords.y ) / static_cast< float >( clientRect.bottom );
    return Vec2( cursorX, 1.f - cursorY );
}

//----------------------------------------------------------------------------------------------
void* Window::GetHwnd() const
{
    return m_windowHandle;
}

//----------------------------------------------------------------------------------------------
IntVec2 Window::GetClientDimensions() const
{
    return m_clientDimensions;
}

//-----------------------------------------------------------------------------------------------
IntVec2 Window::GetClientCenter() const
{
    return IntVec2( m_clientDimensions.x / 2, m_clientDimensions.y / 2 );
}

//-----------------------------------------------------------------------------------------------
bool Window::HasFocus() const
{
    return GetActiveWindow() == m_windowHandle;
}

//-----------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowsMessageHandlingProcedure( HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam )
{
    if ( g_engine->m_imgui && ImGui_ImplWin32_WndProcHandler( windowHandle, wmMessageCode, wParam, lParam ) )
    {
        return true;
    }

    switch ( wmMessageCode )
    {
        case WM_CLOSE:
            {
                FireEvent( "Quit" );
                return 0;
            }

        case WM_KEYDOWN:
            {
                unsigned char asKey = (unsigned char)wParam;
                EventArgs     args;
                args.SetValue( "KeyCode", Stringf( "%d", asKey ) );
                FireEvent( "KeyPressed", args );
                break;
            }

        case WM_KEYUP:
            {
                unsigned char asKey = (unsigned char)wParam;
                EventArgs     args;
                args.SetValue( "KeyCode", Stringf( "%d", asKey ) );
                FireEvent( "KeyReleased", args );
                break;
            }

        case WM_CHAR:
            {
                unsigned char asKey = (unsigned char)wParam;
                EventArgs     args;
                args.SetValue( "KeyCode", Stringf( "%d", asKey ) );
                FireEvent( "CharInput", args );
                break;
            }

        case WM_LBUTTONDOWN:
            {
                if ( g_engine->m_input )
                {
                    g_engine->m_input->HandleKeyPressed( KEYCODE_LEFT_MOUSE );
                }
                break;
            }

        case WM_LBUTTONUP:
            {
                if ( g_engine->m_input )
                {
                    g_engine->m_input->HandleKeyReleased( KEYCODE_LEFT_MOUSE );
                }
                break;
            }

        case WM_RBUTTONDOWN:
            {
                if ( g_engine->m_input )
                {
                    g_engine->m_input->HandleKeyPressed( KEYCODE_RIGHT_MOUSE );
                }
                break;
            }

        case WM_RBUTTONUP:
            {
                if ( g_engine->m_input )
                {
                    g_engine->m_input->HandleKeyReleased( KEYCODE_RIGHT_MOUSE );
                }
                break;
            }
    }

    return DefWindowProc( windowHandle, wmMessageCode, wParam, lParam );
}

//-----------------------------------------------------------------------------------------------
void Window::CreateOSWindow()
{
    HINSTANCE applicationInstanceHandle = ::GetModuleHandle( NULL );

    ::SetProcessDpiAwarenessContext( DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 );

    /*
		cbSize:        The size of this structure
		style:         CS_OWNDC ( device context )
		lpfnWndProc:   is a function pointer to the Window Procedure, It tells Windows which function will receive and handle messages for a window
		lpszClassName: is a pointer to a null-terminated string that names a window class
	*/
    WNDCLASSEX windowClassDescription;
    memset( &windowClassDescription, 0, sizeof( windowClassDescription ) );
    windowClassDescription.cbSize        = sizeof( windowClassDescription );
    windowClassDescription.style         = CS_OWNDC;
    windowClassDescription.lpfnWndProc   = static_cast< WNDPROC >( WindowsMessageHandlingProcedure );
    windowClassDescription.hInstance     = applicationInstanceHandle;
    windowClassDescription.hIcon         = NULL;
    windowClassDescription.hCursor       = NULL;
    windowClassDescription.lpszClassName = TEXT( "Simple Window Class" );
    RegisterClassEx( &windowClassDescription );  // register window class

    DWORD const windowStyleFlags   = WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_OVERLAPPED;
    DWORD const windowStyleExFlags = WS_EX_APPWINDOW;

    RECT        desktopRect;
    HWND        desktopWindowHandle = GetDesktopWindow();
    GetClientRect( desktopWindowHandle, &desktopRect );
    float           desktopWidth  = (float)( desktopRect.right - desktopRect.left );
    float           desktopHeight = (float)( desktopRect.bottom - desktopRect.top );
    float           desktopAspect = desktopWidth / desktopHeight;

    constexpr float maxClientFractionOfDesktop = 0.90f;
    float           clientWidth                = desktopWidth * maxClientFractionOfDesktop;
    float           clientHeight               = desktopHeight * maxClientFractionOfDesktop;

    if ( m_config.m_clientAspect > desktopAspect )
    {
        clientHeight = clientWidth / m_config.m_clientAspect;
    }
    else
    {
        clientWidth = clientHeight * m_config.m_clientAspect;
    }

    m_clientDimensions = IntVec2( static_cast< int >( clientWidth ), static_cast< int >( clientHeight ) );

    float clientMarginX = 0.5f * ( desktopWidth - clientWidth );
    float clientMarginY = 0.5f * ( desktopHeight - clientHeight );
    RECT  clientRect;
    clientRect.left   = (int)clientMarginX;
    clientRect.right  = clientRect.left + (int)clientWidth;
    clientRect.top    = (int)clientMarginY;
    clientRect.bottom = clientRect.top + (int)clientHeight;

    RECT windowRect = clientRect;
    AdjustWindowRectEx( &windowRect, windowStyleFlags, FALSE, windowStyleExFlags );

    WCHAR windowTitle[ 1024 ];
    MultiByteToWideChar( GetACP(), 0, m_config.m_windowTitle.c_str(), -1, windowTitle, sizeof( windowTitle ) / sizeof( windowTitle[ 0 ] ) );

    // creates a window based on a registered window class
    m_windowHandle = CreateWindowEx(
        windowStyleExFlags,
        windowClassDescription.lpszClassName,
        windowTitle,
        windowStyleFlags,
        windowRect.left,
        windowRect.top,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        NULL,
        NULL,
        (HINSTANCE)applicationInstanceHandle,
        NULL );

    HWND hwnd = static_cast< HWND >( m_windowHandle );
    ShowWindow( hwnd, m_config.m_isFullScreen ? SW_MAXIMIZE : SW_SHOW );

    RECT actualClientRect = {};
    GetClientRect( hwnd, &actualClientRect );
    m_clientDimensions = IntVec2( actualClientRect.right - actualClientRect.left, actualClientRect.bottom - actualClientRect.top );

    SetForegroundWindow( hwnd );
    SetFocus( hwnd );

    m_displayDeviceContext = GetDC( hwnd );

    HCURSOR cursor = LoadCursor( NULL, IDC_ARROW );
    SetCursor( cursor );
}

//-----------------------------------------------------------------------------------------------
void Window::RunMessagePump()
{
    MSG queuedMessage;
    for ( ;; )
    {
        // PeekMessage: Non-blocking – checks message queue and returns immediately
        BOOL const wasMessagePresent = PeekMessage( &queuedMessage, NULL, 0, 0, PM_REMOVE );
        if ( !wasMessagePresent )
        {
            break;
        }

        TranslateMessage( &queuedMessage );
        DispatchMessage( &queuedMessage );
    }
}

//-----------------------------------------------------------------------------------------------
AABB2 Window::GetClientBounds()
{
    IntVec2 dimensions = g_engine->m_window->GetClientDimensions();
    return AABB2( Vec2( 0.f, 0.f ), Vec2( static_cast< float >( dimensions.x ), static_cast< float >( dimensions.y ) ) );
}