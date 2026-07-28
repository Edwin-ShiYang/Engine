#include "XboxController.hpp"
#include <Windows.h>
#include <Xinput.h> 
#include <Engine/Core/ErrorWarningAssert.hpp>
#include <Engine/Math/MathUtils.hpp>
#pragma comment( lib, "xinput9_1_0" )


const WORD xboxButtonFlags[ static_cast<int>(XboxButtonID::NUM) - 2 ] = {
	XINPUT_GAMEPAD_A,
	XINPUT_GAMEPAD_B,
	XINPUT_GAMEPAD_X,
	XINPUT_GAMEPAD_Y,
	XINPUT_GAMEPAD_DPAD_UP,
	XINPUT_GAMEPAD_DPAD_DOWN,
	XINPUT_GAMEPAD_DPAD_LEFT,
	XINPUT_GAMEPAD_DPAD_RIGHT,
	XINPUT_GAMEPAD_LEFT_SHOULDER,
	XINPUT_GAMEPAD_RIGHT_SHOULDER,
	XINPUT_GAMEPAD_LEFT_THUMB,
	XINPUT_GAMEPAD_RIGHT_THUMB,
	XINPUT_GAMEPAD_START,
	XINPUT_GAMEPAD_BACK
};

XboxController::XboxController( int controllerID )
	: m_controllerID(controllerID)
{
	m_leftStick.SetDeadZoneThresholds(0.3f, 0.95f);
	m_rightStick.SetDeadZoneThresholds(0.3f, 0.95f);
}

XboxController::~XboxController()
{
}

bool XboxController::IsConnected() const
{
	return m_isConnected;
}

int XboxController::GetControllerID() const
{
	return m_controllerID;
}

AnalogJoystick const& XboxController::GetLeftStick() const
{
	return m_leftStick;
}

AnalogJoystick const& XboxController::GetRightStick() const
{
	return m_rightStick;
}

float XboxController::GetLeftTrigger() const
{
	return m_leftTrigger;
}

float XboxController::GetRightTrigger() const
{
	return m_rightTrigger;
}

KeyButtonState const& XboxController::GetButton(XboxButtonID buttonID) const
{
	return m_button[ static_cast<int>(buttonID) ];
}

bool XboxController::IsButtonDown( XboxButtonID buttonID ) const
{
	return GetButton( buttonID ).m_isPressed;
}

bool XboxController::WasButtonJustPressed(XboxButtonID buttonID) const
{
	return !GetButton( buttonID ).m_wasPressedLastFrame && GetButton( buttonID ).m_isPressed;
}

bool XboxController::WasButtonJustReleased(XboxButtonID buttonID) const
{
	return GetButton(buttonID).m_wasPressedLastFrame && !GetButton(buttonID).m_isPressed;
}

void XboxController::UpdateTrigger(float& out_triggerValue, unsigned char rawValue)
{
	out_triggerValue = GetClampedZeroToOne(static_cast<float>(rawValue));

}

void XboxController::RefreshStatus()
{
	XINPUT_STATE xboxControllerState = {};
	DWORD result = XInputGetState(m_controllerID, &xboxControllerState);

	if (result == ERROR_SUCCESS)
	{
		for (int buttonIndex = 0; buttonIndex < static_cast<int>(XboxButtonID::NUM) - 2; ++ buttonIndex)
		{
			UpdateButton(static_cast<XboxButtonID>(buttonIndex), xboxControllerState.Gamepad.wButtons, xboxButtonFlags[ buttonIndex ]);
		}

		UpdateTrigger(m_leftTrigger,  xboxControllerState.Gamepad.bLeftTrigger);
		m_button[ static_cast<int>(XboxButtonID::LEFT_TRIGGER) ].m_isPressed = m_leftTrigger > 0.f;

		UpdateTrigger(m_rightTrigger, xboxControllerState.Gamepad.bRightTrigger);
		m_button[ static_cast<int>(XboxButtonID::RIGHT_TRIGGER) ].m_isPressed = m_rightTrigger > 0.f;

		UpdateJoystick(m_leftStick,  xboxControllerState.Gamepad.sThumbLX, xboxControllerState.Gamepad.sThumbLY);
		UpdateJoystick(m_rightStick, xboxControllerState.Gamepad.sThumbRX, xboxControllerState.Gamepad.sThumbRY);

		m_isConnected = true;
	}
	else
	{
		Reset();
	}
}

void XboxController::UpdateButton(XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag)
{
	m_button[static_cast<int>(buttonID)].m_isPressed = (buttonFlags & buttonFlag) != 0;
}

void XboxController::UpdateLastFrameButtons()
{
	for (int buttonIndex = 0; buttonIndex < static_cast<int>(XboxButtonID::NUM ); ++ buttonIndex) {
		m_button[buttonIndex].m_wasPressedLastFrame = m_button[buttonIndex].m_isPressed;
	}
}

void XboxController::UpdateJoystick(AnalogJoystick& out_joystick, short rawX, short rawY)
{
	const float rawNormalizedX = RangeMapClamped( static_cast<float>( rawX ), -32768.f, 32767.f, -1.f, 1.f );
	const float rawNormalizedY = RangeMapClamped( static_cast<float>( rawY ), -32768.f, 32767.f, -1.f, 1.f );

	out_joystick.UpdatePosition( rawNormalizedX, rawNormalizedY );
}

void XboxController::Reset()
{
	m_leftTrigger = 0.f;
	m_rightTrigger = 0.f;
	m_leftStick = AnalogJoystick();
	m_rightStick = AnalogJoystick();

	for (int i = 0; i < (int)XboxButtonID::NUM; ++i) {
		m_button[i].m_isPressed = false;
		m_button[i].m_wasPressedLastFrame = false;
	}

	m_isConnected = false;
}



