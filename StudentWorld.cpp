#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <cstdlib>

using namespace std;



int StudentWorld::init() {
	StudentWorld::ice = std::make_unique<Ice>();
	StudentWorld::player = std::make_unique<IceMan>(getWorld(), 0, 30, 60, GraphObject::right, 1.0, 0);
	StudentWorld::initSonar(getWorld());
	return GWSTATUS_CONTINUE_GAME;
}


int StudentWorld::move() {
	StudentWorld::player->doSomething();
	return GWSTATUS_CONTINUE_GAME;
}
//void StudentWorld::cleanUp(){}
GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}


// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

void StudentWorld::initSonar(StudentWorld& world) {
	//spawn one sonar + others for level
	world.getSonar().push_back(make_shared<Sonar>(IID_SONAR, 0, 30, GraphObject::right, 1.0, 0));
}