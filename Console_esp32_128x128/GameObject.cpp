#include "GameObject.h"

void GameObject::start()
{

}

void GameObject::update()
{
	shouldRender = alive;
	if (!alive) return;
}

bool GameObject::intersects(GameObject other)
{
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
