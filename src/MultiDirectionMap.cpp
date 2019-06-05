#include "main.hpp"
#include <algorithm>	// fill

FourDirectionMap::FourDirectionMap() {
	clear();
}

void FourDirectionMap::clear(){
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

bool FourDirectionMap::checkDirection(int x, int y, Direction direction)
{
	int index = directionToIndex(direction);
	if( index == -1 ) return false;
	return directions[x+y*Engine::WIDTH][directionToIndex(direction)];
}

void FourDirectionMap::addDirection(int x, int y, Direction direction) {
	int index = directionToIndex(direction);
	if( index == -1 ) return;
	directions[x+y*Engine::WIDTH][directionToIndex(direction)] = true;
}

void FourDirectionMap::removeDirection(int x, int y, Direction direction) {
	int index = directionToIndex(direction);
	if( index == -1 ) return;
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
			//TCOD_console_set_char_foreground( NULL , x, y, {10,10,10} );
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