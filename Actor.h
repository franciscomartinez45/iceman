#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
#include <array>
#include <bitset>
#include <queue>

// distance formula time
float Object::getDistanceTo(Object& other) {
	return sqrt(pow(this->getX() - other.getX(), 2) + pow(this->getY() - other.getY(), 2));
}

float Object::getDistanceTo(std::pair<unsigned int, unsigned int> p) {
	return sqrt(pow(this->getX() - p.first, 2) + pow(this->getY() - p.second, 2));
}

bool Object::contains(unsigned int x, unsigned int y) {
	return ((x >= getX()) and (x < getX() + int(getSize() * 4)) and (y >= getY()) and (y < getY() + int(getSize() * 4)));
}

bool Object::intersects(unsigned int x, unsigned int y, unsigned int x_size, unsigned int y_size) {
	return (contains(x, y) or contains(x + x_size - 1, y + y_size - 1)
		or contains(x, y + y_size - 1) or contains(x + x_size - 1, y));
}

double Object::getDistanceToPlayer() {
	return getDistanceTo(*(w.getPlayer().get()));
}

std::optional<double> Object::getDistanceToActor(std::unique_ptr<Object>& actor) {
	return getDistanceTo(*(actor.get()));
}

bool Actor::willCollide(std::pair<int, int> new_pos) {
	if (new_pos.first < 0 or new_pos.first > ACTOR_WIDTH_LIMIT or new_pos.second < 0 or new_pos.second > ACTOR_WIDTH_LIMIT)
		return true;

	for (auto& object : w.getObjects()) {
		if (object->isBoulder() and object->intersects(new_pos.first, new_pos.second))
			return true;
	}

	return false;
}

void IceMan::doSomething() {
	if (!isDead()) {
		
		// TESTING FUNCTION
		// REMOVE SOON!
		/*if (isReturning) {
			auto b = w.getIce()->getPrevBlock(getX(), getY());
			if (!b) {
				w.playSound(SOUND_FOUND_OIL);
				isReturning = false;
			}
			else {
				auto p = b.value();
				moveTo(p.first, p.second);
			}
			return;
		}*/

		// remove intersecting ice
		if (w.getIce()->destroyIce(getX(), getY(), ACTOR_HEIGHT, ACTOR_HEIGHT))
			w.playSound(SOUND_DIG);

		int key;
		if (IceMan::w.getKey(key)) {
			Direction new_dir = none;
			switch (key) {
			case KEY_PRESS_UP:
				new_dir = up;
				break;
			case KEY_PRESS_DOWN:
				new_dir = down;
				break;
			case KEY_PRESS_LEFT:
				new_dir = left;
				break;
			case KEY_PRESS_RIGHT:
				new_dir = right;
				break;
			case KEY_PRESS_SPACE:
				//subtract water count
				if (getWater() > 0) {
					useWater();
					//water sound
					w.playSound(SOUND_PLAYER_SQUIRT);
					w.spawnSquirt();
				}
				//squirt water
				break;
			case KEY_PRESS_TAB:
				if (nuggs > 0) {
					w.spawnPlayerNugg();
					nuggs--;
				}
				break;
			case KEY_PRESS_ESCAPE:
				beAnnoyed(INSTAKILL_DAMAGE);
				break;
			case 'Z':
			case 'z':
				if (getSonar() > 0) {
					//reveal items in radius
					w.revealObjects();
					w.playSound(SOUND_SONAR);
					//subtract sonar count
					useSonar();
				}
				break;
			}

			if (new_dir != none) {
				if (new_dir != getDirection())
					setDirection(new_dir);

				else {
					auto new_pos = std::make_pair<int>(getX(), getY());
					auto prev_pos = new_pos;

					switch (new_dir) {
					case up:
						new_pos.second++;
						break;
					case down:
						new_pos.second--;
						break;
					case left:
						new_pos.first--;
						break;
					case right:
						new_pos.first++;
						break;
					}

					// bounds checking
					if (willCollide(new_pos))
						new_pos = prev_pos;
					moveTo(new_pos.first, new_pos.second);
				}
			}
		}
	}
} // glorious bracket staircase

void IceMan::beAnnoyed(int annoy_value) {
	health -= annoy_value;
	if (health <= 0) {
		health = 0;
		dead = true;
		w.playSound(SOUND_PLAYER_GIVE_UP);
	}
}

