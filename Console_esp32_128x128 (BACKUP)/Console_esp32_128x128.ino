//#include <Adafruit_SSD1331.h>
#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include "Textures.h"
#include "HardwarePinout.h"
#include <EasyBuzzer.h>
#include <SD.h>

#define WIDTH 128
#define HEIGHT 128

#define SPRITE_SIZE 16

//Controls status
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
SPIClass displaySPI(VSPI);
Adafruit_ST7735 display = Adafruit_ST7735(&displaySPI, DISPLAY_CS, DISPLAY_DC, DISPLAY_RST);

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

	int16_t colorShift = 0x0000;

	bool shouldRender = true;

	virtual void draw() {
		if (!shouldRender) return;
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
				bufferTexture[posBuffer(renderPosX, renderPosY)] = *(texture + posBuffer(x, y, SPRITE_SIZE)) + colorShift;
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

	bool alive = true;

	virtual void start() {

	}

	virtual void update() {
		shouldRender = alive;
		if (!alive) return;
	}

	bool intersects(GameObject other) {
		//Check for intersection with other gameobject
		if (posX >= other.posX - SPRITE_SIZE / 2 && 
			posX < other.posX + SPRITE_SIZE / 2 &&
			posY >= other.posY - SPRITE_SIZE / 2 &&
			posY < other.posY + SPRITE_SIZE / 2)
		{
			return true;
		}
		else 
		{
			return false;
		}
	}

};

class Player : public GameObject {

public:
	
	bool isAI = false;
	int16_t movementSpeed = 2;
	int16_t xSpeed, ySpeed;

	int8_t deltaXMovement, deltaYMovement;
	
	uint8_t rotationMode = ROT_UP;

	uint8_t movementChance;


	void start() {
		GameObject::start();
		posX = random(0, WIDTH);
		posY = random(0, HEIGHT);

		xSpeed = movementSpeed;
		ySpeed = movementSpeed;

		movementChance = random(50, 95);
		//copyTexture(player_texture, texture);
		//copyAlphaMap(player_alpha_map, alphaMap);
	}

	void update() 
	{
		GameObject::update();
		if(isAI)
		{
			//Should move?
			if (random(0, 101) >= movementChance)
			{
				int8_t randX = random(-5, 6);
				int8_t randY = random(-5, 6);
				UpdateMovement(randX, randY);
			}
		}
		else
		{
			if (button_up) UpdateMovement(0, -2);
			if (button_down) UpdateMovement(0, 2);
			if (button_right) UpdateMovement(2, 0);
			if (button_left) UpdateMovement(-2, 0);

			if (button_A) Serial.println("A");
			if (button_B) Serial.println("B");
			if (button_X) Serial.println("X");
			if (button_Y) Serial.println("Y");
			if (button_select) Serial.println("SELECT");

			if (button_start) Shoot();
		}
		//Update the player rotation
		UpdateRotation();
	}

private:

	void Shoot() 
	{
		EasyBuzzer.singleBeep(500, 2, EndCollisionSound);
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

	void UpdateMovement(int8_t movX, int8_t movY)
	{
		if (movY < 0)
		{
			rotationMode = ROT_UP;
			if (posY - movY > 0)
			{
				posY += movY;
			}
			else
			{

			}
		}
		if (movY > 0)
		{
			rotationMode = ROT_DOWN;
			if (posY + movY < HEIGHT)
			{
				posY += movY;
			}
			else
			{

			}
		}

		if (movX > 0)
		{
			rotationMode = ROT_RIGHT;
			if (posX + movX < WIDTH)
			{
				posX += movX;
			}
			else
			{

			}
		}
		if (movX < 0)
		{
			rotationMode = ROT_LEFT;
			if (posX - movX > 0)
			{
				posX += movX;
			}
			else
			{

			}
		}
		deltaXMovement = movX;
		deltaYMovement = movY;
	}
};

class Obstacle : public GameObject {

public:

