#include "StudentWorld.h"
#include "Actor.h"
#include <string>

using namespace std;

Ice* ic = new Ice();

int StudentWorld::init() {
	player = std::make_unique<IceMan>(getWorld(), 0, 30, 60, GraphObject::right, 1.0, 0);
	return GWSTATUS_CONTINUE_GAME;
}


int StudentWorld::move() {
	player->doSomething();
	return GWSTATUS_CONTINUE_GAME;
}
//void StudentWorld::cleanUp(){}
GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}


// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

