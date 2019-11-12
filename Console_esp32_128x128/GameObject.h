#include "Renderer.h"

class GameObject : public Renderer {

public:

	bool alive = true;

	virtual void start();

	virtual void update();

	bool intersects(GameObject other);

};
