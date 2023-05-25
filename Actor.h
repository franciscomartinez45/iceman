#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <vector>
#include <ranges> // requires C++20
#include "GameWorld.h"

const int ACTOR_WIDTH_LIMIT = VIEW_WIDTH - 4;
const int ACTOR_HEIGHT_LIMIT = VIEW_HEIGHT - 4;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class StudentWorld; // Gordian knotting the circular dependency

class Actor : public GraphObject {
public:
	Actor(int imageID, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = 0)
	: GraphObject(imageID, startX, startY, dir, size, depth) {
		setVisible(true);
	}

	virtual void doSomething() { }
};

const int ICEMAN_MAX_HEALTH = 100;

// further class modualarization to come
class IceMan : public Actor {
public:
	IceMan(StudentWorld& world, int imageID, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = 0)
		: Actor(imageID, startX, startY, dir, size, depth), w(world)  {
		setVisible(true);
	}
	void doSomething();

	int getHealth() { return health; }

	void offsetHealth(int offset);

private:
	int health = ICEMAN_MAX_HEALTH;
	int max_health = ICEMAN_MAX_HEALTH;
	StudentWorld& w; // passed in during init()
};

class IceBlock : public Actor {
public:
	IceBlock(int imageID, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = 0)
		: Actor(imageID, startX, startY, dir, size, depth) {
		setVisible(true);
	}
};

const int TUNNEL_START_X = 30;
const int TUNNEL_END_X = 33;
const int TUNNEL_END_Y = 4;
const int ACTOR_HEIGHT = 4;

class Ice {
public:
	Ice() {
		// from <ranges>
		// think python ranges, but C++ified (i.e. painfully verbose)
		for (auto i : std::ranges::iota_view(0, VIEW_WIDTH)) {
			for (auto j : std::ranges::iota_view(0, VIEW_HEIGHT - ACTOR_HEIGHT)) {
				if ((i < TUNNEL_START_X) || (i > TUNNEL_END_X) || (j <= TUNNEL_END_Y)) {
					iceObjects[i][j] = std::make_unique<IceBlock>(IID_ICE, i, j, GraphObject::right, .25, 3);
				}
			}
		}
	}
	// smart pointers mean that we don't have to delete anything ourselves
	
	std::shared_ptr<IceBlock>& getBlock(int x, int y) {
		// includes bounds checking
		return iceObjects.at(x).at(y);
	}
	
private:
	// a less disgusting way to write this would be appreciated
	// using namespace std would just be a stopgap
	// making the elements unique_ptrs without incurring the compiler's wrath would also be cool
	std::vector<std::vector<std::shared_ptr<IceBlock>>> iceObjects = std::vector<std::vector<std::shared_ptr<IceBlock>>>(VIEW_WIDTH, std::vector<std::shared_ptr<IceBlock>>(VIEW_HEIGHT, nullptr));

};


#endif // ACTOR_H_


