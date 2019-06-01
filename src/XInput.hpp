#include <Windows.h>
#include <Xinput.h>

class Gamepad
{
	private:
	int cId;
	XINPUT_STATE state;
	float deadzoneX;
	float deadzoneY;
	std::chrono::steady_clock::time_point lastPressed[10];
	 
	public:
	Gamepad() : cId(), deadzoneX(0.1f), deadzoneY(0.1f)  {
		for( int i=0; i<10; ++i )
			lastPressed[i] = std::chrono::steady_clock::now();
	}
	Gamepad(int cId, float dzX, float dzY) : cId(cId), deadzoneX(dzX), deadzoneY(dzY) {
		for( int i=0; i<10; ++i )
			lastPressed[i] = std::chrono::steady_clock::now();
	}
	 
	float leftStickX;
	float leftStickY;
	float rightStickX;
	float rightStickY;
	float leftTrigger;
	float rightTrigger;
	 
	int  GetPort();
	XINPUT_GAMEPAD *GetState();
	bool CheckConnection();
	bool Refresh();
	bool IsPressed(WORD);
	bool IsPressed(WORD,double cooldown);
};