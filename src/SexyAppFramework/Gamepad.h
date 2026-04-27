#pragma once

#if SEXY_USE_CONTROLLER

#include <SDL3/SDL_gamepad.h>

#define MAX_GAMEPADS 4

namespace Sexy
{
	enum GamepadButtons
	{
		BUTTON_SOUTH = SDL_GAMEPAD_BUTTON_SOUTH,
		BUTTON_EAST,
		BUTTON_WEST,
		BUTTON_NORTH,
		BUTTON_LEFT_STICK = SDL_GAMEPAD_BUTTON_LEFT_STICK,
		BUTTON_RIGHT_STICK,
		BUTTON_LEFT_SHOULDER,
		BUTTON_RIGHT_SHOULDER,
		BUTTON_DPAD_UP,
		BUTTON_DPAD_DOWN,
		BUTTON_DPAD_LEFT,
		BUTTON_DPAD_RIGHT,
	};

	class Gamepad
	{
		  public:
			Gamepad();
			~Gamepad();
			bool IsConnected();
			void SetWeight(float theWeight);
			bool IsButtonDown(GamepadButtons theButton);
			float GetAxisPosition(SDL_GamepadAxis theAxis);
			float GetLeftAxisXPosition();
			float GetLeftAxisYPosition();
			float GetRightAxisXPosition();
			float GetRightAxisYPosition();
			void AddRumbleEffect(float theLowFrequency, float theHighFrequency, uint32_t theMsDuration);
			void Reset();
			/* //todo: research and implement
			void GetAxisYPositionRamped(void);
			void GetAxisXPositionRamped(void);
			*/
			uint32_t GetGamepadIndex();
			void SetGamepadIndex(uint32_t theIndex);
			void Status();
			void UpdateStates(float);
			void HandleButtonDown(int, unsigned int);
			void HandleButtonUp(int, unsigned int);
			void HandleGameAxisMove(int, int);
			void SetDeviceID(uint32_t theId);
			uint32_t GetDeviceID();
			void SetListener(SDL_Gamepad *theListener);

		private:
			SDL_Gamepad *mInternalGamepad;
			SDL_JoystickID mDeviceID; 
			float mWeight;
	};

}


#endif