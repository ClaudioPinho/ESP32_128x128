#include "HardwarePinout.h"
#include "Input.h"
#include "Console.h"
#include "GameObject.h"

//Store the console controller
Console console;
//Store the input controller
Input input;


//Graphics definitions
#define ALLOW_SCREEN_CLIPING false
#define USE_ALPHA_MAPS true


#define ROT_RIGHT 0
#define ROT_LEFT 1
#define ROT_UP 2
#define ROT_DOWN 3

//MAIN VARIABLES

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
			if (input.button_up) UpdateMovement(0, -2);
			if (input.button_down) UpdateMovement(0, 2);
			if (input.button_right) UpdateMovement(2, 0);
			if (input.button_left) UpdateMovement(-2, 0);

			if (input.button_A) Serial.println("A");
			if (input.button_B) Serial.println("B");
			if (input.button_X) Serial.println("X");
			if (input.button_Y) Serial.println("Y");
			if (input.button_select) Serial.println("SELECT");

			if (input.button_start) Shoot();
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
	//Start the console main controller
	console.start();
	//Start the console input controller
	input.start();


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
	//update the console controller
	console.update();
	//update the input controller
	input.update();

	console.beginFrame();


	//Draw the apple on the screen
	apple.draw(displayBufferFrame);

	for (uint8_t i = 0; i < playerCount; i++)
	{
		player[i].update();
		player[i].draw(displayBufferFrame);
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
		obstacles[i].draw(displayBufferFrame);
	}

	shootProj.update();
	shootProj.draw(displayBufferFrame);

	//Does the player intersect the apple?
	if (player[0].intersects(apple)) 
	{
		//If the player eats the apple then select a new position for the apple
		apple.posX = random(10, WIDTH - 10);
		apple.posY = random(10, HEIGHT - 10);
		EasyBuzzer.singleBeep(500, 1);
	}

	console.closeFrame();
	//Delay this frame
	delay(16);
}

void EndCollisionSound() {
	collisionSoundPlaying = false;
}
