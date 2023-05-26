#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <vector>
#include <ranges> // requires C++20
#include "GameWorld.h"
#include <algorithm>
#include <optional>

const int ACTOR_HEIGHT = 4;
const int ACTOR_WIDTH_LIMIT = VIEW_WIDTH - 4;
const int ACTOR_HEIGHT_LIMIT = VIEW_HEIGHT - 4;

class StudentWorld; // Gordian knotting the circular dependency

class Object : public GraphObject {
public:
	Object(StudentWorld& world, int imageID, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = 0)
	: GraphObject(imageID, startX, startY, dir, size, depth), w(world) {
		setVisible(true);
	}

	float getDistanceTo(Object& other);
	bool contains(unsigned int x, unsigned int y);
	bool intersects(unsigned int x, unsigned int y, unsigned int x_size = ACTOR_HEIGHT, unsigned int y_size = ACTOR_HEIGHT);

	bool isDead() { return dead; }

	virtual void doSomething() = 0;

protected:
	bool dead = false;

	StudentWorld& w; // passed in during init()
};

class Actor : public Object {
public:
	Actor(StudentWorld& world, int imageID, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = 0)
		: Object(world, imageID, startX, startY, dir, size, depth){}
};

const int ICEMAN_MAX_HEALTH = 10;
const int ICEMAN_DEFAULT_WATER = 5;
const int ICEMAN_DEFAULT_SONAR = 1;

const int CAP_TWO_DIGITS = 99;
// further class modualarization to come
class IceMan : public Actor {
public:
	IceMan(StudentWorld& world, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = 0)
		: Actor(world, IID_PLAYER, startX, startY, dir, size, depth)  {
	}
	void doSomething();

	int getHealth() { return health; }
	int getWater() { return water; }
	int getSonar() { return sonar; }
	int getNuggs() { return nuggs; }

	void addWater(unsigned int add) { 
		water += add; 
		if (water > CAP_TWO_DIGITS)
			water = CAP_TWO_DIGITS;
	}

	void offsetHealth(int offset);

private:
	int health = ICEMAN_MAX_HEALTH;
	int max_health = ICEMAN_MAX_HEALTH;
	int water = ICEMAN_DEFAULT_WATER;
	int sonar = ICEMAN_DEFAULT_SONAR;
	int nuggs = 0;
};

const int ICE_DEPTH = 3;
const int GOODIE_DEPTH = 2;

class Prop : public Object {
public:
	Prop(StudentWorld& world, int imageID, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = GOODIE_DEPTH)
		: Object(world, imageID, startX, startY, dir, size, depth) {}
};

class Water : public Prop {
public:
	Water(StudentWorld& world, int level, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = GOODIE_DEPTH)
		: Prop(world, IID_WATER_POOL, startX, startY, dir, size, depth) {
		lifespan = std::max(100, 300 - (10 * level));
	}

	void doSomething();

private:
	int lifespan = 0;
};

const double ICE_SIZE = 0.25;

class IceBlock : public Prop {
public:
	IceBlock(StudentWorld& world, int startX, int startY, Direction dir = right, double size = ICE_SIZE, unsigned int depth = ICE_DEPTH)
		: Prop(world, IID_ICE, startX, startY, dir, size, depth) {
	}

	void doSomething() { }
};

const int TUNNEL_START_X = 30;
const int TUNNEL_END_X = 33;
const int TUNNEL_END_Y = 4;

const unsigned int DEFAULT_ICE_DESTROY_RANGE = 4;

class Ice {
public:
	Ice(StudentWorld& world);
	// smart pointers mean that we don't have to delete anything ourselves
	
	std::shared_ptr<IceBlock>& getBlock(int x, int y) {
		// includes bounds checking
		return iceObjects.at(x).at(y);
	}
	
	bool destroyIce(unsigned int x, unsigned int y, unsigned int x_size, unsigned int y_size);

	std::optional<std::pair<unsigned int, unsigned int>> getOpenSquare(unsigned int i);
	std::optional<std::pair<unsigned int, unsigned int>> getIceSquare(unsigned int i);

private:	
	// a less disgusting way to write this would be appreciated
	// using namespace std would just be a stopgap
	// making the elements unique_ptrs without incurring the compiler's wrath would also be cool
	std::vector<std::vector<std::shared_ptr<IceBlock>>> iceObjects = std::vector<std::vector<std::shared_ptr<IceBlock>>>(VIEW_WIDTH, std::vector<std::shared_ptr<IceBlock>>(VIEW_HEIGHT, nullptr));

	std::vector<std::pair<unsigned int, unsigned int>> openSquares;
	std::vector<std::pair<unsigned int, unsigned int>> iceSquares;

	void populateAvailableSquares();
};


#endif // ACTOR_H_
