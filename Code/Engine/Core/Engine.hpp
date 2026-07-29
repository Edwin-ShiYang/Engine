#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Network/NetworkSystem.hpp"
#include "Engine/Renderer/DearImGUISystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/AbilitySystem/AbilitySystem.hpp"

//-----------------------------------------------------------------------------------------------
class Engine;
class Window;
class Renderer;
class InputSystem;
class AudioSystem;
class EventSystem;
class NetworkSystem;

//-----------------------------------------------------------------------------------------------
extern Engine* g_engine;

//-----------------------------------------------------------------------------------------------
struct EngineConfig
{
    AudioConfig           m_audioConfig;
    InputConfig           m_inputConfig;
    RenderConfig          m_renderConfig;
    WindowConfig          m_windowConfig;
    DevConsoleConfig      m_devConsoleConfig;
    EventSystemConfig     m_eventSystemConfig;
    DearImGUISystemConfig m_dearImGUISystemConfig;
    NetworkConfig         m_networkConfig;
    AbilitySystemConfig   m_abilitySystemConfig;
};

//-----------------------------------------------------------------------------------------------
class Engine
{
public:
    Engine( EngineConfig const& config );
    ~Engine();

    void Startup();
    void BeginFrame();
    void EndFrame();
    void Shutdown();

public:
    Window*          m_window        = nullptr;
    Renderer*        m_render        = nullptr;
    InputSystem*     m_input         = nullptr;
    DevConsole*      m_devConsole    = nullptr;
    AudioSystem*     m_audio         = nullptr;
    EventSystem*     m_eventSystem   = nullptr;
    DearImGUISystem* m_imgui         = nullptr;
    NetworkSystem*   m_network       = nullptr;
    AbilitySystem*   m_abilitySystem = nullptr;

    EngineConfig     m_config;
};