	void start() {
		GameObject::start();
		//Define a random position for the obstacles
		posX = random(0, WIDTH);
		posY = random(0, HEIGHT);
		//Define the obstacle texture
		texture = &texture_rock[0];
		alphaMap = &texture_rock_alpha[0];
	}

	void update() {
		GameObject::update();
	}
};

class Projectil : public GameObject {

public:

	uint8_t projectileMovX, projectileMovY;

	void start() {
		GameObject::start();
		texture = &texture_projectile[0];
		alphaMap = &texture_projectile_alpha[0];
	}

	void update() {
		GameObject::update();
		posX += projectileMovX;
		posY += projectileMovY;
	}
};

const uint8_t playerCount = 5;
const uint8_t obstacleCount = 254;

bool collisionSoundPlaying = false;

Player player[playerCount];
Obstacle obstacles[obstacleCount];
GameObject apple;
Projectil shootProj;

void setup() {
	Serial.begin(115200);

	//SETUP SLAVE SELECT PINS
	//pinMode(SD_CS, OUTPUT);
	//pinMode(DISPLAY_CS, OUTPUT);

	EasyBuzzer.setPin(SPEAKER);

	SD.begin();

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

	//SETUP THE VSPI
	//BEGIN THE SPI WITH THE VSPI PINS
	displaySPI.begin(18, 19, 23, 5);

	display.initR(INITR_144GREENTAB);

	for (uint8_t i = 0; i < playerCount; i++)
	{
		player[i].start();
		player[i].isAI = true;
		player[i].colorShift = 0x000A;
	}
	player[0].isAI = false;
	player[0].colorShift = 0x0000;

	for (uint8_t i = 0; i < obstacleCount; i++)
	{
		obstacles[i].start();
	}

	shootProj.start();
	shootProj.alive = false;

	//Set the apple texture
	apple.texture = &texture_apple[0];
	apple.alphaMap = &texture_apple_alpha[0];
	apple.start();
	apple.posX = random(10, WIDTH - 10);
	apple.posY = random(10, HEIGHT - 10);
}

// the loop function runs over and over again until power down or reset
void loop() {
	//Read and update the controls at the beggining of the frame
	UpdateControls();
	//Update the buzzer
	EasyBuzzer.update();
  
	//Clear bufferframe !!!! LETS FILL THE BACKGROUND
	for (uint16_t i = 0; i < WIDTH * HEIGHT; i++)
	{
		//bufferTexture[i] = 0x0000;
		bufferTexture[i] = grass_texture[i];
	}

	//Draw the apple on the screen
	apple.draw();

	for (uint8_t i = 0; i < playerCount; i++)
	{
		player[i].update();
		player[i].draw();
	}

	for (uint8_t i = 0; i < obstacleCount; i++)
	{
		//Lets check if the rock collides with the players
		for (uint8_t x = 0; x < playerCount; x++)
		{
			if (obstacles[i].intersects(player[x])) 
			{
				obstacles[i].posX += player[x].deltaXMovement * 2;
				obstacles[i].posY += player[x].deltaYMovement * 2;
				if (!collisionSoundPlaying) {
					EasyBuzzer.singleBeep(10, 2, EndCollisionSound);
					collisionSoundPlaying = true;
				}
			}
		}
		obstacles[i].draw();
	}

	shootProj.update();
	shootProj.draw();

	//Does the player intersect the apple?
	if (player[0].intersects(apple)) 
	{
		//If the player eats the apple then select a new position for the apple
		apple.posX = random(10, WIDTH - 10);
		apple.posY = random(10, HEIGHT - 10);
		EasyBuzzer.singleBeep(500, 1);
	}

	//Draw texture to display
	display.drawRGBBitmap(0, 0, bufferTexture, WIDTH, HEIGHT);
	//Delay this frame
	delay(16);
}

void EndCollisionSound() {
	collisionSoundPlaying = false;
}

void UpdateControls(){
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
