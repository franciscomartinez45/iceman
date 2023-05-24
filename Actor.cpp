#include "Actor.h"
#include "StudentWorld.h"


void IceMan::doSomething() {
	// remove intersecting ice
	bool destroyed_ice = false;
	auto& ice = w.getIce();
	auto& sonar = w.getSonar();
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
		case KEY_PRESS_SPACE://squirt
				squirt_item= std::make_unique<squirt>(IID_WATER_SPURT, IceMan::getX(),  IceMan::getY(), IceMan::getDirection(), 1.0, 1);
				squirt_item->doSomething();
				w.playSound(SOUND_PLAYER_SQUIRT);
				break;

			case KEY_PRESS_TAB:
				 //SEE ABOVE
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

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
void squirt::doSomething() {
	
	auto new_pos = std::make_pair<int, int>(getSquirt().getX(), getSquirt().getY());
	int distance{};

	while (distance < 4) {
		
		switch (getSquirt().getDirection()) {
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
		
		getSquirt().moveTo(new_pos.first, new_pos.second);
		distance++;
	}
	
	

	
	
}
