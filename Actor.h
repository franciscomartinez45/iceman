#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <vector>
#include <ranges> // requires C++20
#include "GameWorld.h"
#include <algorithm>
#include <optional>
#include <array>
#include <variant>

const int ACTOR_HEIGHT = 4;
const int ACTOR_WIDTH_LIMIT = VIEW_WIDTH - 4;
const int ACTOR_HEIGHT_LIMIT = VIEW_HEIGHT - 4;

class StudentWorld; // Gordian knotting the circular dependency

class Object : public GraphObject {
public:
	Object(StudentWorld& world, bool actor, int imageID, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = 0)
	: GraphObject(imageID, startX, startY, dir, size, depth), w(world), actorStatus(actor) {
		setVisible(true);
	}

	float getDistanceTo(Object& other);
	float getDistanceTo(std::pair<unsigned int, unsigned int> p);
	bool contains(unsigned int x, unsigned int y);
	bool intersects(unsigned int x, unsigned int y, unsigned int x_size = ACTOR_HEIGHT, unsigned int y_size = ACTOR_HEIGHT);

	bool isDead() { return dead; }
	bool isActor() { return actorStatus; }
	virtual bool isBoulder() { return false; }
	void wasKilledByBoulder() { killedByBoulder = true; }

	virtual void doSomething() = 0;
	virtual void beAnnoyed(int annoy_value) = 0;

protected:
	double getDistanceToPlayer();
	std::optional<double> getDistanceToActor(std::unique_ptr<Object>& object);

	bool dead = false;

	const bool actorStatus;

	bool killedByBoulder = false;

	StudentWorld& w; // passed in during init()
};

const int INSTAKILL_DAMAGE = 100;

class Actor : public Object {
public:
	Actor(StudentWorld& world, int m_health, int imageID, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = 0)
		: Object(world, true, imageID, startX, startY, dir, size, depth), max_health(m_health){}

	virtual void beAnnoyed(int annoy_value) =0;
	int getHealth() { return health; }
	virtual void beBribed() { }

protected:
	bool willCollide(std::pair<int, int> new_pos);

	int max_health;
	int health = max_health;
};

const int ICEMAN_MAX_HEALTH = 10;
const int ICEMAN_DEFAULT_WATER = 5;
const int ICEMAN_DEFAULT_SONAR = 1;

const int CAP_TWO_DIGITS = 99;
// further class modualarization to come
class IceMan : public Actor {
public:
	IceMan(StudentWorld& world, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = 0)
		: Actor(world, ICEMAN_MAX_HEALTH, IID_PLAYER, startX, startY, dir, size, depth)  {
	}
	void doSomething();
	void beAnnoyed(int annoy_value);

	int getWater() { return water; }
	int getSonar() { return sonar; }
	int getNuggs() { return nuggs; }
	int getOil() {return numBarrels;}


	void addWater(unsigned int add) { 
		water += add; 
		if (water > CAP_TWO_DIGITS)
			water = CAP_TWO_DIGITS;
	}

	void addNuggs() {
		nuggs++;
		if (nuggs > CAP_TWO_DIGITS)
			nuggs = CAP_TWO_DIGITS;
	}
	void foundOil() {
		numBarrels--;
	}
	void addOil(int number) {
		numBarrels += number;
	}
	void addSonar() {
		sonar++;
	}
	void useSonar() {
		sonar--;
	}
	void useWater() {
		water--;
	}
private:
	// TESTING FUNCTION - NOT INTENDED FOR REAL GAMEPLAY
	void getReturnPath();	

	int water = ICEMAN_DEFAULT_WATER;
	int sonar = 999;//ICEMAN_DEFAULT_SONAR;
	int nuggs = 0;
	int numBarrels = 0;
};

const int PROTESTER_MAX_HEALTH = 5;
class Protester : public Actor {
public:
	Protester(StudentWorld& world, int startX, int startY, Direction dir = left, double size = 1.0, unsigned int depth = 0);

	void doSomething();
	void beAnnoyed(int annoy_value); // Francisco
	virtual void beBribed();         // Aaron

protected:
	void moveTowardsOilField(); // Francisco
	bool attemptShout();        // Francisco
	bool attemptMoveToIceman(); // Aaron
	void pickNewDirection();    // Aaron

