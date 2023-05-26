#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
#include <array>

// distance formula time
float Object::getDistanceTo(Object& other) {
	return sqrt(pow(this->getX() - other.getX(), 2) + pow(this->getY() - other.getY(), 2));
}

bool Object::contains(unsigned int x, unsigned int y) {
	return ((x >= getX()) and (x < getX() + int(getSize() * 4)) and (y >= getY()) and (y < getY() + int(getSize() * 4)));
}

bool Object::intersects(unsigned int x, unsigned int y, unsigned int x_size, unsigned int y_size) {
	return (contains(x, y) or contains(x + x_size - 1, y + y_size - 1)
		or contains(x, y + y_size - 1) or contains(x + x_size - 1, y));
}

// health is almost never set in the game,
// so health can only be incremented or decremented
void IceMan::offsetHealth(int offset) {
	health += offset;
	if (health > max_health)
		health = max_health;
	else if (health < 0)
		health = 0;
}

void IceMan::doSomething() {
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
			// SEE ABOVE
			break;
		case KEY_PRESS_ESCAPE:
			// CONTINUE SEEING ABOVE
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
				if (new_pos.first < 0 or new_pos.first > ACTOR_WIDTH_LIMIT or new_pos.second < 0 or new_pos.second > ACTOR_WIDTH_LIMIT)
					new_pos = prev_pos;
				moveTo(new_pos.first, new_pos.second);
			}
		}
	}
} // glorious bracket staircase

const double WATER_PICKUP_DISTANCE = 3.0;
const unsigned int WATER_PICKUP_AMOUNT = 5;
const unsigned int WATER_PICKUP_SCORE = 100;

void Water::doSomething() {
	if (!dead) {
		if (getDistanceTo(*(w.getPlayer().get())) <= WATER_PICKUP_DISTANCE) {
			dead = true;

			w.playSound(SOUND_GOT_GOODIE);
			w.getPlayer()->addWater(WATER_PICKUP_AMOUNT);
			w.increaseScore(WATER_PICKUP_SCORE);
		}
		else {
			if (lifespan == 0)
				dead = true;
			else
				lifespan--;
		}
	}
}

Ice::Ice(StudentWorld& world) {
	// from <ranges>
	// think python ranges, but C++ified (i.e. painfully verbose)
	for (auto i : std::ranges::iota_view(0, VIEW_WIDTH)) {
		for (auto j : std::ranges::iota_view(0, VIEW_HEIGHT - ACTOR_HEIGHT)) {
			if ((i < TUNNEL_START_X) || (i > TUNNEL_END_X) || (j < TUNNEL_END_Y)) {
				iceObjects[i][j] = std::make_unique<IceBlock>(world, i, j);
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
