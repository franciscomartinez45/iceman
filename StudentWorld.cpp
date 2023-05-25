#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;



int StudentWorld::init() {
	StudentWorld::player = std::make_unique<IceMan>(getWorld(), 0, 30, 60, GraphObject::right, 1.0, 0);
	ice = std::make_unique<Ice>();
	return GWSTATUS_CONTINUE_GAME;
}


int StudentWorld::move() {
	StudentWorld::player->doSomething();

	setStatusBar();

	return GWSTATUS_CONTINUE_GAME;
}
//void StudentWorld::cleanUp(){}
GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// VERY rudimentary for now
// expect more features later
void StudentWorld::setStatusBar() {
	std::stringstream status;
	const std::string separator = "  ";

	// level number
	status << std::setfill(' ') << std::setw(2);
	status << getLevel() << separator;

	// lives
	status << std::setfill(' ') << std::setw(1);
	status << getLives() << separator;

	// health
	status << std::setfill(' ') << std::setw(3);
	status << getPlayerHealth() << '%' << separator;
	
	// water
	status << std::setfill(' ') << std::setw(2);
	status << "?" << separator;

	// gold
	status << std::setfill(' ') << std::setw(2);
	status << "?" << separator;

	// oil left
	status << std::setfill(' ') << std::setw(2);
	status << "?" << separator;

	// sonar
	status << std::setfill(' ') << std::setw(2);
	status << "?" << separator;

	// score
	status << std::setfill('0') << std::setw(6);
	status << getScore();

	setGameStatText(status.str());
}
