#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <memory>
#include <list>
#include <random>

enum class ObjectType {
	Boulder,
	Nugg,
	Barrel,
	Sonar

};

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir) {}

	virtual int init();

	virtual int move();

	virtual void cleanUp();

	StudentWorld& getWorld() {
		return *this;
	}

	std::unique_ptr<Ice>& getIce() {
		return ice;
	}

	std::unique_ptr<IceMan>& getPlayer() {
		return player;
	}

	std::list<std::unique_ptr<Object>>& getObjects() {
		return objects;
	}

	auto& getGenerator() {
		return generator;
	}

	void spawnSquirt();
	void spawnPlayerNugg();
	void revealObjects();

	bool isIntersectingBoulder(unsigned int x, unsigned int y);
	bool isIntersectingIce(unsigned int x, unsigned int y);
private:
	bool isIntersectingObject(unsigned int x, unsigned int y);
	bool isIntersectingObject(std::pair<unsigned int, unsigned int> p);

	void spawnWater();
	void spawnObjectInIce(ObjectType type);
	bool vetIceSpawnCoords(std::pair<unsigned int, unsigned int> p,
		unsigned int x_range_start, unsigned int x_range_end, unsigned int y_range_start, unsigned int y_range_end);

	void attemptSpawnProtester();

	int getPlayerHealth() { return double(player->getHealth()) / double(ICEMAN_MAX_HEALTH) * 100; }

	void setStatusBar();

	std::unique_ptr<IceMan> player = nullptr;
	std::list<std::unique_ptr<Object>> objects;
	std::unique_ptr<Ice> ice = nullptr;

	std::default_random_engine generator;

	int targetProtesters;
	int protesterAddTime;
	int protesterAddTimer = 0;
};

#endif // STUDENTWORLD_H_
