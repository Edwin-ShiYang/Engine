#include "Engine/Renderer/DearImGUISystem.hpp"
#include "Engine/Core/Engine.hpp"
#include <d3d11.h>

//----------------------------------------------------------------------------------------------
DearImGUISystem::DearImGUISystem( DearImGUISystemConfig const& config )
    : m_config( config )
{
}

//----------------------------------------------------------------------------------------------
DearImGUISystem::~DearImGUISystem()
{
}

//----------------------------------------------------------------------------------------------
void DearImGUISystem::BeginFrame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

//----------------------------------------------------------------------------------------------
void DearImGUISystem::Render()
{
}

//----------------------------------------------------------------------------------------------
void DearImGUISystem::EndFrame()
{
    ImGui::Render();
    ImDrawData* data = ImGui::GetDrawData();
    ImGui_ImplDX11_RenderDrawData( data );
}

//----------------------------------------------------------------------------------------------
void DearImGUISystem::Startup()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //io.IniFilename = nullptr;

    io.Fonts->AddFontFromFileTTF( m_config.m_fontPath.c_str(), m_config.m_fontSize );
    // ImGui::GetStyle().ScaleAllSizes( 1.15f );

    ImGui_ImplWin32_Init( g_engine->m_window->m_windowHandle );
    ImGui_ImplDX11_Init( g_engine->m_render->m_device, g_engine->m_render->m_deviceContext );
    SetImGuiCatppuccinMacchiatoTheme();
}

//----------------------------------------------------------------------------------------------
void DearImGUISystem::ShutDown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

