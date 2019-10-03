//#include <Adafruit_SSD1331.h>
#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include "Textures.h"

#define WIDTH 128
#define HEIGHT 128

#define SPRITE_SIZE 16

#define sclk 18 // 18
#define mosi 19 // 23
#define cs   5
#define rst  17
#define dc   16

//SPEAKER
#define SPEAKER 12

//Controls

#define BUTTON_UP 11 
#define BUTTON_DOWN 10
#define BUTTON_RIGHT 9
#define BUTTON_LEFT 13
//#define BUTTON_SELECT 0
//#define BUTTON_START 2
//#define BUTTON_X 8
//#define BUTTON_Y 15
//#define BUTTON_A 7
//#define BUTTON_B 6

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

//end of controls

//Graphics definitions

#define ALLOW_SCREEN_CLIPING false
#define USE_ALPHA_MAPS true


#define ROT_RIGHT 0
#define ROT_LEFT 1
#define ROT_UP 2
#define ROT_DOWN 3

//MAIN VARIABLES

//Adafruit_ST7735 display = Adafruit_ST7735(cs, dc, mosi, sclk, rst);
SPIClass spiRFID(VSPI);
Adafruit_ST7735 display = Adafruit_ST7735(&spiRFID, cs, dc, rst);

//Create a empty buffer frame for the display
uint16_t bufferTexture[WIDTH * HEIGHT];

//Gets the position of a pixel in 2D coordinates inside the buffer frame
int16_t posBuffer(int16_t x, int16_t y) {
	return x + WIDTH * y;
}

//Gets the position of a pixel in 2D coordinated inside a single dimension array with a certain x(Width) size
int16_t posBuffer(int16_t x, int16_t y, int16_t xSize) {
	return x + xSize * y;
}

void copyTexture(int16_t texOrigin[], int16_t texDestiny[]) {
	for (int16_t index = 0; index < textureSpriteResolution; index++)
	{
		texDestiny[index] = texOrigin[index];
	}
}

void copyAlphaMap(bool alphaTexOrigin[], bool alphaTexDestiny[]) {
	for (int16_t index = 0; index < textureSpriteResolution; index++)
	{
		alphaTexOrigin[index] = alphaTexDestiny[index];
	}
}

class Object {

public:

	int16_t posX, posY;

};

class Renderer : public Object {

public:

	//Dedicate space for a single texture on this renderer
	int16_t* texture;

	bool* alphaMap;

	virtual void draw() {
		//Determine the starting position of this texture to draw considering it's central pivot
		int16_t bufferStartX = posX - (SPRITE_SIZE / 2);
		int16_t bufferStartY = posY - (SPRITE_SIZE / 2);

		for (int16_t x = 0; x < SPRITE_SIZE; x++)
		{
			for (int16_t y = 0; y < SPRITE_SIZE; y++)
			{
				int16_t renderPosX = bufferStartX + x;
				int16_t renderPosY = bufferStartY + y;
				//If these pixels are out of bounds then skip them
				if ((renderPosX < 0 || renderPosX > WIDTH - 1 || renderPosY < 0 || renderPosY > HEIGHT - 1) && !ALLOW_SCREEN_CLIPING) continue;
				//Draws on the buffer texture the texture given to this object
				if (USE_ALPHA_MAPS && alphaMap[posBuffer(x, y, SPRITE_SIZE)]) continue;
				//bufferTexture[posBuffer(renderPosX, renderPosY)] = texture[posBuffer(x, y, SPRITE_SIZE)];
				bufferTexture[posBuffer(renderPosX, renderPosY)] = *(texture + posBuffer(x, y, SPRITE_SIZE));
			}
		}
	}
};

/*
class GameManager {

private:

	LinkedList<GameObject *> gameObjects = LinkedList<GameObject *>();

public:

	void start() {
		for (uint8_t i = 0; i < gameObjects.size(); i++)
		{
			(*gameObjects.get(i)).start();
		}
	}

	void update() {
		for (uint8_t i = 0; i < gameObjects.size(); i++)
		{
			(*gameObjects.get(i)).update();
			(*gameObjects.get(i)).draw();
		}
	}

	void addGameObject(GameObject gO) {
		gameObjects.add(&gO);
	}

} gameManager;
*/

class GameObject : public Renderer {
	
public:

	virtual void start() {

	}


	virtual void update() {

	}

};

class Player : public GameObject {

public:
	
