#pragma once
#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Input/KeyButtonState.hpp"


//-----------------------------------------------------------------------------------------------
enum class XboxButtonID
{
	A,
	B,
	X,
	Y,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	LEFT_SHOULDER,
	RIGHT_SHOULDER,
	LEFT_THUMB,
	RIGHT_THUMB,
	START,
	BACK,
	LEFT_TRIGGER,
	RIGHT_TRIGGER,
	NUM
};


//-----------------------------------------------------------------------------------------------
class XboxController 
{
	friend class InputSystem;

public:
	XboxController( int controllerID );
	~XboxController();
	bool					IsConnected() const;
	int						GetControllerID() const;
	AnalogJoystick const&	GetLeftStick() const;
	AnalogJoystick const&   GetRightStick() const;
	float                   GetLeftTrigger() const;
	float                   GetRightTrigger() const;
	KeyButtonState const&   GetButton( XboxButtonID buttonID ) const;
	bool                    IsButtonDown( XboxButtonID buttonID ) const;
	bool                    WasButtonJustPressed( XboxButtonID buttonID ) const;
	bool                    WasButtonJustReleased(XboxButtonID buttonID) const;

private:
	void RefreshStatus();
	void Reset();
	void UpdateJoystick( AnalogJoystick& out_joystick, short rawX, short rawY );
	void UpdateTrigger( float& out_triggerValue, unsigned char rawValue );
	void UpdateButton( XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag );
	void UpdateLastFrameButtons();

private:
	int						m_controllerID	= -1;
	bool					m_isConnected   = false;
	float					m_leftTrigger   = 0.f;
	float					m_rightTrigger  = 0.f;
	KeyButtonState	        m_button[ (int) XboxButtonID::NUM ];
	AnalogJoystick          m_leftStick;
	AnalogJoystick          m_rightStick;
};