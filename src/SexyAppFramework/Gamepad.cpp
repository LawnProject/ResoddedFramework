#if SEXY_USE_CONTROLLER
#include "Gamepad.h"
#include <SDL3/SDL_log.h>

using namespace Sexy;

Gamepad::Gamepad()
{
	mInternalGamepad = nullptr;
	mDeviceID = 0;
	SetWeight(0.244f);
	memset(mPrevButtons, 0, sizeof(mPrevButtons));
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
	if (mInternalGamepad == nullptr) return false;
	return SDL_GetGamepadButton(mInternalGamepad, (SDL_GamepadButton)theButton);
}

bool Gamepad::IsButtonJustPressed(GamepadButtons theButton)
{
	if (mInternalGamepad == nullptr) return false;
	bool aCurState = SDL_GetGamepadButton(mInternalGamepad, (SDL_GamepadButton)theButton);
	return aCurState && !mPrevButtons[(int)theButton];
}

bool Gamepad::IsButtonJustReleased(GamepadButtons theButton)
{
	if (mInternalGamepad == nullptr) return false;
	bool aCurState = SDL_GetGamepadButton(mInternalGamepad, (SDL_GamepadButton)theButton);
	return !aCurState && mPrevButtons[(int)theButton];
}

float Sexy::Gamepad::GetAxisPosition(SDL_GamepadAxis theAxis)
{
	if (mInternalGamepad == nullptr) return 0.0f;
	float aRawValue = SDL_GetGamepadAxis(mInternalGamepad, theAxis) / 32767.0f;
	float aFiltered = fabsf(aRawValue) < mWeight ? 0.0f : aRawValue;
	return aFiltered;
}

float Gamepad::GetAxisXPositionRamped()
{
	float v1 = GetLeftAxisXPosition();
	float v2 = GetLeftAxisYPosition();
	float v3 = sqrtf(v1 * v1 + v2 * v2);
	if (v3 < mWeight) return 0.0f;

	float aScale = (v3 - mWeight) / (1.0f - mWeight);
	return (v1 / v3) * aScale;
}

float Gamepad::GetAxisYPositionRamped()
{
	float v1 = GetLeftAxisXPosition();
	float v2 = GetLeftAxisYPosition();
	float v3 = sqrtf(v1 * v1 + v2 * v2);
	if (v3 < mWeight) return 0.0f;

	float aScale = (v3 - mWeight) / (1.0f - mWeight);
	return (v2 / v3) * aScale;
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
	if (mInternalGamepad == nullptr) return;
	SDL_RumbleGamepad(mInternalGamepad, (uint16_t)(theLowFrequency * UINT16_MAX), (uint16_t)(theHighFrequency * UINT16_MAX), theMsDuration);
}

void Gamepad::Reset()
{
	if (mInternalGamepad == nullptr) return;
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
	if (mInternalGamepad == nullptr) return;
	for (int i = 0; i < SDL_GAMEPAD_BUTTON_COUNT; i++)
	{
		mPrevButtons[i] = SDL_GetGamepadButton(mInternalGamepad, (SDL_GamepadButton)i);
	}
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