	bool isResting();            // Aaron
	bool straightLineToIceman(); // Francisco
	bool isFacingIceman();       // Aaron
	void updateRestTicks();      // Francisco

	int waitTicks;
	bool leavingOilField;

	int ticksSinceLastPerpendicularTurn;

};

const int ICE_DEPTH = 3;
const int GOODIE_DEPTH = 2;
const int BOULDER_DEPTH = 1;

const double ITEM_PICKUP_DISTANCE = 3.0;

class Prop : public Object {
public:
	Prop(StudentWorld& world, bool affectsPlayer, bool affectsActors, double affectRadius, int imageID, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = GOODIE_DEPTH)
		: Object(world, false, imageID, startX, startY, dir, size, depth), affectPlayer(affectsPlayer), affectActors(affectsActors), radius(affectRadius) {}

	virtual void beAnnoyed(int annoy_value) { }

protected:

	virtual bool checkRadius();
	virtual void affectPlayerInRadius() { } ;
	virtual void affectObjectInRadius(std::unique_ptr<Object>& object) { };
	virtual bool isBoulder() { return false; }

	bool affectPlayer;
	bool affectActors;

	double radius;
};

class Goodie : public Prop {
public:
	Goodie(StudentWorld& world, int pointValue, const unsigned int soundID, int imageID, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = GOODIE_DEPTH)
		: Prop(world, true, false, ITEM_PICKUP_DISTANCE, imageID, startX, startY, dir, size, depth), obtainSoundEffect(soundID), scoreValue(pointValue) {}

protected:
	void affectPlayerInRadius();
	virtual void updatePlayerInventory()=0;
	
	const unsigned int obtainSoundEffect;
	int scoreValue;
};

const unsigned int WATER_PICKUP_SCORE = 100;
class Water : public Goodie {
public:
	Water(StudentWorld& world, int level, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = GOODIE_DEPTH)
		: Goodie(world, WATER_PICKUP_SCORE, SOUND_GOT_GOODIE, IID_WATER_POOL, startX, startY, dir, size, depth) {
		lifespan = std::max(100, 300 - (10 * level));
	}

	void doSomething();

private:
	void updatePlayerInventory();
	virtual void affectObjectInRadius(std::unique_ptr<Actor>& object) {}

	int lifespan = 0;
};


const double ITEM_REVEAL_DISTANCE = 4.0;

// literally just calls setVisible() again at the start of the constructor
class HiddenGoodie : public Goodie {
public:
	HiddenGoodie(StudentWorld& world, int pointValue, const unsigned int soundID, int imageID, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = GOODIE_DEPTH)
		: Goodie(world, pointValue, soundID, imageID, startX, startY, dir, size, depth) {
		setVisible(false);
	}

protected:
	bool checkRadius();

	virtual void updatePlayerInventory()=0;
	virtual void affectObjectInRadius(std::unique_ptr<Actor>& object) {}
};
const unsigned int BARREL_PICKUP_SCORE = 1'000;
class Barrel : public HiddenGoodie {
public:
	Barrel(StudentWorld& world, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = GOODIE_DEPTH)
		: HiddenGoodie(world, BARREL_PICKUP_SCORE, SOUND_FOUND_OIL, IID_BARREL, startX, startY, dir, size, depth) { }

	void doSomething();

private:
	void updatePlayerInventory();


};
const unsigned int SONAR_PICKUP_SCORE = 75;
class Sonar : public HiddenGoodie {
public:
	Sonar(StudentWorld& world,int level, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = GOODIE_DEPTH)
		: HiddenGoodie(world, SONAR_PICKUP_SCORE, SOUND_GOT_GOODIE, IID_SONAR, startX, startY, dir, size, depth) {
		lifespan = std::max(100, 300 - (10 * level));
	}

	void doSomething();
private:
	void updatePlayerInventory();
	int lifespan = 0;

};
const unsigned int NUGG_PICKUP_SCORE = 10;
const int NUGG_LIFESPAN = 100;
class Nugg : public HiddenGoodie {
public:
	Nugg(StudentWorld& world, bool usingAsBribe, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = GOODIE_DEPTH)
		: HiddenGoodie(world, NUGG_PICKUP_SCORE, SOUND_GOT_GOODIE, IID_GOLD, startX, startY, dir, size, depth) {
		affectPlayer = !usingAsBribe;
		affectActors = usingAsBribe;
		setVisible(usingAsBribe);
	}

