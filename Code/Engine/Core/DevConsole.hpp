#pragma once


//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
struct AABB2;
class BitmapFont;
class Timer;

//-----------------------------------------------------------------------------------------------
struct DevConsoleConfig
{
	bool        m_isEnabled         = true;
	Camera*     m_camera            = nullptr;
	std::string m_fontName          = "SquirrelFixedFont";
	float       m_fontAspect        = 0.7f;
	int         m_linesOnScreen     = 40;
	int         m_maxCommandHistory = 128;
	bool        m_startOpen         = false;
};


//-----------------------------------------------------------------------------------------------
struct DevConsoleLine
{
	Rgba8 m_color;
	std::string m_text;
};


//-----------------------------------------------------------------------------------------------
enum DevConsoleMode
{
	HIDDEN,
	OPEN_FULL
};


//-----------------------------------------------------------------------------------------------
class DevConsole
{
public:
	DevConsole( DevConsoleConfig const& config );
	~DevConsole();

	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void Execute( std::string const& consoleCommandText );
	void AddLine( Rgba8 const& color, std::string const& text );
	void Render ( AABB2 const& bounds ) const;

	DevConsoleMode GetMode() const;
	void SetMode( DevConsoleMode mode );
	void ToggleOpen( DevConsoleMode mode );
	bool IsOpen();
	void PrintRegisteredCommands();

	static const Rgba8 ERROR_COLOR;
	static const Rgba8 WARNING_COLOR;
	static const Rgba8 INFO_MAJOR_COLOR;
	static const Rgba8 INFO_MINOR_COLOR;
	static const Rgba8 INPUT_TEXT_COLOR;
	static const Rgba8 INPUT_INSTRUCTION_POINT_COLOR;
	static const Rgba8 BACKGROUND_COLOR;

	static bool Event_KeyPressed( EventArgs& args );
	static bool Event_CharInput( EventArgs& args );
	static bool Command_Clear( EventArgs& args );
	static bool Command_Help( EventArgs& args );
	
	std::vector<std::string> GetCommands( std::string const& consoleCommandText );
	void FireEventsWithArgs( std::string const& command );

protected:
	//void Render_OpenFull( AABB2 const& bounds, BitmapFont& font, float fontAspect = 1.f ) const;

private:
	bool HandleKeyPressed( unsigned char keyCode );
	bool HandleCharInput( unsigned char keyCode );


protected:
	DevConsoleConfig            m_config;
	bool                        m_isOpen = false;
	std::vector<DevConsoleLine> m_lines;
	std::string                 m_inputText;
	int                         m_insertionPointPosition = 0;
	bool                        m_insertionPointVisible  = true;
	Timer*                      m_insertionPointBlinkTimer;
	std::vector<std::string>    m_commandHistory;
	int                         m_historyIndex = -1;
	DevConsoleMode              m_mode = DevConsoleMode::HIDDEN;
	int                         m_frameNumber = 0;
	float                       m_fontWidth = 0.f;
};