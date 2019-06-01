#include "main.hpp"
#include <stdlib.h>     /* rand */
#include <algorithm>	// fill
#include <iostream>

HeightMap::HeightMap(char initialHeight) {
	for (int i=0; i<engine.WIDTH*engine.HEIGHT; ++i) {
		heights[i] = initialHeight;
	}
}

void HeightMap::clear() {
	std::fill(heights, heights+(Engine::WIDTH*Engine::HEIGHT), 0);
}

void  HeightMap::setHeight(int x, int y, char height) {
	heights[x+y*engine.WIDTH] = height;
}

void HeightMap::changeHeight(int x, int y, char change) {
	heights[x+y*engine.WIDTH] += change;
	if( heights[x+y*engine.WIDTH] < 0 )
		heights[x+y*engine.WIDTH] = 0;
	else if( heights[x+y*engine.WIDTH] > Engine::DEPTH )
		heights[x+y*engine.WIDTH] = Engine::DEPTH;
}

char HeightMap::changeHeightConserving(int x, int y, char change) {
	heights[x+y*engine.WIDTH] += change;
	char amountChanged = change;
	if( heights[x+y*engine.WIDTH] < 0 ) {
		amountChanged = (-1)*(heights[x+y*engine.WIDTH] - change);
		heights[x+y*engine.WIDTH] = 0;
	}
	else if( heights[x+y*engine.WIDTH] > Engine::DEPTH ) {
		amountChanged = heights[x+y*engine.WIDTH] - Engine::DEPTH;
		heights[x+y*engine.WIDTH] = Engine::DEPTH;
	}
	return amountChanged;
}

void HeightMap::moveHeight(int x0, int y0, int x_dest, int y_dest, char amount) {
	changeHeight(x0, y0, -amount);
	changeHeight(x_dest, y_dest, amount);
}

void HeightMap::moveHeightConserving(int x0, int y0, int x_dest, int y_dest, char amount) {
	char amountTaken = -1 * changeHeightConserving(x0, y0, -amount);
	char amountAdded =      changeHeightConserving(x_dest, y_dest, amountTaken);
	changeHeight(x0, y0, amountTaken-amountAdded);
}

char  HeightMap::getHeight(int x, int y) const {
	return heights[x+y*engine.WIDTH];
}

void HeightMap::render() const {
	for (int x=1; x<engine.WIDTH-1; ++x)
		for (int y=1; y<engine.HEIGHT-1; ++y) {
			TCODConsole::root->setCharBackground(x,y, TCODColor::white * ( (float)getHeight(x,y)/((float)engine.DEPTH) )*0.5 );
		}
}


ColorMap::ColorMap() : HeightMap(0) {
	for (int i=0; i<engine.WIDTH*engine.HEIGHT; ++i) {
		players[i] = NULL;
	}
}

void ColorMap::clear() {
	HeightMap::clear();
	std::fill(players, &players[(Engine::WIDTH-1)*(Engine::HEIGHT-1)], nullptr);
}

void  ColorMap::setPlayer(int x, int y, Player* player) {
	players[x+y*engine.WIDTH] = player;
}

Player* ColorMap::getPlayer(int x, int y) const {
	return players[x+y*engine.WIDTH];
}

void ColorMap::moveColor(int x0, int y0, int x_dest, int y_dest, char amount) {
	Player* player = getPlayer(x0,y0);
	moveHeight(x0,y0, x_dest,y_dest, amount);	// move the color
	if ( getHeight(x0,y0) == 0 )				// reset old field if player is now gone
		setPlayer(x0,y0,NULL);
	setPlayer(x_dest,y_dest,player);			// set ownership of new field
}

void ColorMap::moveColorConserving(int x0, int y0, int x_dest, int y_dest, char amount) {
	Player* player = getPlayer(x0,y0);
	moveHeightConserving(x0,y0, x_dest,y_dest, amount);	// move the color
	if ( getHeight(x0,y0) == 0 )				// reset old field if player is now gone
		setPlayer(x0,y0,NULL);
	if ( getHeight(x_dest,y_dest) != 0 )
		setPlayer(x_dest,y_dest,player);		// set ownership of new field
}

void ColorMap::render() const {
	for (int x=0; x<engine.WIDTH; ++x)
		for (int y=0; y<engine.HEIGHT; ++y) {
			Player* playerHere = getPlayer(x,y);
			if( playerHere ) {
				TCOD_console_set_char_background( NULL , x, y, TCOD_color_multiply_scalar(playerHere->col, ((float)getHeight(x,y)/(float)Engine::DEPTH) ), TCOD_BKGND_ADD);
				//TCOD_console_set_char( NULL, x, y, playerHere==engine.players[0] ? 'G' : 'R');
				//TCOD_console_set_char_foreground( NULL , x, y, {10,10,10} );
			}
		}
}

