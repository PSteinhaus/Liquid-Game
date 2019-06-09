#include "main.hpp"
#include <algorithm>	// fill

FourDirectionMap::FourDirectionMap() {
	clear();
}

void FourDirectionMap::clear(){
	std::lock_guard<std::mutex> lockGuard(engine.rootDirectionMutex);
	for(int i=0; i<Engine::WIDTH*Engine::HEIGHT; i++) {
		for(int j=0; j<4; j++)
			directions[i][j] = false;
	}
	//std::fill(directions, directions+(Engine::WIDTH*Engine::HEIGHT)*4, false);
}

const int FourDirectionMap::directionToIndex(Direction direction) {
	switch(direction) {
		case UP: return 0;
		case LEFT: return 1;
		case DOWN: return 2;
		case RIGHT: return 3;
		default: return -1;
	}
	return -1;
}

const Direction FourDirectionMap::indexToDirection(int index) {
	switch(index) {
		case 0: return UP;
		case 1: return LEFT;
		case 2: return DOWN;
		case 3: return RIGHT;
		default: return NEUTRAL;
	}
	return NEUTRAL;
}

bool FourDirectionMap::checkDirection(int x, int y, Direction direction)
{
	int index = directionToIndex(direction);
	if( index == -1 ) return false;
	std::lock_guard<std::mutex> lockGuard(engine.rootDirectionMutex);
	return directions[x+y*Engine::WIDTH][directionToIndex(direction)];
}

void FourDirectionMap::addDirection(int x, int y, Direction direction) {
	int index = directionToIndex(direction);
	if( index == -1 ) return;
	std::lock_guard<std::mutex> lockGuard(engine.rootDirectionMutex);
	directions[x+y*Engine::WIDTH][directionToIndex(direction)] = true;
}

void FourDirectionMap::removeDirection(int x, int y, Direction direction) {
	int index = directionToIndex(direction);
	if( index == -1 ) return;
	std::lock_guard<std::mutex> lockGuard(engine.rootDirectionMutex);
	directions[x+y*Engine::WIDTH][directionToIndex(direction)] = false;
}

void FourDirectionMap::clearField(int x, int y) {
	for( int i=0; i<4; i++ )
		directions[x+y*Engine::WIDTH][i] = false;
}

const int FourDirectionMap::getTilePosition(int x, int y) {
	bool left = checkDirection(x,y,LEFT);
	bool up = checkDirection(x,y,UP);
	bool down = checkDirection(x,y,DOWN);
	bool right = checkDirection(x,y,RIGHT);
	switch( left ) {
		case true:
			switch( up ) {
				case true:
					switch( down ) {
						case true:
							switch( right ) {
								case true:
									return 197;
								case false:
									return 180;
							}
						case false:
							switch( right ) {
								case true:
									return 193;
								case false:
									return 217;
							}
					}
				case false:
					switch( down ) {
						case true:
							switch ( right ) {
								case true:
									return 194;
								case false:
									return 191;
							}
						case false:
							switch( right ) {
								case true:
									return 196;
								case false:
									return 181;
							}
					}
			}	
		case false:
			switch( up ) {
				case true:
					switch( down ) {
						case true:
							switch( right ) {
								case true:
									return 195;
								case false:
									return 179;
							}
						case false:
							switch( right ) {
								case true:
									return 192;
								case false:
									return 184;
							}
					}
				case false:
					switch( down ) {
						case true:
							switch ( right ) {
								case true:
									return 218;
								case false:
									return 183;
							}
						case false:
							switch( right ) {
								case true:
									return 182;
								case false:
									return 0;
							}
					}
			}	
	}
	return 0;
}

void FourDirectionMap::render() {
	for (int x=0; x<engine.WIDTH; ++x)
		for (int y=0; y<engine.HEIGHT; ++y) {
			TCOD_console_set_char( NULL, x, y, getTilePosition(x,y) );
			float greyValue = 255 * engine.getRootCharge(x,y);
			unsigned char greyVal = greyValue>255 ? 255 : (unsigned char) greyValue;
			TCOD_console_set_char_foreground( NULL , x, y, {greyVal,greyVal,greyVal} );
			// DEBUG
			
			auto sources = engine.getRootSources(x,y);
			engine.lockRootSource(x,y);
			if( sources->begin() != sources->end() ) {
				int distance = sources->begin()->second;
				TCOD_console_set_char_background( NULL , x, y, {(unsigned char)(distance*30),0,0}, TCOD_BKGND_SET );
			}
			engine.unlockRootSource(x,y);
		}
}


EightDirectionMap::EightDirectionMap() {
	clear();
}

void EightDirectionMap::clear(){
	for(int i=0; i<Engine::WIDTH*Engine::HEIGHT; i++) {
		for(int j=0; j<8; j++)
			directions[i][j] = false;
	}
	//std::fill(directions, directions+(Engine::WIDTH*Engine::HEIGHT)*8, false);
}

const int EightDirectionMap::directionToIndex(Direction direction) {
	return (int)direction - 1;
}

bool EightDirectionMap::checkDirection(int x, int y, Direction direction)
{
	return directions[x+y*Engine::WIDTH][directionToIndex(direction)];
}

void EightDirectionMap::addDirection(int x, int y, Direction direction) {
	directions[x+y*Engine::WIDTH][directionToIndex(direction)] = true;
}

void EightDirectionMap::removeDirection(int x, int y, Direction direction) {
	directions[x+y*Engine::WIDTH][directionToIndex(direction)] = false;
}