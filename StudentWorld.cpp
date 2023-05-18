#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <cstdlib>
using namespace std;



int StudentWorld::init() {
	StudentWorld::player = std::make_unique<IceMan>(getWorld(), 0, 30, 60, GraphObject::right, 1.0, 0);
	StudentWorld::ice = std::make_unique<Ice>();
	auto randomPair = std::make_pair<int, int>(rand() % VIEW_WIDTH, rand() % VIEW_HEIGHT);
	StudentWorld::sonar = std::make_unique<Sonar>(IID_SONAR,randomPair.first,randomPair.second,GraphObject::right,1.0,0);
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