void ColorMap::update() {
	// create all the liquid-like dynamics
	bool clk = engine.turn%2;
	int x0; int y0;
	if( clk ) {
		x0 = 0; y0 = 0;
	}
	else {
		x0 = engine.WIDTH-1; y0 = engine.HEIGHT-1;
	}

	for (int x=x0 ; clk ? x<engine.WIDTH : x>-1 ; clk ? ++x : --x )
		for (int y=y0 ; clk ? y<engine.HEIGHT : y>-1 ; clk ? ++y : --y ) {
			Player* player = getPlayer(x,y);
			if (player) {						// if there is someone there
				bool moved = false;				// remember that you haven't moved the color yet
				char localHeight = engine.getHeight(x,y);	// get the height of the color
				char localTotalHeight = engine.totalHeight(x,y,player);	// and the height of the color + other factors (HeightMap of player and global one)
				DirectionMap::Direction direction = player->getDirection(x,y);	// check for a forced direction
				if( direction != DirectionMap::NEUTRAL )						// if true try to move there
				{
					int dx=0; int dy=0;
					DirectionMap::calcDxDy(&dx, &dy, direction);			// calculate your destination coordinates based on the given direction
					int x_dest = x + dx;									//
					int y_dest = y + dy;
					Player* player_dest = getPlayer(x_dest,y_dest);			// check for a player on the field you want to move to
					if ( player_dest && player_dest!=player ) {				// if it's a different player
						engine.fightBetween(x,y, x_dest,y_dest);			// fight
					}														// else try to move there
					else if ( localHeight>1 && engine.totalGroundHeight(x_dest,y_dest,player) < localTotalHeight && engine.totalHeight(x_dest,y_dest,player) < Engine::DEPTH )
																			// settle for an "equal" ground-height or better (if it's not full already anyway)
						moveColorConserving(x,y,x_dest,y_dest, localHeight/2);						// (this means color can't flow up-hill, but it can get more concentrated when "pumped" this way)
				}
				else	// if there is no forced direction try to move randomly
				{
					// check a random neighbour for lower height
					for (int i=0; i<4; ++i)
					{
						int x_dest = x + rand() % 3 -1;
						int y_dest = y + rand() % 3 -1;
						Player* player_dest = getPlayer(x_dest,y_dest);
						if ( player_dest && player_dest!=player ) {
							engine.fightBetween(x,y, x_dest,y_dest);
							moved = true;
							break;
						}
						if ( engine.totalHeight(x_dest,y_dest,player)+1 < localTotalHeight )	// check the neighbour for lower height
						{
							moveColor(x,y,x_dest,y_dest);										// and move a unit of color there if true
							moved = true;
							break;
						}
					}
					if( !moved ) {
						// try settling for an "equal" one but try to stick to the rest of the fluid
						int max_x_dest = -1;
						int max_y_dest = -1;
						char max_neighbours = 1;
						for (int i=0; i<9; ++i)
						{
							int x_dest = x + rand() % 3 -1;
							int y_dest = y + rand() % 3 -1;
							Player* player_dest = getPlayer(x_dest,y_dest);
							if ( player_dest && player_dest!=player ) {
								engine.fightBetween(x,y, x_dest,y_dest);
								moved = true;
								break;
							}
							if ( localHeight>1 && engine.totalHeight(x_dest,y_dest,player) < localTotalHeight )	// if the field is about as high as you
							{
								// count the neighbours
								char neighbours = 0;
								for( int dx=-1; dx<=1; ++dx )
									for( int dy=-1; dy<=1; ++dy )
										if (engine.totalHeight(x_dest+dx,y_dest+dy, player) == localTotalHeight) ++neighbours;

								if( neighbours > max_neighbours ) {
									max_neighbours = neighbours;
									max_x_dest = x_dest;
									max_y_dest = y_dest;
								}
							}
						}
						// if you found a suitable field
						if( max_x_dest >= 0 && max_y_dest >= 0 && !moved ) {
							moveColor(x,y,max_x_dest,max_y_dest);		// move there
							moved = true;
						}
					}
					// just try settling for an "equal" one
					if( !moved ) {
						int x_dest = x + rand() % 3 -1;
						int y_dest = y + rand() % 3 -1;
						Player* player_dest = getPlayer(x_dest,y_dest);
						if ( player_dest && player_dest!=player ) {
							engine.fightBetween(x,y, x_dest,y_dest);
						}
						else if ( localHeight>1 && engine.totalHeight(x_dest,y_dest,player) < localTotalHeight )	// try settling for an "equal" one
							moveColor(x,y,x_dest,y_dest);
					}
				}
			}
		}
}

char ColorMap::pumpAt(int x, int y, char amount, Player* pumpingPlayer) {
	const char startingAmount = amount;
	// clear the boolArray
	std::fill(boolArray, boolArray+(Engine::WIDTH*Engine::HEIGHT), 0);
	// include the first field
	lastIndex = 0;
	indexArray[0] = y*Engine::WIDTH+x;
	boolArray[y*Engine::WIDTH+x] = true;
	// find fields to pump to
	for(unsigned int i=0; i<=lastIndex; ++i) {
		if (pumpFromIndex(indexArray[i],amount,pumpingPlayer)) break;
	}
	return startingAmount - amount; // return how much color has been successfully pumped
}

