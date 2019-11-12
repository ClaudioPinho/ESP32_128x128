#include "Object.h"

class Renderer : public Object {

public:

	//Dedicate space for a single texture on this renderer
	int16_t* texture;

	bool* alphaMap;

	int16_t colorShift;

	bool shouldRender = true;

	//Gets the position of a pixel in 2D coordinates inside the buffer frame
	int16_t posBuffer(int16_t x, int16_t y) {
		return x + WIDTH * y;
	}

	//Gets the position of a pixel in 2D coordinated inside a single dimension array with a certain x(Width) size
	int16_t posBuffer(int16_t x, int16_t y, int16_t xSize) {
		return x + xSize * y;
	}

	virtual void draw(uint16_t* targetBuffer);
};