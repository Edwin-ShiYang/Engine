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
}

//----------------------------------------------------------------------------------------------
void DearImGUISystem::ShutDown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}