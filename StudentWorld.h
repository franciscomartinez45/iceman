#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <memory>
#include <list>

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
		: GameWorld(assetDir)
	{
		srand(time(NULL));
	}

	virtual int init();

	virtual int move();

	virtual void cleanUp(){
		player.reset();
		actors.clear();
		props.clear();
		ice.reset();
	}
	
	StudentWorld& getWorld() {
		return *this;
	}
	
	std::unique_ptr<Ice>& getIce() {
		return ice;
	}

	std::unique_ptr<IceMan>& getPlayer() {
		return player;
	}

	std::list<std::unique_ptr<Actor>>& getActors() {
		return actors;
	}

	std::list<std::unique_ptr<Prop>>& getProps() {
		return props;
	}
	void spawnSquirt();
	void spawnPlayerNugg();
	void revealObjects();
private:
	bool isIntersectingObject(unsigned int x, unsigned int y);
	bool isIntersectingObject(std::pair<unsigned int, unsigned int> p);

	void spawnWater();
	void spawnObjectInIce(ObjectType type);
	bool vetIceSpawnCoords(std::pair<unsigned int, unsigned int> p,
		unsigned int x_range_start, unsigned int x_range_end, unsigned int y_range_start, unsigned int y_range_end);

	int getPlayerHealth() { return double(player->getHealth()) / double(ICEMAN_MAX_HEALTH) * 100; }
	
	void setStatusBar();

	std::unique_ptr<IceMan> player = nullptr;
	std::list<std::unique_ptr<Actor>> actors;
	std::list<std::unique_ptr<Prop>> props;
	std::unique_ptr<Ice> ice = nullptr;
};

#endif // STUDENTWORLD_H_