void IceMan::getReturnPath() {
	std::cout << "Start!\n";
	std::optional<std::pair<int, int>> coords = std::make_pair(getX(), getY());

	while (coords != std::nullopt) {
		auto& coordsv = coords.value();
		std::cout << coordsv.first << ' ' << coordsv.second << '\n';
		coords = w.getIce()->getPrevBlock(coordsv.first, coordsv.second);
	}
	std::cout << "DONE\n";
}

Protester::Protester(StudentWorld& world, int startX, int startY, Direction dir = left, double size = 1.0, unsigned int depth = 0)
	: Actor(world, PROTESTER_MAX_HEALTH, IID_PLAYER, startX, startY, dir, size, depth) {
	updateRestTicks();
}

void Protester::doSomething() {
	if (!isDead()) {
		if (!isResting()) {
			if (leavingOilField) {
				moveTowardsOilField();
			}
			else if (!attemptShout()) {
				if (!attemptMoveToIceman()) {
					pickNewDirection();
				}
			}
		}
	}
}

const unsigned int WATER_PICKUP_AMOUNT = 5;

bool Prop::checkRadius() {
	bool found = false;
	if (affectPlayer) {
		if (getDistanceToPlayer() <= radius) {
			affectPlayerInRadius();
			found = true;
		}
	}
	if (affectActors) {
		for (auto& object : w.getObjects()) {
			if (object->isActor()) {
				if (getDistanceToActor(object) <= ITEM_PICKUP_DISTANCE) {
					affectObjectInRadius(object);
					found = true;
				}
			}
		}
	}
	return found;
}

void Goodie::affectPlayerInRadius() {
	dead = true;

	w.playSound(obtainSoundEffect);
	updatePlayerInventory();
	w.increaseScore(scoreValue);
}

void Water::doSomething() {
	if (!dead) {
		if (!checkRadius()) {
			if (lifespan <= 0)
				dead = true;
			else
				lifespan--;
		}
	}
}
void Sonar::doSomething() { //collectible item
	if (!dead) {
		checkRadius();
		if (affectActors) {
			if (lifespan <= 0)
				dead = true;
			else
				lifespan-=10;
		}
	}
}
void Sonar::updatePlayerInventory() {
	w.getPlayer()->addSonar();
}



void Water::updatePlayerInventory() {
	w.getPlayer()->addWater(WATER_PICKUP_AMOUNT);
}

bool HiddenGoodie::checkRadius() {
	if (!isVisible() and getDistanceToPlayer() <= ITEM_REVEAL_DISTANCE) {
		setVisible(true);
		return false;
	}
		
	return Goodie::checkRadius();
}

void Nugg::updatePlayerInventory() {
	w.getPlayer()->addNuggs();
}

void Nugg::doSomething() {
	if (!dead) {
		checkRadius();

		if (affectActors) {
			if (lifespan <= 0)
				dead = true;
			else
				lifespan--;
		}
	}
}
void Barrel::doSomething() {
	if (!dead) {
		checkRadius();
		if (affectActors) {
			dead = true;
		}
	}
}
void Barrel::updatePlayerInventory() {
	w.getPlayer()->foundOil();
}
void Boulder::doSomething() {
	if (!isDead()) {
		switch (state) {
		case BoulderState::Stable:
			if (!hasIceUnder())
				state = BoulderState::Jostling;
			break;

		case BoulderState::Jostling:
			if (jostleTimer == 0) {
				state = BoulderState::Falling;
				w.playSound(SOUND_FALLING_ROCK);
			}
			else
				jostleTimer--;
			break;

		case BoulderState::Falling:
			if (getY() == 0 or hasIceUnder() or hasBoulderUnder())
				dead = true;
			else {
				moveTo(getX(), getY() - 1);
				checkRadius();
			}
				
			break;
		}
	}
}
void Squirt::doSomething() {
	if (lifespan > 0 and !isDead()) {
			switch (w.getPlayer()->getDirection()) {
			case GraphObject::right:
				currentPosition.first++;
				break;
			case GraphObject::left:
				currentPosition.first--;
				break;
			case GraphObject::up:
				currentPosition.second++;
				break;
			case GraphObject::down:
				currentPosition.second--;
				break;
			}
			moveTo(currentPosition.first, currentPosition.second);
			lifespan--;
		}
	
	else { dead = true; }
}
void Boulder::affectPlayerInRadius() {
	w.getPlayer()->beAnnoyed(INSTAKILL_DAMAGE);
}

void Boulder::affectObjectInRadius(std::unique_ptr<Actor>& object) {
	// TODO
}

