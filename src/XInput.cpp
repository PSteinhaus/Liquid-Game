#include "main.hpp"
#include <iostream>
#include <math.h>

using std::cout;
using std::endl;
 
int Gamepad::GetPort()
{
	return cId + 1;
}
 
XINPUT_GAMEPAD *Gamepad::GetState()
{
	return &state.Gamepad;
}
 
bool Gamepad::CheckConnection()
{
	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	 
	return (XInputGetState(cId, &state) == ERROR_SUCCESS);
}
 
// Returns false if the controller has been disconnected
bool Gamepad::Refresh()
{
 
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	if (XInputGetState(cId, &state) != ERROR_SUCCESS)
	{
		return false;
	}
	else
	{
		float normLX = fmaxf(-1, (float) state.Gamepad.sThumbLX / 32767);
		float normLY = fmaxf(-1, (float) state.Gamepad.sThumbLY / 32767);
		 
		leftStickX = (abs(normLX) < deadzoneX ? 0 : (abs(normLX) - deadzoneX) * (normLX / abs(normLX)));
		leftStickY = (abs(normLY) < deadzoneY ? 0 : (abs(normLY) - deadzoneY) * (normLY / abs(normLY)));
		 
		if (deadzoneX > 0) leftStickX *= 1 / (1 - deadzoneX);
		if (deadzoneY > 0) leftStickY *= 1 / (1 - deadzoneY);
		 
		float normRX = fmaxf(-1, (float) state.Gamepad.sThumbRX / 32767);
		float normRY = fmaxf(-1, (float) state.Gamepad.sThumbRY / 32767);
		 
		rightStickX = (abs(normRX) < deadzoneX ? 0 : (abs(normRX) - deadzoneX) * (normRX / abs(normRX)));
		rightStickY = (abs(normRY) < deadzoneY ? 0 : (abs(normRY) - deadzoneY) * (normRY / abs(normRY)));
		 
		if (deadzoneX > 0) rightStickX *= 1 / (1 - deadzoneX);
		if (deadzoneY > 0) rightStickY *= 1 / (1 - deadzoneY);
		 
		leftTrigger = (float) state.Gamepad.bLeftTrigger / 255;
		rightTrigger = (float) state.Gamepad.bRightTrigger / 255;
		 
		return true;
	}
	return false;
}
 
bool Gamepad::IsPressed(WORD button)
{
	return (state.Gamepad.wButtons & button) != 0;
}

bool Gamepad::IsPressed(WORD button, double cooldown)
{
	int index;
	switch(button) {
		case XINPUT_GAMEPAD_DPAD_UP: index = 0;
			break;
		case XINPUT_GAMEPAD_DPAD_DOWN: index = 1;
			break;
		case XINPUT_GAMEPAD_DPAD_LEFT: index = 2;
			break;
		case XINPUT_GAMEPAD_DPAD_RIGHT: index = 3;
			break;
		case XINPUT_GAMEPAD_LEFT_SHOULDER: index = 4;
			break;
		case XINPUT_GAMEPAD_RIGHT_SHOULDER: index = 5;
			break;
		case XINPUT_GAMEPAD_A: index = 6;
			break;
		case XINPUT_GAMEPAD_B: index = 7;
			break;
		case XINPUT_GAMEPAD_X: index = 8;
			break;
		case XINPUT_GAMEPAD_Y: index = 9;
			break;
		default : return false;
	}

	using namespace std::chrono;
	if( IsPressed(button) ) {
		steady_clock::time_point now = steady_clock::now();
		// calculate the time span between now and the last time this buttons was acknowledged as "pressed"
		steady_clock::duration time_span = now - lastPressed[index];
		double nseconds = double(time_span.count()) * steady_clock::period::num / steady_clock::period::den;
		if( nseconds >= cooldown ) {	// if the time span reaches or surpasses the desired cooldown acknowledge the button press
			lastPressed[index] = now;
			return true;
		}
	}
	return false;
}
