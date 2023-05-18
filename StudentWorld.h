#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <memory>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir)
	{
	}

	virtual int init();

	virtual int move();

	virtual void cleanUp(){}
	
	StudentWorld& getWorld() {
		return *this;
	}
	
	std::unique_ptr<Ice>& getIce() {
		return ice;
	}
	std::unique_ptr<Sonar>& getSonar() {
		return sonar;
	}

private:
	std::unique_ptr<IceMan> player = nullptr;
	std::unique_ptr<Ice> ice = nullptr;
	std::unique_ptr<Sonar> sonar = nullptr;
};

#endif // STUDENTWORLD_H_
