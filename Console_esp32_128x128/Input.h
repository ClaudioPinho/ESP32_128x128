#include "HardwarePinout.h"

class Input {

public :

	//Control status
	bool button_up;
	bool button_down;
	bool button_right;
	bool button_left;
	bool button_select;
	bool button_start;
	bool button_X;
	bool button_Y;
	bool button_A;
	bool button_B;

	void start();

	void update();

};