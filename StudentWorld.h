#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <memory>
#include <list>

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
		actors.clear();
		goodies.clear();
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

private:
	bool isIntersectingObject(unsigned int x, unsigned int y);
	bool isIntersectingObject(std::pair<unsigned int, unsigned int> p);

	void spawnWater();

	int getPlayerHealth() { return double(player->getHealth()) / double(ICEMAN_MAX_HEALTH) * 100; }

	void setStatusBar();

	std::unique_ptr<IceMan> player = nullptr;
	std::list<std::unique_ptr<Actor>> actors;
	std::list<std::unique_ptr<Prop>> goodies;
	std::unique_ptr<Ice> ice = nullptr;
};

#endif // STUDENTWORLD_H_
