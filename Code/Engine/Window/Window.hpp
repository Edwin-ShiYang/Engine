#pragma once

//----------------------------------------------------------------------------------------------
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include <string>

//----------------------------------------------------------------------------------------------
class InputSystem;

//----------------------------------------------------------------------------------------------
struct WindowConfig
{
    bool        m_isEnabled    = true;
    float       m_clientAspect = ( 16.f / 9.f );
    std::string m_windowTitle  = "Unnamed SD Application";
    bool        m_isFullScreen = false;
};

//----------------------------------------------------------------------------------------------
class Window
{
public:
    Window( WindowConfig const& config );
    ~Window();

    void         Startup();
    void         Shutdown();
    void         BeginFrame();
    void         EndFrame();
    AABB2        GetClientBounds();

    WindowConfig m_config;
    void*        m_displayDeviceContext = 0;  // Actually an HDC  in Windows
    void*        m_windowHandle         = 0;  // Actually an HWND in Windows
    IntVec2      m_clientDimensions     = IntVec2::ZERO;

    Vec2         GetNormalizedMouseUV() const;
    void*        GetHwnd() const;
    IntVec2      GetClientDimensions() const;
    IntVec2      GetClientCenter() const;
    bool         HasFocus() const;

private:
    void CreateOSWindow();
    void RunMessagePump();
};