	int16_t movementSpeed = 2;
	int16_t xSpeed, ySpeed;
	uint8_t rotationMode = ROT_UP;

	void start() {
		
		posX = random(0, WIDTH);
		posY = random(0, HEIGHT);

		xSpeed = movementSpeed;
		ySpeed = movementSpeed;

		//copyTexture(player_texture, texture);
		//copyAlphaMap(player_alpha_map, alphaMap);
	}

	void UpdateRotation() 
	{
		if (rotationMode == ROT_UP)
		{
			texture = &bug_up[0];
			alphaMap = &bug_up_alpha[0];
		}
		if (rotationMode == ROT_RIGHT)
		{
			texture = &bug_right[0];
			alphaMap = &bug_right_alpha[0];
		}
		if (rotationMode == ROT_LEFT)
		{
			texture = &bug_left[0];
			alphaMap = &bug_left_alpha[0];
		}
		if (rotationMode == ROT_DOWN)
		{
			texture = &bug_down[0];
			alphaMap = &bug_down_alpha[0];
		}

	}

	void update() {
		//Update the player rotation
		UpdateRotation();
		if (button_up)
		{
			rotationMode = ROT_UP;
			if (posY - ySpeed > 0)
			{
				posY -= ySpeed;
			}
			else
			{
				
			}
		}
		if (button_down)
		{
			rotationMode = ROT_DOWN;
			if (posY + ySpeed < HEIGHT)
			{
				posY += ySpeed;
			}
			else
			{

			}
		}
		if (button_right)
		{
			rotationMode = ROT_RIGHT;
			if (posX + xSpeed < WIDTH)
			{
				posX += xSpeed;
			}
			else 
			{

			}
		}
		if (button_left)
		{
			rotationMode = ROT_LEFT;
			if (posX - xSpeed > 0)
			{
				posX -= xSpeed;
			}
			else
			{

			}
		}
	}
};

class Obstacle : public GameObject {

public:

	void Start() {
		//Define a random position for the obstacles
		posX = random(0, WIDTH);
		posY = random(0, HEIGHT);
		//Define the obstacle texture
		texture = &texture_rock[0];
		alphaMap = &texture_rock_alpha[0];
	}

	void Update() {

	}
};

//Obstacle obstacle;
Player player;
//Obstacle obstacle;

void setup() {
	pinMode(BUTTON_UP, INPUT_PULLUP);
	pinMode(BUTTON_DOWN, INPUT_PULLUP);
	pinMode(BUTTON_RIGHT, INPUT_PULLUP);
	pinMode(BUTTON_LEFT, INPUT_PULLUP);
	//pinMode(BUTTON_START, INPUT_PULLUP);
	//pinMode(BUTTON_SELECT, INPUT_PULLUP);
	//pinMode(BUTTON_X, INPUT_PULLUP);
	//pinMode(BUTTON_Y, INPUT_PULLUP);
	//pinMode(BUTTON_A, INPUT_PULLUP);
	//pinMode(BUTTON_B, INPUT_PULLUP);

	display.initR(INITR_144GREENTAB);

	spiRFID.begin(18, 19, 23, 5);

	player.start();
}

// the loop function runs over and over again until power down or reset
void loop() {
	//Read and update the controls at the beggining of the frame
	UpdateControls();
  
	//Clear bufferframe !!!! LETS FILL THE BACKGROUND
	for (uint16_t i = 0; i < WIDTH * HEIGHT; i++)
	{
		//bufferTexture[i] = 0x0000;
		bufferTexture[i] = grass_texture[i];
	}

	//Game code...
	//display.fillScreen(random(0, 65535));

	player.update();
	player.draw();

	//obstacle.draw();

	//Draw texture to display
	display.drawRGBBitmap(0, 0, bufferTexture, WIDTH, HEIGHT);
	//Delay this frame
	delay(16);
}

void UpdateControls(){
  button_up = !digitalRead(BUTTON_UP);
  button_down = !digitalRead(BUTTON_DOWN);
  button_right = !digitalRead(BUTTON_RIGHT);
  button_left = !digitalRead(BUTTON_LEFT);
  /*
  button_select = !digitalRead(BUTTON_SELECT);
  button_start = !digitalRead(BUTTON_START);
  button_X = !digitalRead(BUTTON_X);
  button_Y = !digitalRead(BUTTON_Y);
  button_A = !digitalRead(BUTTON_A);
  button_B = !digitalRead(BUTTON_B);
  */
}