bool ColorMap::pumpFromIndex(unsigned int i, char& amount, Player* pumpingPlayer) {
	int x = i%Engine::WIDTH;
	int y = i/Engine::WIDTH;
	Player* player = getPlayer(x,y);
	if( player!=NULL && player!=pumpingPlayer ) return false;	// don't pump your enemy's field
	// first check the given field
	char freeSpace = Engine::DEPTH - engine.totalHeight(x,y,pumpingPlayer);
	if( freeSpace>0 )	// if there is free space on that field
	{
		setPlayer(x,y,pumpingPlayer);
		changeHeight(x,y,freeSpace);		// pump it full
		amount -=freeSpace;
		if(amount <= 0) {					// you're done
			changeHeight(x,y,amount);		// make sure you didn't pump too much
			amount = 0;						// and set amount to 0 to indicate that all has been pumped
			return true;
		}
	}
	// if you're not done yet add its neighbours to the list to be checked
	DirectionMap::Direction direction = pumpingPlayer->getDirection(x,y);	// check if there is a forced direction
	if( direction == DirectionMap::NEUTRAL )						// if not add all neighbours to the list
	{
		for( int dx = -1; dx <=1; ++dx )
			for( int dy = -1; dy <=1; ++dy ) {
				if( !dx && !dy ) continue;								// your own field is, of course, not worth checking
				if( !engine.inMap(x+dx,y+dy) ) continue;
				unsigned int index = (y+dy)*Engine::WIDTH + (x+dx);
				Player* otherPlayer = getPlayer(x+dx,y+dy);
				if( boolArray[ index ]==false && (!otherPlayer || otherPlayer==pumpingPlayer) && engine.totalGroundHeight(x+dx,y+dy,pumpingPlayer)<Engine::DEPTH ) {		// if the field hasn't been checked/added yet and belongs to you
					indexArray[++lastIndex] = index;									// add it to the list
					boolArray[index] = true;											// and mark it as added
				}
			}
	}
	else															// if true add only the one correct neighbour to the list
	{
		int dx=0; int dy=0;
		DirectionMap::calcDxDy(&dx,&dy,direction);
		if( !engine.inMap(x+dx,y+dy) ) return false;
		unsigned int index = (y+dy)*Engine::WIDTH + (x+dx);
		Player* otherPlayer = getPlayer(x+dx,y+dy);
		if( boolArray[ index ]==false && (!otherPlayer || otherPlayer==pumpingPlayer) && engine.totalGroundHeight(x+dx,y+dy,pumpingPlayer)<Engine::DEPTH ) {	// if the field hasn't been checked/added yet and belongs to you
			indexArray[++lastIndex] = index;									// add it to the list
			boolArray[index] = true;											// and mark it as added
		}
	}

	return false;
}


void DirectionMap::setDirection(int x, int y, Direction direction) {
	heights[x+y*engine.WIDTH] = (char)direction;
}

DirectionMap::Direction DirectionMap::getDirection(int x, int y) const {
	return static_cast<DirectionMap::Direction>(getHeight(x,y));
}

const DirectionMap::Direction DirectionMap::calcDirection(char dx, char dy) {
	switch(dx) {
		case -1:
			switch(dy) {
				case -1:
					return DOWN_LEFT;
				case 0:
					return LEFT;
				case 1:
					return UP_LEFT;
				default: return NEUTRAL;
			}
		case 0:
			switch(dy) {
				case -1:
					return DOWN;
				case 0:
					return NEUTRAL;
				case 1:
					return UP;
				default: return NEUTRAL;
			}
		case 1:
			switch(dy) {
				case -1:
					return DOWN_RIGHT;
				case 0:
					return RIGHT;
				case 1:
					return UP_RIGHT;
				default: return NEUTRAL;
			}
	}
	return NEUTRAL;
}

void DirectionMap::calcDxDy(int* dx, int* dy, Direction direction) {
	switch(direction) {
		case UP:
			*dx = 0; *dy = -1;
			break;
		case DOWN:
			*dx = 0; *dy = 1;
			break;
		case RIGHT:
			*dx = 1; *dy = 0;
			break;
		case LEFT:
			*dx = -1; *dy = 0;
			break;
		case DOWN_RIGHT:
			*dx = 1; *dy = 1;
			break;
		case DOWN_LEFT:
			*dx = -1; *dy = 1;
			break;
		case UP_LEFT:
			*dx = -1; *dy = -1;
			break;
		case UP_RIGHT:
			*dx = 1; *dy = -1;
			break;
		default:
			break;
	}
	return;
}

const int DirectionMap::getTilePosition(Direction direction) {
	switch(direction) {
		case UP:
			return 24;
		case DOWN:
			return 25;
		case RIGHT:
			return 26;
		case LEFT:
			return 27;
		case DOWN_RIGHT:
			return 208;
		case DOWN_LEFT:
			return 209;
		case UP_LEFT:
			return 210;
		case UP_RIGHT:
			return 211;
		default:
			return 0;
	}
	return 0;
}