bool Boulder::hasIceUnder() {
	auto j = getY() - 1;
	if (j < 0)
		return false;

	for (auto i : std::ranges::iota_view(getX(), getX() + ACTOR_HEIGHT)) {
		if (w.getIce()->getBlock(i, j) != nullptr)
			return true;
	}
	return false;
}

bool Boulder::hasBoulderUnder() {
	if ((getY() - ACTOR_HEIGHT) < 0)
		return false;

	for (auto& object : w.getObjects()) {
		if (object->isBoulder()) {
			if ((object->getY() == getY() + ACTOR_HEIGHT - 1)
				and (object->getX() > getX() - ACTOR_HEIGHT)
				and (object->getX() < getX() + ACTOR_HEIGHT))
				return true;
		}
	}
	return false;
}

Ice::Ice(StudentWorld& world) : w(world) {
	for (auto& arr : prevSpaces)
		arr.fill(std::nullopt);

	for (auto& arr : iceObjects)
		arr.fill(nullptr);

	// from <ranges>
	// think python ranges, but C++ified (i.e. painfully verbose)
	for (auto i : std::ranges::iota_view(0, VIEW_WIDTH)) {
		for (auto j : std::ranges::iota_view(0, VIEW_HEIGHT - ACTOR_HEIGHT)) {
			if ((i < TUNNEL_START_X) || (i > TUNNEL_END_X) || (j < TUNNEL_END_Y)) {
				iceObjects[i][j] = std::make_shared<IceBlock>(i, j);
			}
		}
	}

	openSquares.reserve(VIEW_WIDTH * (VIEW_HEIGHT - ACTOR_HEIGHT));
	iceSquares.reserve(VIEW_WIDTH * (VIEW_HEIGHT - ACTOR_HEIGHT));
	populateAvailableSquares();
}

std::shared_ptr<IceBlock>& Ice::getBlock(int x, int y) {
	// includes bounds checking
	return iceObjects.at(x).at(y);
}

std::optional<std::pair<int, int>> Ice::getPrevBlock(int x, int y) {
	return prevSpaces.at(x).at(y);
}


// removes ice in an x_size by y_size block from the given coordinates
// returns false if no ice was in the area, true otherwise
bool Ice::destroyIce(unsigned int x, unsigned int y, unsigned int x_size, unsigned int y_size) {
	bool destroyed_ice = false;
	for (auto i : std::ranges::iota_view(x, x + x_size)) {
		for (auto j : std::ranges::iota_view(y, y + y_size)) {
			auto& block = getBlock(i, j);
			if (block != nullptr) {
				block.reset();
				destroyed_ice = true;
			}
		}
	}
	
	if (destroyed_ice) {
		populateAvailableSquares();
		calculateExitPaths();
	}
		

	return destroyed_ice;
}

// returns a valid iceless point to spawn an object in
std::optional<std::pair<unsigned int, unsigned int>> Ice::getOpenSquare(unsigned int i) {
	if (openSquares.empty())
		return std::nullopt;
	return openSquares.at(i);
}

// getOpenSquare, but for valid ice points
std::optional<std::pair<unsigned int, unsigned int>> Ice::getIceSquare(unsigned int i) {
	if (iceSquares.empty())
		return std::nullopt;
	return iceSquares.at(i);
}

