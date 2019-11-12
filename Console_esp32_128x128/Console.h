#include "HardwarePinout.h"
#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <EasyBuzzer.h>
#include "Textures.h"
#include <SPI.h>
#include <SD.h>

#define WIDTH 128
#define HEIGHT 128

#define SPRITE_SIZE 16

#define COMM_PORT_BAUND_RATE 115200

#define BUFFER_OFFSET_X 0
#define BUFFER_OFFSET_Y 0

class Console {
public:

	void start();
	
	void update();

	void beginFrame();

	void closeFrame();
};
