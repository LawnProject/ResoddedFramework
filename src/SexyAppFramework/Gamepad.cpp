#if SEXY_USE_CONTROLLER
#include "Gamepad.h"
#include "Common.h"
#include <SDL3/SDL_log.h>

using namespace Sexy;

Gamepad::Gamepad()
{
	mInternalGamepad = nullptr;
	mDeviceID = 0;
	SetWeight(0.244f);
}

Gamepad::~Gamepad()
{
	if (mInternalGamepad != nullptr)
		SDL_CloseGamepad(mInternalGamepad);
}

bool Gamepad::IsConnected()
{
	return mInternalGamepad != nullptr && SDL_GamepadConnected(mInternalGamepad);
}

void Gamepad::SetWeight(float theWeight)
{
	mWeight = theWeight;
}

bool Gamepad::IsButtonDown(GamepadButtons theButton)
{
	return SDL_GetGamepadButton(mInternalGamepad, (SDL_GamepadButton)theButton); //We arranged GamepadButtons to mimic the SDL_GamepadButton layout - Electr0Gunner
}

float Sexy::Gamepad::GetAxisPosition(SDL_GamepadAxis theAxis)
{
	float aRawValue = SDL_GetGamepadAxis(mInternalGamepad, theAxis) / (float)UINT16_MAX;
	float aFiltered = fabsf(aRawValue) < mWeight ? 0.0f : aRawValue;
	return aFiltered;
}

float Gamepad::GetLeftAxisXPosition()
{
	return GetAxisPosition(SDL_GAMEPAD_AXIS_LEFTX);
}

float Gamepad::GetLeftAxisYPosition()
{
	return GetAxisPosition(SDL_GAMEPAD_AXIS_LEFTY);
}

float Gamepad::GetRightAxisXPosition()
{
	return GetAxisPosition(SDL_GAMEPAD_AXIS_RIGHTX);
}

float Gamepad::GetRightAxisYPosition()
{
	return GetAxisPosition(SDL_GAMEPAD_AXIS_RIGHTY);
}

void Gamepad::AddRumbleEffect(float theLowFrequency, float theHighFrequency, uint32_t theMsDuration)
{
	SDL_RumbleGamepad(mInternalGamepad, theLowFrequency * UINT16_MAX, theHighFrequency * UINT16_MAX, theMsDuration);
}

void Gamepad::Reset()
{
	SDL_RumbleGamepad(mInternalGamepad, 0, 0, 0);
}

uint32_t Gamepad::GetGamepadIndex()
{
	if (mInternalGamepad == nullptr)
		return 0;
	return SDL_GetGamepadID(mInternalGamepad);
}

void Gamepad::SetGamepadIndex(uint32_t theIndex)
{

}

void Gamepad::UpdateStates(float)
{
}

void Gamepad::HandleButtonDown(int, unsigned int)
{
}

void Gamepad::HandleButtonUp(int, unsigned int)
{
}

void Gamepad::HandleGameAxisMove(int, int)
{
}

void Gamepad::SetDeviceID(uint32_t theId)
{
	mDeviceID = theId;
	mInternalGamepad = SDL_OpenGamepad(mDeviceID);
}

uint32_t Gamepad::GetDeviceID()
{
	return mDeviceID;
}

void Gamepad::SetListener(SDL_Gamepad *theListener)
{
	mInternalGamepad = theListener;
	mDeviceID = SDL_GetGamepadID(mInternalGamepad);
}


#endif