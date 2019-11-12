#include "Input.h"

void Input::start()
{
	pinMode(BUTTON_UP, INPUT_PULLUP);
	pinMode(BUTTON_DOWN, INPUT_PULLUP);
	pinMode(BUTTON_RIGHT, INPUT_PULLUP);
	pinMode(BUTTON_LEFT, INPUT_PULLUP);
	pinMode(BUTTON_START, INPUT_PULLUP);
	pinMode(BUTTON_SELECT, INPUT_PULLUP);
	pinMode(BUTTON_X, INPUT_PULLUP);
	pinMode(BUTTON_Y, INPUT_PULLUP);
	pinMode(BUTTON_A, INPUT_PULLUP);
	pinMode(BUTTON_B, INPUT_PULLUP);
}

void Input::update()
{
	button_up = !digitalRead(BUTTON_UP);
	button_down = !digitalRead(BUTTON_DOWN);
	button_right = !digitalRead(BUTTON_RIGHT);
	button_left = !digitalRead(BUTTON_LEFT);
	button_select = !digitalRead(BUTTON_SELECT);
	button_start = !digitalRead(BUTTON_START);
	button_X = !digitalRead(BUTTON_X);
	button_A = !digitalRead(BUTTON_A);
	button_Y = !digitalRead(BUTTON_Y);
	button_B = !digitalRead(BUTTON_B);
}