//----------------------------------------------------------------------------------------------
void DearImGUISystem::SetImGuiCatppuccinMacchiatoTheme()
{
    ImGuiStyle& style  = ImGui::GetStyle();
    ImVec4*     colors = style.Colors;

    colors[ ImGuiCol_Text ]         = ImVec4( 0.792f, 0.827f, 0.961f, 1.000f );
    colors[ ImGuiCol_TextDisabled ] = ImVec4( 0.486f, 0.529f, 0.675f, 1.000f );
    colors[ ImGuiCol_WindowBg ]     = ImVec4( 0.141f, 0.153f, 0.227f, 0.940f );
    colors[ ImGuiCol_ChildBg ]      = ImVec4( 0.118f, 0.125f, 0.188f, 0.900f );
    colors[ ImGuiCol_PopupBg ]      = ImVec4( 0.118f, 0.125f, 0.188f, 0.980f );
    colors[ ImGuiCol_Border ]       = ImVec4( 0.286f, 0.302f, 0.392f, 1.000f );
    colors[ ImGuiCol_BorderShadow ] = ImVec4( 0.000f, 0.000f, 0.000f, 0.000f );

    colors[ ImGuiCol_FrameBg ]        = ImVec4( 0.212f, 0.227f, 0.310f, 1.000f );
    colors[ ImGuiCol_FrameBgHovered ] = ImVec4( 0.286f, 0.302f, 0.392f, 1.000f );
    colors[ ImGuiCol_FrameBgActive ]  = ImVec4( 0.361f, 0.376f, 0.459f, 1.000f );

    colors[ ImGuiCol_TitleBg ]          = ImVec4( 0.118f, 0.125f, 0.188f, 1.000f );
    colors[ ImGuiCol_TitleBgActive ]    = ImVec4( 0.141f, 0.153f, 0.227f, 1.000f );
    colors[ ImGuiCol_TitleBgCollapsed ] = ImVec4( 0.094f, 0.098f, 0.149f, 1.000f );

    colors[ ImGuiCol_MenuBarBg ] = ImVec4( 0.118f, 0.125f, 0.188f, 1.000f );

    colors[ ImGuiCol_ScrollbarBg ]          = ImVec4( 0.094f, 0.098f, 0.149f, 1.000f );
    colors[ ImGuiCol_ScrollbarGrab ]        = ImVec4( 0.286f, 0.302f, 0.392f, 1.000f );
    colors[ ImGuiCol_ScrollbarGrabHovered ] = ImVec4( 0.361f, 0.376f, 0.459f, 1.000f );
    colors[ ImGuiCol_ScrollbarGrabActive ]  = ImVec4( 0.541f, 0.678f, 0.957f, 1.000f );

    colors[ ImGuiCol_CheckMark ]        = ImVec4( 0.651f, 0.855f, 0.584f, 1.000f );
    colors[ ImGuiCol_SliderGrab ]       = ImVec4( 0.541f, 0.678f, 0.957f, 1.000f );
    colors[ ImGuiCol_SliderGrabActive ] = ImVec4( 0.776f, 0.627f, 0.965f, 1.000f );

    colors[ ImGuiCol_Button ]        = ImVec4( 0.212f, 0.227f, 0.310f, 1.000f );
    colors[ ImGuiCol_ButtonHovered ] = ImVec4( 0.286f, 0.302f, 0.392f, 1.000f );
    colors[ ImGuiCol_ButtonActive ]  = ImVec4( 0.541f, 0.678f, 0.957f, 1.000f );

    colors[ ImGuiCol_Header ]        = ImVec4( 0.212f, 0.227f, 0.310f, 1.000f );
    colors[ ImGuiCol_HeaderHovered ] = ImVec4( 0.286f, 0.302f, 0.392f, 1.000f );
    colors[ ImGuiCol_HeaderActive ]  = ImVec4( 0.541f, 0.678f, 0.957f, 0.800f );

    colors[ ImGuiCol_Separator ]        = ImVec4( 0.286f, 0.302f, 0.392f, 1.000f );
    colors[ ImGuiCol_SeparatorHovered ] = ImVec4( 0.541f, 0.678f, 0.957f, 1.000f );
    colors[ ImGuiCol_SeparatorActive ]  = ImVec4( 0.776f, 0.627f, 0.965f, 1.000f );

    colors[ ImGuiCol_ResizeGrip ]        = ImVec4( 0.541f, 0.678f, 0.957f, 0.250f );
    colors[ ImGuiCol_ResizeGripHovered ] = ImVec4( 0.541f, 0.678f, 0.957f, 0.670f );
    colors[ ImGuiCol_ResizeGripActive ]  = ImVec4( 0.776f, 0.627f, 0.965f, 0.950f );

    colors[ ImGuiCol_Tab ]                = ImVec4( 0.118f, 0.125f, 0.188f, 1.000f );
    colors[ ImGuiCol_TabHovered ]         = ImVec4( 0.541f, 0.678f, 0.957f, 0.800f );
    colors[ ImGuiCol_TabActive ]          = ImVec4( 0.212f, 0.227f, 0.310f, 1.000f );
    colors[ ImGuiCol_TabUnfocused ]       = ImVec4( 0.118f, 0.125f, 0.188f, 1.000f );
    colors[ ImGuiCol_TabUnfocusedActive ] = ImVec4( 0.212f, 0.227f, 0.310f, 1.000f );

    colors[ ImGuiCol_TableHeaderBg ]     = ImVec4( 0.212f, 0.227f, 0.310f, 1.000f );
    colors[ ImGuiCol_TableBorderStrong ] = ImVec4( 0.286f, 0.302f, 0.392f, 1.000f );
    colors[ ImGuiCol_TableBorderLight ]  = ImVec4( 0.212f, 0.227f, 0.310f, 1.000f );
    colors[ ImGuiCol_TableRowBg ]        = ImVec4( 0.000f, 0.000f, 0.000f, 0.000f );
    colors[ ImGuiCol_TableRowBgAlt ]     = ImVec4( 0.212f, 0.227f, 0.310f, 0.350f );

    colors[ ImGuiCol_TextSelectedBg ] = ImVec4( 0.541f, 0.678f, 0.957f, 0.350f );
    colors[ ImGuiCol_DragDropTarget ] = ImVec4( 0.933f, 0.831f, 0.624f, 0.900f );
    colors[ ImGuiCol_NavHighlight ]   = ImVec4( 0.541f, 0.678f, 0.957f, 1.000f );

    style.WindowRounding    = 8.f;
    style.ChildRounding     = 6.f;
    style.FrameRounding     = 4.f;
    style.PopupRounding     = 6.f;
    style.ScrollbarRounding = 6.f;
    style.GrabRounding      = 4.f;
    style.TabRounding       = 4.f;

    style.WindowBorderSize = 1.f;
    style.FrameBorderSize  = 1.f;
    style.PopupBorderSize  = 1.f;

    style.WindowPadding    = ImVec2( 12.f, 10.f );
    style.FramePadding     = ImVec2( 8.f, 4.f );
    style.ItemSpacing      = ImVec2( 8.f, 6.f );
    style.ItemInnerSpacing = ImVec2( 6.f, 4.f );
}