#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <vector>
#include "GameWorld.h"

const int ACTOR_WIDTH_LIMIT = VIEW_WIDTH - 4;
const int ACTOR_HEIGHT_LIMIT = VIEW_HEIGHT - 4;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class IceMan : public GraphObject {
public:
	IceMan(GameWorld& world, int imageID, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = 0)
		: GraphObject(imageID, startX, startY, dir, size, depth), w(world)  {
		setVisible(true);
	}
	void doSomething();
	~IceMan() { }

	GameWorld& w; // passed in during init()
};

class IceBlock : public GraphObject {
public:
	IceBlock(int imageID, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = 0)
		: GraphObject(imageID, startX, startY, dir, size, depth) {
		setVisible(true);
	}
	~IceBlock() {
		delete this;
	}
};

class Ice {
public:
	Ice() {
		for (int i{}; i < VIEW_WIDTH; i++) {
			for (int j{}; j < VIEW_HEIGHT; j++)
			iceObjects.push_back(new IceBlock(IID_ICE, i, j, GraphObject::right, .25, 3));
		}
	}
	~Ice() {
		iceObjects.clear();
	}
	
private:
	std::vector<IceBlock*> iceObjects;

};


#endif // ACTOR_H_
