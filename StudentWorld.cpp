#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <array>

using namespace std;

int StudentWorld::init() {
	StudentWorld::player = std::make_unique<IceMan>(getWorld(), 30, 60);
	ice = std::make_unique<Ice>();

	// spawn ice goodies
	int num_boulders = std::min(getLevel() / 2 + 2, unsigned int(9));
	int num_nuggs = std::max(5 - getLevel(), unsigned int(2));
	int num_barrels = std::min(getLevel() + 2, unsigned int(21));
	
	
		
	for (auto i : std::ranges::iota_view(0, num_boulders))
		spawnObjectInIce(ObjectType::Boulder);
	for (auto i : std::ranges::iota_view(0, num_nuggs))
		spawnObjectInIce(ObjectType::Nugg);
	for (auto i : std::ranges::iota_view(0, num_barrels)) 
		spawnObjectInIce(ObjectType::Barrel);
		player->addOil(num_barrels);
	
	// useful test case - you may want to repurpose this for the barrels
	//props.push_back(std::make_unique<Nugg>(getWorld(), false, 60, 60));

	return GWSTATUS_CONTINUE_GAME;
}


const int CHANCE_OF_SONAR = 5;
int StudentWorld::move() {
	setStatusBar();
	if (player->getOil() > 0) {
		StudentWorld::player->doSomething();
		for (auto& object : objects)
			object->doSomething();

		int goodie_chance = getLevel() * 25 + 300; // magic numbers - boss's orders
		
		if (rand() % goodie_chance == 0) {
			if (rand() % CHANCE_OF_SONAR == 0) {
				spawnObjectInIce(ObjectType::Sonar);
			}
			else {
				spawnWater();
			}
		}

		// handle dead objects
		if (player->isDead()) {
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}

		for (auto it = objects.begin(); it != objects.end(); it++) {
			if ((*it)->isDead()) {
				it = objects.erase(it);
				if (it == objects.end())
					break;
			}
		}

		return GWSTATUS_CONTINUE_GAME;
	}
	return GWSTATUS_FINISHED_LEVEL;
	
}
//void StudentWorld::cleanUp(){}
GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

void StudentWorld::spawnPlayerNugg() {
	objects.push_back(std::make_unique<Nugg>(getWorld(), true, player->getX(), player->getY()));
}

// checks to see if a point is intersecting any major game objects
// (the player, goodies, etc.)
bool StudentWorld::isIntersectingObject(unsigned int x, unsigned int y) {
	if (player->intersects(x, y))
		return true;
	else {
		for (auto& object : objects) {
			if (!object->isActor()) {
				if (object->intersects(x, y))
					return true;
			}
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
		objects.push_back(std::make_unique<Water>(getWorld(), getLevel(), water_coords.first, water_coords.second));
	}
}

const int BOULDER_SPAWN_LIMIT = 20;
void StudentWorld::spawnObjectInIce(ObjectType type) {
	int x_range_end = VIEW_WIDTH - ACTOR_HEIGHT;
	int y_range_start = (type != ObjectType::Boulder) ? 0 : BOULDER_SPAWN_LIMIT;
	int y_range_end = VIEW_HEIGHT - (ACTOR_HEIGHT * 2);

	// this method is somewhat inefficient,
	// but the fact that it only runs at the
	// start of each level means that it
	// won't be much of an issue (hopefully)
	std::pair<unsigned int, unsigned int> spawn_coords;
	do {
		spawn_coords = ice->getIceSquare(rand()).value(); // the map is guaranteed to have open ice squares at this point
	} while (!vetIceSpawnCoords(spawn_coords, 0, x_range_end, y_range_start, y_range_end));

	switch (type) {
	case ObjectType::Boulder:
		objects.push_back(std::make_unique<Boulder>(getWorld(), spawn_coords.first, spawn_coords.second));
		ice->destroyIce(spawn_coords.first, spawn_coords.second, ACTOR_HEIGHT, ACTOR_HEIGHT);
		break;

	case ObjectType::Nugg:
		objects.push_back(std::make_unique<Nugg>(getWorld(), false, spawn_coords.first, spawn_coords.second));
		break;

	case ObjectType::Barrel:
		objects.push_back(std::make_unique<Barrel>(getWorld(), true, spawn_coords.first, spawn_coords.second));
		break;
	
	case ObjectType::Sonar:
		objects.push_back(std::make_unique<Sonar>(getWorld(), getWorld().getLevel(), true, spawn_coords.first, spawn_coords.second));
		break;
	
}
}

const double MIN_SPAWN_RADIUS = 6.0;
// determines whether or not a given coordinate set can spawn
// tests whether it lies within the given range
// and whether or not it falls too close to any preexisting objects
bool StudentWorld::vetIceSpawnCoords(std::pair<unsigned int, unsigned int> p,
	unsigned int x_range_start, unsigned int x_range_end, unsigned int y_range_start, unsigned int y_range_end) {
	
	// range checking
	if (p.first < x_range_start or p.first > x_range_end or p.second < y_range_start or p.second > y_range_end)
		return false;

	// radius checking
	for (auto& object : objects) {
		if (object->getDistanceTo(p) <= MIN_SPAWN_RADIUS) {
			return false;
		}
	}
	return true;
}

// TODO: ADD BARREL COUNT
void StudentWorld::setStatusBar() {
	std::stringstream status;
	const std::string separator = "  ";

	// level number
	status << "Lvl:";
	status << std::setfill(' ') << std::setw(2);
	status << getLevel() << separator;

	// lives
	status << "Lives:";
	status << std::setfill(' ') << std::setw(1);
	status << getLives() << separator;

	// health
	status << "Health:";
	status << std::setfill(' ') << std::setw(3);
	status << getPlayerHealth() << '%' << separator;
	
	// water
	status << "Water:";
	status << std::setfill(' ') << std::setw(2);
	status << player->getWater() << separator;

	// gold
	status << "Gold:";
	status << std::setfill(' ') << std::setw(2);
	status << player->getNuggs() << separator;

	// oil left
	status << "Oil Left:";
	status << std::setfill(' ') << std::setw(2);
	status << player->getOil() << separator;

	// sonar
	status << "Sonar:";
	status << std::setfill(' ') << std::setw(2);
	status << player->getSonar() << separator;

	// score
	status << "Pts:";
	status << std::setfill('0') << std::setw(6);
	status << getScore();

	setGameStatText(status.str());
}

void StudentWorld::revealObjects() {
	for (auto& object : objects) {
		if (!object->isActor()) {
			if (!object->isVisible() and object->getDistanceTo(*(player.get())) <= 6.0) {
				object->setVisible(true);
			}
		}
	}
}

void StudentWorld::spawnSquirt() {
	objects.push_back(make_unique<Squirt>(getWorld(), player->getX(), player->getY(), player->getDirection(), 1.0, 1.0));
}