// record which 4x4 squares contain and don't contain ice
// (useful for spawning objects)
// it would do us good to move this into a thread at some point
void Ice::populateAvailableSquares() {
	
	openSquares.erase(openSquares.begin(), openSquares.end());
	iceSquares.erase(iceSquares.begin(), iceSquares.end());

	std::array<int, (VIEW_HEIGHT)> open_vertical_conts;
	open_vertical_conts.fill(0);
	std::array<int, (VIEW_HEIGHT)> ice_vertical_conts;
	ice_vertical_conts.fill(0);

	const int j_range = int(iceObjects.size() - ACTOR_HEIGHT);
	for (auto j : std::ranges::iota_view(0, j_range)) { // ignore upper strip of playfield
		int open_horizontal_cont = 0;
		int ice_horizontal_cont = 0;
		for (auto i : std::ranges::iota_view(0, VIEW_WIDTH)) {
			if (getBlock(i, j) == nullptr) {
				open_horizontal_cont++;
				open_vertical_conts[i]++;
				ice_horizontal_cont = 0;
				ice_vertical_conts[i] = 0;
			}
			else {
				open_horizontal_cont = 0;
				open_vertical_conts[i] = 0;
				ice_horizontal_cont++;
				ice_vertical_conts[i]++;
			}

			if (i >= (ACTOR_HEIGHT - 1) and j >= (ACTOR_HEIGHT - 1)) {
				auto candidate_coordinate = std::make_pair(i - ACTOR_HEIGHT + 1, j - ACTOR_HEIGHT + 1);

				if (open_horizontal_cont >= ACTOR_HEIGHT
					and open_vertical_conts[i] >= ACTOR_HEIGHT
					and open_vertical_conts[i - 1] >= ACTOR_HEIGHT
					and open_vertical_conts[i - 2] >= ACTOR_HEIGHT
					and open_vertical_conts[i - 3] >= ACTOR_HEIGHT) {
					openSquares.push_back(candidate_coordinate);
				}

				if (ice_horizontal_cont >= ACTOR_HEIGHT
					and ice_vertical_conts[i] >= ACTOR_HEIGHT
					and ice_vertical_conts[i - 1] >= ACTOR_HEIGHT
					and ice_vertical_conts[i - 2] >= ACTOR_HEIGHT
					and ice_vertical_conts[i - 3] >= ACTOR_HEIGHT) {
					iceSquares.push_back(candidate_coordinate);
				}
			}
		}
	} // return of the glorious bracket staircase

	// this works now (I think) and is no longer necessary
	/*std::cout << "OPEN\n";
	for (auto& i : openSquares) {
		std::cout << i.first << ' ' << i.second << '\n';
	}
	std::cout << "ICE\n";
	for (auto i : iceSquares) {
		std::cout << i.first << ' ' << i.second << '\n';
	}*/
}

const int RETURN_POINT_X = VIEW_WIDTH - ACTOR_HEIGHT;
const int RETURN_POINT_Y = VIEW_HEIGHT - ACTOR_HEIGHT;

void Ice::calculateExitPaths() {
	int path_point_x = RETURN_POINT_X;
	int path_point_y = RETURN_POINT_Y;

	// this is effectively a 60x60 (objects will never be further than that)
	// array of bools
	std::bitset<(RETURN_POINT_X * (RETURN_POINT_Y + 1) + RETURN_POINT_X + 1)> seen_points(false);
	std::queue<int> path_points;

	// the real deal will use the actors as stopping points
	// but this will be useful as a test case
	auto& player = w.getPlayer();
	// hacky way to condense two points into a single unique value
	int point_hash = path_point_x + (path_point_y * (RETURN_POINT_Y + 1));
	seen_points[point_hash] = true;
	path_points.push(point_hash);

	while (!path_points.empty()) {

		auto current_point = path_points.front();
		path_points.pop();
		path_point_x = current_point % (RETURN_POINT_Y + 1);
		path_point_y = current_point / (RETURN_POINT_Y + 1);

		//std::cout << current_point << ' ' << path_point_x << ' ' << path_point_y << '\n';

		// find next points
		std::array<std::pair<int, int>, 4> neighbors = {
			std::make_pair(path_point_x + 1, path_point_y),
			std::make_pair(path_point_x - 1, path_point_y),
			std::make_pair(path_point_x, path_point_y + 1),
			std::make_pair(path_point_x, path_point_y - 1)
		};

		for (auto& point : neighbors) {
			// check if point is within bounds
			//std::cout << "Test " << point.first << ' ' << point.second << '\n';
			if (point.first >= 0 and point.first <= RETURN_POINT_X
				and point.second >= 0 and point.second <= RETURN_POINT_Y) {
				// check if point is an open space
				if (!w.isIntersectingBoulder(point.first, point.second)
					and !w.isIntersectingIce(point.first, point.second)) {
					// point cannot have been seen already
					int prospective_point_hash = point.first + (point.second * (RETURN_POINT_Y + 1));
					if (!seen_points[prospective_point_hash]) {
						seen_points[prospective_point_hash] = true;
						path_points.push(prospective_point_hash);
						prevSpaces[point.first][point.second] = std::make_pair(path_point_x, path_point_y);
						//std::cout << point.first << ' ' << point.second << ' ' << prospective_point_hash << '\n';
					}
				}
			}
		}
	}
} // extremely glorious bracket staircase

double Squirt::getDistanceToPlayer() { return 0.0; }
double Squirt::getDistanceToActor(std::unique_ptr<Actor>& object) {return 0.0; }

bool Squirt::checkRadius() { return true; }
void Squirt::affectPlayerInRadius(){}
void Squirt::affectObjectInRadius(std::unique_ptr<Actor>& object){}