	void doSomething();

private:
	void updatePlayerInventory();

	int lifespan = NUGG_LIFESPAN;
};
const int SQUIRT_TRAVEL_DISTANCE = 4;
class Squirt : public Prop {
public:
	Squirt(StudentWorld& world, int startX, int startY, Direction dir, double size = 1.0, unsigned int depth = 1.0)
		: Prop(world,true,true,ITEM_PICKUP_DISTANCE, IID_WATER_SPURT,startX,startY,dir,size,depth){
		affectPlayer = false;
		affectActors = true;
		setVisible(true);
		lifespan = SQUIRT_TRAVEL_DISTANCE;
		currentPosition.first = startX;
		currentPosition.second = startY;
		
	}
	void doSomething();
	double getDistanceToPlayer();
	double getDistanceToActor(std::unique_ptr<Actor>& object);

	bool checkRadius();
	void affectPlayerInRadius();
	void affectObjectInRadius(std::unique_ptr<Actor>& object);
	void move();
private:
	int lifespan = 0;
	std::pair<int, int> currentPosition;
};


enum class BoulderState {
	Stable,
	Jostling,
	Falling
};

const int BOULDER_WAIT_TIME = 30;

class Boulder : public Prop {
public:
	Boulder(StudentWorld& world, int startX, int startY, Direction dir = down, double size = 1.0, unsigned int depth = BOULDER_DEPTH)
		: Prop(world, true, true, ITEM_PICKUP_DISTANCE, IID_BOULDER, startX, startY, dir, size, depth) {}

	void doSomething();

	bool isBoulder() { return true; }

private:
	void affectPlayerInRadius();
	void affectObjectInRadius(std::unique_ptr<Actor>& object);

	bool hasIceUnder();
	bool hasBoulderUnder();

	BoulderState state = BoulderState::Stable;
	int jostleTimer = BOULDER_WAIT_TIME;
};

const double ICE_SIZE = 0.25;

// ice really doesn't need anything more than basic functionality
class IceBlock : public GraphObject {
public:
	IceBlock(int startX, int startY, Direction dir = right, double size = ICE_SIZE, unsigned int depth = ICE_DEPTH)
		: GraphObject(IID_ICE, startX, startY, dir, size, depth) {
		setVisible(true);
	}

	void doSomething() { }

	// stores whatever IceBlock is closest to a Protester's return point
	std::optional<std::pair<int, int>> previous = std::nullopt;
};

const int TUNNEL_START_X = 30;
const int TUNNEL_END_X = 33;
const int TUNNEL_END_Y = 4;

const unsigned int DEFAULT_ICE_DESTROY_RANGE = 4;

class Ice {
public:
	Ice(StudentWorld& world);
	// smart pointers mean that we don't have to delete anything ourselves
	
	std::shared_ptr<IceBlock>& getBlock(int x, int y);
	std::optional<std::pair<int, int>> getPrevBlock(int x, int y);
	
	bool destroyIce(unsigned int x, unsigned int y, unsigned int x_size, unsigned int y_size);

	std::optional<std::pair<unsigned int, unsigned int>> getOpenSquare(unsigned int i);
	std::optional<std::pair<unsigned int, unsigned int>> getIceSquare(unsigned int i);

	size_t getNumOpenSquares() { return openSquares.size(); }
	size_t getNumIceSquares() { return iceSquares.size(); }

private:
	// a less disgusting way to write this would be appreciated
	// using namespace std would just be a stopgap
	// making the elements unique_ptrs without incurring the compiler's wrath would also be cool
	std::array<std::array<std::shared_ptr<IceBlock>, VIEW_HEIGHT>, VIEW_WIDTH> iceObjects;
	std::array<std::array<std::optional<std::pair<int, int>>, VIEW_HEIGHT>, VIEW_WIDTH> prevSpaces;

	std::vector<std::pair<unsigned int, unsigned int>> openSquares;
	std::vector<std::pair<unsigned int, unsigned int>> iceSquares;

	void populateAvailableSquares();

	void calculateExitPaths();

	StudentWorld& w;
};


#endif // ACTOR_H_
