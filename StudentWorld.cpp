#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <array>

using namespace std;

int StudentWorld::init() {
	StudentWorld::player = std::make_unique<IceMan>(getWorld(), 30, 60);
	ice = std::make_unique<Ice>(getWorld());

	return GWSTATUS_CONTINUE_GAME;
}


const int CHANCE_OF_SONAR = 5;
int StudentWorld::move() {
	setStatusBar();

	StudentWorld::player->doSomething();
	for (auto& actor : actors)
		actor->doSomething();
	for (auto& goodie : goodies)
		goodie->doSomething();

	int goodie_chance = getLevel() * 25 + 300; // magic numbers - boss's orders
	goodie_chance = 10; // testing
	if (rand() % goodie_chance == 0) {
		if (rand() % CHANCE_OF_SONAR == 0) {
			// spawn sonar here
		}
		else {
			spawnWater();
		}
	}

	// handle dead objects
	for (auto it = actors.begin(); it != actors.end(); it++) {
		if ((*it)->isDead()) {
			it = actors.erase(it);
			if (it == actors.end())
				break;
		}
	}
	for (auto it = goodies.begin(); it != goodies.end(); it++) {
		if ((*it)->isDead()) {
			it = goodies.erase(it);
			if (it == goodies.end())
				break;
		}
	}

	return GWSTATUS_CONTINUE_GAME;
}
//void StudentWorld::cleanUp(){}
GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// checks to see if a point is intersecting any major game objects
// (the player, goodies, etc.)
bool StudentWorld::isIntersectingObject(unsigned int x, unsigned int y) {
	if (player->intersects(x, y))
		return true;
	else {
		for (auto& goodie : goodies) {
			if (goodie->intersects(x, y))
				return true;
		}
	}
	return false;
}

bool StudentWorld::isIntersectingObject(std::pair<unsigned int, unsigned int> p) {
	return isIntersectingObject(p.first, p.second);
}

void StudentWorld::spawnWater() {
	auto coord_index = rand();
	auto water_coords_o = ice->getOpenSquare(coord_index);
	if (water_coords_o) {
		auto water_coords = water_coords_o.value();
		if (isIntersectingObject(water_coords)) {
			auto initial_coords = water_coords;
			do {
				coord_index++;
				water_coords = ice->getOpenSquare(coord_index).value();
			} while (isIntersectingObject(water_coords) and water_coords != initial_coords);
			if (water_coords == initial_coords) {
				return;
			}
		}
		goodies.push_back(std::make_unique<Water>(getWorld(), getLevel(), water_coords.first, water_coords.second));
	}
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
	status << player->getWater() << separator;

	// gold
	status << std::setfill(' ') << std::setw(2);
	status << player->getNuggs() << separator;

	// oil left
	status << std::setfill(' ') << std::setw(2);
	status << "?" << separator;

	// sonar
	status << std::setfill(' ') << std::setw(2);
	status << player->getSonar() << separator;

	// score
	status << std::setfill('0') << std::setw(6);
	status << getScore();

	setGameStatText(status.str());
}
