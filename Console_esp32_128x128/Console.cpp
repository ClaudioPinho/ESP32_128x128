#include "Console.h"

//Store SPI class for the display
SPIClass displaySPI(VSPI);
//Store the display class
Adafruit_ST7735 display = Adafruit_ST7735(&displaySPI, DISPLAY_CS, DISPLAY_DC, DISPLAY_RST);

//Store the SPI class for the SD card reader
SPIClass sdSPI(HSPI);

//Create a empty buffer frame for the display
uint16_t displayBufferFrame[WIDTH * HEIGHT];

void Console::start()
{
	//Start the display SPI class
	displaySPI.begin(18, 19, 23, 5);
	display.initR(INITR_144GREENTAB);
	//Start the buzzer
	EasyBuzzer.setPin(SPEAKER);
	//Start the SD card reader SPI class
	sdSPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

	//Start the serial comm port
	Serial.begin(COMM_PORT_BAUND_RATE);

	//Check if the SD was recognized and mounted
	if (!SD.begin(15, sdSPI)) {
		Serial.println("Card Mount Failed!");
		return;
	}
	else {
		Serial.println("Mounting Was Successfull!");
	}
}

void Console::update()
{
	//Update the buzzer controller
	EasyBuzzer.update();
}

void Console::beginFrame()
{
	/*
	//Clear bufferframe
	for (uint16_t i = 0; i < WIDTH * HEIGHT; i++)
	{
		displayBufferFrame[i] = 0x0000;
	}
	*/
	//Draw a background
	for (uint16_t i = 0; i < WIDTH * HEIGHT; i++)
	{
		displayBufferFrame[i] = grass_texture[i];
	}
}

void Console::closeFrame()
{
	//Draw texture to console display
	display.drawRGBBitmap(BUFFER_OFFSET_X, BUFFER_OFFSET_Y, displayBufferFrame, WIDTH, HEIGHT);
}
