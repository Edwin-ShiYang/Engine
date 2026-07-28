#include "Engine/Core/Engine.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Window/Window.hpp"
#include "../Model/ModelImporter.hpp"

//-----------------------------------------------------------------------------------------------
Engine* g_engine = nullptr;

//-----------------------------------------------------------------------------------------------
Engine::Engine( EngineConfig const& config )
    : m_config( config )
{
    /*
		Set the g_engine pointer to point to me, even during
		my construction, so that engine subsystem can find
		me and each other during the initialization process.
	*/
    g_engine = this;

    if ( config.m_eventSystemConfig.m_isEnabled ) m_eventSystem = new EventSystem( config.m_eventSystemConfig );
    if ( config.m_inputConfig.m_isEnabled ) m_input = new InputSystem( config.m_inputConfig );
    if ( config.m_dearImGUISystemConfig.m_isEnabled ) m_imgui = new DearImGUISystem( config.m_dearImGUISystemConfig );
    if ( config.m_windowConfig.m_isEnabled ) m_window = new Window( config.m_windowConfig );
    if ( config.m_renderConfig.m_isEnabled ) m_render = new Renderer( config.m_renderConfig );
    if ( config.m_devConsoleConfig.m_isEnabled ) m_devConsole = new DevConsole( config.m_devConsoleConfig );
    if ( config.m_audioConfig.m_isEnabled ) m_audio = new AudioSystem( config.m_audioConfig );
    if ( config.m_networkConfig.m_isEnabled ) m_network = new NetworkSystem( config.m_networkConfig );
}

//-----------------------------------------------------------------------------------------------
void Engine::Startup()
{
    if ( m_eventSystem )
        m_eventSystem->Startup();
    if ( m_window )
        m_window->Startup();
    if ( m_render )
        m_render->Startup();
    if ( m_imgui )
        m_imgui->Startup();
    if ( m_devConsole )
        m_devConsole->Startup();
    if ( m_input )
        m_input->Startup();
    if ( m_audio )
        m_audio->Startup();
    if ( m_network )
        m_network->Startup();
}

//-----------------------------------------------------------------------------------------------
Engine::~Engine()
{
    if ( m_audio ) m_audio->Shutdown();
    if ( m_devConsole ) m_devConsole->Shutdown();
    if ( m_input ) m_input->Shutdown();
    if ( m_imgui ) m_imgui->ShutDown();
    if ( m_render ) m_render->Shutdown();
    if ( m_window ) m_window->Shutdown();
    if ( m_eventSystem ) m_eventSystem->Shutdown();
    if ( m_network ) m_network->Shutdown();

    delete m_audio;
    m_audio = nullptr;

    delete m_render;
    m_render = nullptr;

    delete m_imgui;
    m_imgui = nullptr;

    delete m_window;
    m_window = nullptr;

    delete m_input;
    m_input = nullptr;

    delete m_devConsole;
    m_devConsole = nullptr;

    delete m_eventSystem;
    m_eventSystem = nullptr;

    delete m_network;
    m_network = nullptr;
}

//-----------------------------------------------------------------------------------------------
void Engine::BeginFrame()
{
    if ( m_eventSystem ) m_eventSystem->BeginFrame();
    if ( m_devConsole ) m_devConsole->BeginFrame();
    if ( m_window ) m_window->BeginFrame();
    if ( m_imgui ) m_imgui->BeginFrame();
    if ( m_render ) m_render->BeginFrame();
    if ( m_input ) m_input->BeginFrame();
    if ( m_audio ) m_audio->BeginFrame();
    if ( m_network ) m_network->BeginFrame();
}

//-----------------------------------------------------------------------------------------------
void Engine::EndFrame()
{
    if ( m_eventSystem ) m_eventSystem->EndFrame();
    if ( m_window ) m_window->EndFrame();
    if ( m_imgui ) m_imgui->EndFrame();
    if ( m_render ) m_render->EndFrame();
    if ( m_input ) m_input->EndFrame();
    if ( m_audio ) m_audio->EndFrame();
    if ( m_devConsole ) m_devConsole->EndFrame();
    if ( m_network ) m_network->EndFrame();
}

//-----------------------------------------------------------------------------------------------
void Engine::Shutdown()
{
}