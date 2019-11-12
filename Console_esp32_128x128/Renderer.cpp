#include "Renderer.h"

void Renderer::draw(uint16_t* targetBuffer)
{
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
			targetBuffer[posBuffer(renderPosX, renderPosY)] = *(texture + posBuffer(x, y, SPRITE_SIZE)) + colorShift;
		}
	}
}
