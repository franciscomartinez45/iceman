#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <memory>
#include<vector>
#include <unordered_map>

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
	std::vector<std::shared_ptr<Sonar>>& getSonar() {
		return sonar;
	}
	void initSonar(StudentWorld& world);
	
private:
	

	std::unique_ptr<Actors> player = nullptr;
	std::unique_ptr<Ice> ice = nullptr;
	std::vector<std::shared_ptr<Sonar>> sonar;
};

#endif // STUDENTWORLD_H_
