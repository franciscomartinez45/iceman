#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>

// distance formula time
float Object::getDistanceTo(Object& other) {
	return sqrt(pow(this->getX(), other.getX()) + pow(this->getY(), other.getY()));
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
	bool destroyed_ice = false;
	auto& ice = w.getIce();
	for (auto i : std::ranges::iota_view(getX(), getX() + ACTOR_HEIGHT)) {
		for (auto j : std::ranges::iota_view(getY(), getY() + ACTOR_HEIGHT)) {
			auto& block = ice->getBlock(i, j);
			if (block != nullptr) {
				block.reset();
				destroyed_ice = true;
			}
		}
	}
	if (destroyed_ice)
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
				if (new_pos.first < 0 || new_pos.first > ACTOR_WIDTH_LIMIT || new_pos.second < 0 || new_pos.second > ACTOR_WIDTH_LIMIT)
					new_pos = prev_pos;
				moveTo(new_pos.first, new_pos.second);
			}
		}
	}
} // glorious bracket staircase
