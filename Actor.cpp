#include "Actor.h"
#include "StudentWorld.h"



void IceMan::doSomething() {
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

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
