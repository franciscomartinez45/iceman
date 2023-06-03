#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
#include <array>
#include <random>
#include <bitset>

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
	if (new_pos.first < 0 or new_pos.first > ACTOR_WIDTH_LIMIT or new_pos.second < 0 or new_pos.second > ACTOR_HEIGHT_LIMIT)
		return true;

	for (auto& object : w.getObjects()) {
		if (object->isBoulder() and object->intersects(new_pos.first, new_pos.second))
			return true;
	}

	return false;
}

std::pair<int, int> Actor::getPointInDirection(Direction dir) {
	auto new_pos = std::make_pair<int>(getX(), getY());
	switch (dir) {
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
	return new_pos;
}

bool Actor::moveInDirection(Direction dir) {
	auto new_pos = getPointInDirection(dir);
	auto prev_pos = std::make_pair<int>(getX(), getY());;

	// bounds checking
	bool moved = true;
	if (willCollide(new_pos)) {
		new_pos = prev_pos;
		moved = false;
	}

	moveTo(new_pos.first, new_pos.second);
	return moved;
}

void IceMan::doSomething() {
	if (!isDead()) {
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
				if (water > 0) {
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
					moveInDirection(new_dir);
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
			ticksSinceLastPerpendicularTurn++;
		}
	}
}

void Protester::beAnnoyed(int annoy_value)
{
	health -= annoy_value;
	if (health <= 0) {
		dead = true;
		w.playSound(SOUND_PROTESTER_GIVE_UP);
	}
	else { w.playSound(SOUND_PROTESTER_ANNOYED); }
}

const int PROTESTER_BRIBE_AMOUNT = 25;
void Protester::beBribed() {
	w.playSound(SOUND_PROTESTER_FOUND_GOLD);
	w.increaseScore(PROTESTER_BRIBE_AMOUNT);
}

bool Protester::attemptMoveToIceman() {
	if (straightLineToIceman()) {
		Direction new_dir = none;
		if (getX() == w.getPlayer()->getX()) {
			if (getY() < w.getPlayer()->getY())
				new_dir = up;
			else
				new_dir = down;
		}
		if (getY() == w.getPlayer()->getY()) {
			if (getX() < w.getPlayer()->getX())
				new_dir = right;
			else
				new_dir = left;
		}
		setDirection(new_dir);
		moveInDirection(new_dir);

		return true;
	}
	return false;
}

const int NUM_DIRECTIONS = 4;
void Protester::pickNewDirection() {
	std::bitset<NUM_DIRECTIONS> viable_directions(false);
	for (auto i : std::ranges::iota_view(0, NUM_DIRECTIONS)) {
		viable_directions.set(i, !willCollide(getPointInDirection(Direction(i + 1))));
	}

	if (numSquaresToMoveInCurrentDirection <= 0) {
		auto new_dir = std::uniform_int_distribution<int>(0, viable_directions.count() - 1)(w.getGenerator());
		while (!viable_directions[new_dir]) {
			new_dir++;
		}

		setDirection(Direction(new_dir + 1));
		updateNumSquares();
	}

	if (ticksSinceLastPerpendicularTurn >= PROTESTER_PERPENDICULAR_THRESHOLD) {
		// pontificate if a perpendicular path is possible
		auto perpendicular_options = std::make_pair<Direction, Direction>(none, none);
		switch (getDirection()) {
		case up:
		case down:
			if (viable_directions[left - 1])
				perpendicular_options.first = left;
			if (viable_directions[right - 1])
				perpendicular_options.second = right;
			break;
		case left:
		case right:
			if (viable_directions[up - 1])
				perpendicular_options.first = up;
			if (viable_directions[down - 1])
				perpendicular_options.second = down;
			break;
		}

		if (perpendicular_options.first != none or perpendicular_options.second != none) {
			if (perpendicular_options.first == none)
				setDirection(perpendicular_options.second);
			else if (perpendicular_options.second == none)
				setDirection(perpendicular_options.first);
			else {
				auto use_first = std::uniform_int_distribution<int>(0, 1)(w.getGenerator());
				setDirection((use_first == 0) ? perpendicular_options.first : perpendicular_options.second);
			}
			updateNumSquares();
			ticksSinceLastPerpendicularTurn = 0;
		}
	}

	if (!moveInDirection(getDirection())) {
		numSquaresToMoveInCurrentDirection = 0;
	}

	numSquaresToMoveInCurrentDirection--;
}

void Protester::moveToCenter() {
	moveTo(getX() - 1, getY());
	moveTicks--;
}

void Protester::moveTowardsOilField() //moves to center of field

{ //go from 60,60 to 30,60

	if (moveTicks > 0) {
		moveToCenter();
	}




}

const int SHOUT_COOLOFF_LENGTH = 15;
bool Protester::attemptShout() {
	if (getDistanceToPlayer() <= 4) {
		if (shoutCooloff <= 0) {
			w.playSound(SOUND_PROTESTER_YELL);
			w.getPlayer()->beAnnoyed(2);
			shoutCooloff = SHOUT_COOLOFF_LENGTH;
		}
		else if (shoutCooloff > 0) {
			shoutCooloff--;
		}
		return true;
	}
	else
		return false; 
}
bool Protester::willCollide(std::pair<int, int> new_pos) {
	if (!Actor::willCollide(new_pos)) {
		for (auto& object : w.getObjects()) {
			if (object->isBoulder() and object->getDistanceTo(*this) <= ITEM_PICKUP_DISTANCE) {
				return true;
			}
		}

		for (auto i : std::ranges::iota_view(new_pos.first, new_pos.first + ACTOR_HEIGHT)) {
			for (auto j : std::ranges::iota_view(new_pos.second, new_pos.second + ACTOR_HEIGHT)) {
				if (w.getIce()->getBlock(i, j) != nullptr) {
					return true;
				}
			}
		}
		return false;
	}

	return true;
}

bool Protester::isResting() {
	if (currentWaitTicks <= 0) {
		currentWaitTicks = waitTicks;
		return false;
	}
	else {
		currentWaitTicks--;
		return true;
	}
}


bool Protester::isFacingIceman() {

	
	auto player = std::pair<int, int>(w.getPlayer()->getX(), w.getPlayer()->getY());
	auto dir = none;
	if (player.first == getX()) { // same row
		if (player.second < getY()) {//protester is to the right of iceman
			dir = down; //
			
		}
		else if (player.second >= getY()) { //protester is to the left or at the position of iceman
			dir = up;
		}
	}
	else if (player.second == getY()) { //same column
		if (player.first < getX()) {//protester is above iceman
			dir = left;
		}
		else if (player.first >= getX()) {//protester is below or same height as iceman
			dir = right;
		}
	}

	if (dir == getDirection()) { //if facing iceman check for empty path
		switch (dir) {
		case left:
			for (int i : std::ranges::iota_view(player.first, getX())) { //checks all values on same row from player to protester for collision
				if (w.getIce()->getBlock(i, getY()) != nullptr) {
					return false;
				}
			}
			break;

		case right:
			for (int i : std::ranges::iota_view(getX(), player.first)) { //checks all blocks to the right of protester
				if (w.getIce()->getBlock(i, getY()) != nullptr) {
					return false;
				}
			}
			break;
		case up:
			for (int i : std::ranges::iota_view(getY(), player.second)) { //from protester y to player y
				if (w.getIce()->getBlock(getX(), i) != nullptr) {
					return false;
				}
			}
			break;
		case down:
			for (int i : std::ranges::iota_view(player.second, getY())) { //from iceman y to protester y
				if (w.getIce()->getBlock(getX(), i) != nullptr){
					return false;
				}
			}
			break;
		}
		return true;
	}
	else {
		return false;
	}
}

const double ANNOY_DISTANCE = 4.0;
bool Protester::straightLineToIceman() {
	if (getDistanceToPlayer() < ANNOY_DISTANCE) {
		return false;
	}
	if (getX() == w.getPlayer()->getX()) {
		auto y_min = std::min(getY(), w.getPlayer()->getY());
		auto y_max = std::max(getY(), w.getPlayer()->getY());

		std::pair<int, int> check_coords = std::make_pair(getX(), y_min);

		for (auto i : std::ranges::iota_view(y_min, y_max + 1)) {
			check_coords.second = i;
			if (willCollide(check_coords))
				return false;
		}
		return true;
	}
	if (getY() == w.getPlayer()->getY()) {
		auto x_min = std::min(getX(), w.getPlayer()->getX());
		auto x_max = std::max(getX(), w.getPlayer()->getX());

		std::pair<int, int> check_coords = std::make_pair(x_min, getY());

		for (auto i : std::ranges::iota_view(x_min, x_max + 1)) {
			check_coords.first = i;
			if (willCollide(check_coords))
				return false;
		}
		return true;
	}
	return false;
}

const int MIN_NUM_SQUARES = 8;
const int MAX_NUM_SQUARES = 60;
void Protester::updateNumSquares() {
	numSquaresToMoveInCurrentDirection = std::uniform_int_distribution<int>(MIN_NUM_SQUARES, MAX_NUM_SQUARES)(w.getGenerator());
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
				lifespan -= 10;
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

void Nugg::affectObjectInRadius(std::unique_ptr<Object>& object) {
	object->beBribed();
	dead = true;
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
const int ANNOYANCE_POINTS = 2;
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


		if (w.getIce()->getBlock(currentPosition.first, currentPosition.second) == nullptr) {
			for (auto& p : w.getObjects()) {
				if (p->isActor() and p->getX() == currentPosition.first and p->getY() == currentPosition.second) {
					p->beAnnoyed(ANNOYANCE_POINTS);
					dead = true;
					lifespan = 0;
				}
			}
			moveTo(currentPosition.first, currentPosition.second);
			lifespan--;
		}
		else {
			lifespan = 0;
			dead = true;
		}
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
	// from <ranges>
	// think python ranges, but C++ified (i.e. painfully verbose)
	for (auto i : std::ranges::iota_view(0, VIEW_WIDTH)) {
		for (auto j : std::ranges::iota_view(0, VIEW_HEIGHT - ACTOR_HEIGHT)) {
			if ((i < TUNNEL_START_X) || (i > TUNNEL_END_X) || (j < TUNNEL_END_Y)) {
				iceObjects[i][j] = std::make_unique<IceBlock>(i, j);
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

	if (destroyed_ice)
		populateAvailableSquares();

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

	for (auto j : std::ranges::iota_view(0, int(iceObjects.size() - ACTOR_HEIGHT))) { // ignore upper strip of playfield
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
double Squirt::getDistanceToPlayer() { return 0.0; }
double Squirt::getDistanceToActor(std::unique_ptr<Actor>& object) { return 0.0; }

bool Squirt::checkRadius() { return true; }
void Squirt::affectPlayerInRadius() {}
void Squirt::affectObjectInRadius(std::unique_ptr<Actor>& object) { }
