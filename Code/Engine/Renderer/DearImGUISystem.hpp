#pragma once

//----------------------------------------------------------------------------------------------
#include "ThirdParty/imGUI/imgui.h"
#include "ThirdParty/imGUI/imgui_impl_dx11.h"
#include "ThirdParty/imGUI/imgui_impl_win32.h"
#include <string>

//-----------------------------------------------------------------------------------------------
struct DearImGUISystemConfig
{
    bool        m_isEnabled = true;
    std::string m_fontPath  = "Data/Fonts/Consolas-Regular.ttf";
    float       m_fontSize  = 18.f;
};

//----------------------------------------------------------------------------------------------
class DearImGUISystem
{
public:
    DearImGUISystem( DearImGUISystemConfig const& config );
    ~DearImGUISystem();

    void BeginFrame();
    void Render();
    void EndFrame();
    void Startup();
    void ShutDown();

protected:
    DearImGUISystemConfig m_config;
};