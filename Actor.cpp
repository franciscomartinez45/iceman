#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
#include <array>

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
				// TODO
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

bool IceMan::willCollide(std::pair<int, int> new_pos) {
	if (new_pos.first < 0 or new_pos.first > ACTOR_WIDTH_LIMIT or new_pos.second < 0 or new_pos.second > ACTOR_WIDTH_LIMIT)
		return true;
	
	for (auto& prop : w.getProps()) {
		if (prop->isBoulder() and prop->intersects(new_pos.first, new_pos.second))
			return true;
	}

	return false;
}

const unsigned int WATER_PICKUP_AMOUNT = 5;

double Prop::getDistanceToPlayer() {
	return getDistanceTo(*(w.getPlayer().get()));
}

double Prop::getDistanceToActor(std::unique_ptr<Actor>& actor) {
	return getDistanceTo(*(actor.get()));
}

bool Prop::checkRadius() {
	bool found = false;
	if (affectPlayer) {
		if (getDistanceToPlayer() <= radius) {
			affectPlayerInRadius();
			found = true;
		}
	}
	if (affectActors) {
		for (auto& actor : w.getActors()) {
			if (getDistanceToActor(actor) <= ITEM_PICKUP_DISTANCE) {
				affectObjectInRadius(actor);
				found = true;
			}
		}
	}
	return found;
} // glorious bracket staircase

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
void Sonar::doSomething() {
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

	for (auto& prop : w.getProps()) {
		if (prop->isBoulder()) {
			if ((prop->getY() == getY() + ACTOR_HEIGHT - 1)
				and (prop->getX() > getX() - ACTOR_HEIGHT)
				and (prop->getX() < getX() + ACTOR_HEIGHT))
				return true;
		}
	}
	return false;
}

Ice::Ice() {
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
// bounds checking is handled through the mod operator
// since we'll most often be coupling this with rng calls
std::optional<std::pair<unsigned int, unsigned int>> Ice::getOpenSquare(unsigned int i) {
	if (openSquares.empty())
		return std::nullopt;
	return openSquares.at(i % openSquares.size());
}

// getOpenSquare, but for valid ice points
std::optional<std::pair<unsigned int, unsigned int>> Ice::getIceSquare(unsigned int i) {
	if (iceSquares.empty())
		return std::nullopt;
	return iceSquares.at(i % iceSquares.size());
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
