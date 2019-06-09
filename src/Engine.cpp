#include "main.hpp"
#include <stdlib.h>     /* rand */
#include <limits>
#include <algorithm>
#include <set>
#include <iostream>
#include <thread>

Engine::Engine() : turn(0), lastKey(), gameStatus(RUNNING), structures()
{
	TCODSystem::setFps(FPS);
	TCOD_console_set_custom_font("terminalAdapted16x16.png", 6, 16, 16);
	TCODConsole::initRoot(WIDTH, HEIGHT, "liquid_strategy prototype", false, TCOD_RENDERER_GLSL);
	players[0] = new Player( {0,255,0}, 0 );
	players[1] = new Player( {255,0,0}, 1 );
	players[2] = new Player( {255,255,0}, 2 );
	players[3] = new Player( {0,0,255}, 3 );

	colorMap = new ColorMap();
	globalHeightMap = new HeightMap(100);
	rootDirections = new FourDirectionMap();

	addCreature( new SolarTree(15,45) );
	addCreature( new SolarTree(5,45) );
	/*
	for( int x=1; x<WIDTH-1; ++x )
		for( int y=1; y<HEIGHT-1; ++y )
			globalHeightMap->setHeight(x,y,0);
	setPlayer(10,12,players[0]);
	setPlayer(50,25,players[1]);
	addStructure( new Spawner(10,12,players[0],0.2) );
	addStructure( new Spawner(50,25,players[1],0.2) );
	*/
	loadBitmap("risiko.bmp");
}

Engine::~Engine() {
	for(int i=0;i<4;++i)
		delete players[i];
	delete colorMap;
	delete globalHeightMap;
	delete rootDirections;
	for( auto it=structures.begin(); it!=structures.end(); it++ )
		delete (*it);
	for( auto it=creatures.begin(); it!=creatures.end(); it++ )
		delete (*it);
}

void 	Engine::addCreature(Creature* creature) {
	creatures.push_back(creature);
}

void	Engine::removeCreature(Creature* creature) {
	for( auto it = creatures.begin(); it != creatures.end(); ++it ) {
		if( *it == creature ) {
			creatures.erase(it);
			delete creature;
		}
	}
}

void 	Engine::addStructure(Structure* structure) {
	structures.push_back(structure);
}

void	Engine::removeStructure(Structure* structure) {
	for( auto it = structures.begin(); it != structures.end(); ++it ) {
		if( *it == structure ) {
			structures.erase(it);
			delete structure;
		}
	}
}

Player* Engine::getPlayer(int x, int y) const {
	return colorMap->getPlayer(x,y);
}

char	Engine::getHeight(int x, int y) const {
	return colorMap->getHeight(x,y);
}

char	Engine::getGlobalHeight(int x, int y) const {
	return globalHeightMap->getHeight(x,y);
}

char	Engine::totalGroundHeight(int x, int y, const Player* player) const {
	char gh = globalHeightMap->getHeight(x,y);
	char ph = player->getHeight(x,y);
	int h = (ph>gh ? ph : gh) ;
	return h>CHAR_MAX ? CHAR_MAX : (char)h ;
}

char	Engine::totalHeight(int x, int y, Player* player) const {
	char gh = globalHeightMap->getHeight(x,y);
	char ph = player->getHeight(x,y);
	int h = getHeight(x,y) + (ph>gh ? ph : gh) ;
	return h>CHAR_MAX ? CHAR_MAX : (char)h ;
}

void	Engine::setField (int x, int y, Player* player, char height) {
	setPlayer(x,y,player);
	setHeight(x,y,height);
}

void	Engine::setPlayer(int x, int y, Player* player) {
	colorMap->setPlayer(x,y,player);
}

void	Engine::setHeight(int x, int y, char height) {
	colorMap->setHeight(x,y,height);
}

void	Engine::setGlobalHeight(int x, int y, char height) {
	globalHeightMap->setHeight(x,y,height);
}

void	Engine::clearLevel() {
	colorMap->clear();
	globalHeightMap->clear();
	for( auto it = structures.begin(); it != structures.end(); ++it ) {
		if( *it != NULL )
			delete *it;
	}
	structures.clear();
}

void	Engine::loadBitmap(const char* filename)
{
    int i;
    FILE* f = fopen(filename, "rb");
    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    int size = 3 * width * height;
    unsigned char data[size]; // allocate 3 bytes per pixel
    fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
    fclose(f);

    for(i = 0; i < size; i += 3)			// switch color values because windows stores them in a funny way
    {
        unsigned char tmp = data[i];
        data[i] = data[i+2];
        data[i+2] = tmp;
    }

    // clear the world
    clearLevel();
    // build the global HeightMap according to the bitmap
    for( int x=0; x<WIDTH; ++x )
	    for( int y=0; y<HEIGHT; ++y ) {
	    	unsigned char r = data[(3*(x+y*width))];
	    	unsigned char g = data[(3*(x+y*width))+1];
	    	unsigned char b = data[(3*(x+y*width))+2];
	    	if( r == g && g == b ) {	// if the bitmap is grey set height
	    		char height = r<CHAR_MAX ? r : CHAR_MAX;
	    		setGlobalHeight(x,y, height );
	    	}
	    	else if ( r==255 && g==0 && b==0 ) {	// if it's red place a spawner for player 1
				addStructure( new Spawner(x,y,players[0]) );
	    	}
	    	else if ( r==0 && g==255 && b==0 ) {	// if it's green place a spawner for player 2
				addStructure( new Spawner(x,y,players[1]) );
	    	}
	    	else if ( r==255 && g==255 && b==0 ) {	// if it's yellow place a spawner for player 3
				addStructure( new Spawner(x,y,players[2]) );
	    	}
	    	else if ( r==0 && g==0 && b==255 ) {	// if it's blue place a spwaner for player 4
				addStructure( new Spawner(x,y,players[3]) );
	    	}
	    	else if ( r==100 && g==200 && b==255 ) {	// free spawner
				addStructure( new Spawner(x,y,nullptr) );
	    	}
	    }
    
}

void	Engine::changeHeight(int x, int y, char change) {
	colorMap->changeHeight(x,y,change);
}

char	Engine::pumpAt(int x, int y, char amount, Player* pumpingPlayer) {
	return colorMap->pumpAt(x,y,amount,pumpingPlayer);
}

void	Engine::fightBetween(int x1, int y1, int x2, int y2) {
	char height1 = getHeight(x1,y1);
	char height2 = getHeight(x2,y2);
	char die1 = rand() % (height1+1);
	char die2 = rand() % (height2+1);
	if( die2 > die1 ) {							// field 2 wins
		setHeight(x1,y1,0);						// field 1 loses all units
		moveColor(x2,y2,x1,y1, height2);		// field 2 moves all units into field 1
	} else if( die1 > die2 ) {					// else field 1 wins
		setHeight(x2,y2,0);						// field 2 loses all units
		moveColor(x1,y1,x2,y2, height1);		// field 2 moves all units into field 1
	}
}

void	Engine::moveColor(int x0, int y0, int x_dest, int y_dest, char amount) {
	colorMap->moveColor(x0, y0, x_dest, y_dest, amount);
}


void Engine::update() {
	++turn;
	/// FOR LATER
	for (int i=0; i<4; ++i) {
		players[i]->update();
	}
	
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &lastKey, NULL);
	if (lastKey.vk == TCODK_ESCAPE) {
		gameStatus = EXIT;
	}

	for( auto it=structures.begin(); it!=structures.end(); ++it ) {
		(*it)->update();
	}
	if( perSecond(CREATURE_UPDATES_PER_SECOND) ) {
		for( auto it=creatures.begin(); it!=creatures.end(); ++it ) {
			(*it)->update();
		}
	}
	if( perSecond(20) ) {
		// let the liquids flow
		colorMap->update();
	}
}

void Engine::render() {
	TCODConsole::root->clear();

	globalHeightMap->render();	// render the world height
	rootDirections->render();	// render the roots
	colorMap->render();	// render the fluids
	for( auto it=structures.begin(); it!=structures.end(); ++it )	// render the structures
		(*it)->render();
	for( auto it=creatures.begin(); it!=creatures.end(); ++it )		// render the creatures
		(*it)->render();
	for (int i=0; i<4; ++i) {	// render the cursors
		players[i]->render();
	}
}

bool Engine::inMap(int x, int y) const {
	// returns whether a position is actually on the board
	return x >= 1 && x < WIDTH-1 && y >= 1 && y < HEIGHT-1;
}

bool Engine::perSecond(float repetitions) const {		// returns true "repetitions" times per second
	if( repetitions > FPS ) return true;
	return turn % (unsigned int)(FPS/repetitions) == 0;
}

template <typename Map>
bool key_compare (Map const *lhs, Map const *rhs) {
    return lhs->size() == rhs->size()
        && std::equal(lhs->begin(), lhs->end(), rhs->begin(), 
                      [] (auto a, auto b) { return a.first == b.first; });	// this line provides the comparision function for std::equal as a lambda expression
}

bool Engine::checkRootDirection(int x, int y, Direction direction) {
	return rootDirections->checkDirection( x, y, direction);
}

bool Engine::checkRootConnection(int x, int y, Direction direction) {
	int dx=0, dy=0;
	DirectionMap::calcDxDy(&dx,&dy, direction);
	return rootDirections->checkDirection( x, y, direction) && rootDirections->checkDirection( x+dx, y+dy, DirectionMap::opposingDirection(direction) );
}

void Engine::addRootDirection(int x, int y, Direction direction) {
	std::thread rootAdditionThread(&Engine::addRootDirectionThread, this, x,y,direction);
	rootAdditionThread.detach();
}

void Engine::addRootDirectionThread(int x, int y, Direction direction) {
	rootMutex.lock();
	std::cout << "rootMutex aquired\n";
	if( !rootAllowed(x,y,direction) ) {
		rootMutex.unlock();
		return;	// check whether this new root would violate a law
	}
	rootDirections->addDirection( x, y, direction );
	auto mySources = *getRootSources(x,y);
	int dx=0, dy=0;
	DirectionMap::calcDxDy(&dx,&dy, direction);
	auto otherSources = *getRootSources(x+dx,y+dy);
		
	for( auto it=mySources.begin(); it!=mySources.end(); it++ ) {			// update the roots starting from your sources
		std::cout << "spreading :"<< x <<","<< y <<"\n";
		//std::thread threadHere(&Source::spreadFrom, (*it).first, x,y);
		//threadHere.detach();
		(*it).first->spreadFrom(x,y);
	}
	for( auto it=otherSources.begin(); it!=otherSources.end(); it++ ) {		// update the roots starting from the other's sources
		std::cout << "spreading from other :"<< x+dx <<","<< y+dy <<"\n";
		//std::thread threadThere(&Source::spreadFrom, (*it).first, x+dx,y+dy);
		//threadThere.detach();
		(*it).first->spreadFrom(x+dx,y+dy);
	}
	rootMutex.unlock();
	std::cout << "rootMutex released\n";
}

void Engine::removeRootDirection(int x, int y, Direction direction) {
	std::thread rootDeletionThread(&Engine::removeRootDirectionThread, this, x,y,direction);
	rootDeletionThread.detach();
}

void Engine::removeRootDirectionThread(int x, int y, Direction direction) {
	rootMutex.lock();
	rootDirections->removeDirection( x, y, direction);
	int dx=0, dy=0;
	DirectionMap::calcDxDy(&dx,&dy, direction);
	// because you just severed a connection
	auto mySources = *getRootSources(x,y);
	for( auto it=mySources.begin(); it!=mySources.end(); it++ ) {	// update the roots starting from
		//std::thread threadHere (&Source::killFrom, (*it).first, x,y);
		//std::thread threadThere(&Source::killFrom, (*it).first, x+dx,y+dy);
		//threadHere.detach();
		//threadThere.detach();
		(*it).first->killFrom(x,y);									// this field
		(*it).first->killFrom(x+dx,y+dy);							// and the other
	}
	rootMutex.unlock();
}

void Engine::clearRootField(int x, int y) {
	for( int i=0; i<ROOT_DIRECTIONS; i++ ) {
		Direction direction = rootIndexToDirection(i);
		removeRootDirection(x, y, direction);
	}
}

Direction Engine::rootIndexToDirection(int i) const {
	return rootDirections->indexToDirection(i);
}

std::map<Source*,int>* Engine::getRootSources(int x, int y) {
	return &rootSources[x+y*WIDTH];
}

float Engine::getRootCharge(int x, int y) {
	float charge = 0;
	if( rootMutex.try_lock() ) {				// try to access the holy rootSources-system
		auto sources = getRootSources(x,y);
		for( auto it=sources->begin(); it!=sources->end(); it++ )	// add the charges of all sources
			charge += (*it).first->getCharge();
		rootMutex.unlock();
		rootCharges[x+y*WIDTH] = charge;				// set rootCharges, so you can re-read the value more easily later
	} else {										// if you can't
		charge = getLastRootCharge(x,y);			// just use the old charge
	}
	return charge;
}

float Engine::getLastRootCharge(int x, int y) const {
	return rootCharges[x+y*WIDTH];
}

void Engine::rootIterationBegin(int* x_ptr, int* y_ptr) {	// starts a search for connected roots at *x_ptr,*y_ptr
	std::fill(boolArray, boolArray+(WIDTH*HEIGHT), 0);
	boolArray[(*x_ptr)+(*y_ptr)*WIDTH] = true;
	indexArray[0] = (*x_ptr)+(*y_ptr)*WIDTH;
	currentIndex = 0;
	lastIndex = 0;
	{
		int i = 0;
		Direction currentDirection = rootDirections->indexToDirection(i);
		while( currentDirection != NEUTRAL ) {				// go through all directions
			int dx = 0, dy = 0;
			DirectionMap::calcDxDy( &dx,&dy, currentDirection );
			int index = (*x_ptr + dx) + (*y_ptr + dy)*WIDTH;
			if( !boolArray[index] && checkRootConnection( *x_ptr, *y_ptr, currentDirection) ) {	// if there is an unfound connection add the connected field to the list
				indexArray[++lastIndex] = index;
				boolArray[index] = true;
			}
			currentDirection = rootDirections->indexToDirection(++i);
		}
	}
	currentIndex++;
}

void Engine::rootIterationNext(int* x_ptr, int* y_ptr) {	// sets *x_ptr,*y_ptr to the next field in the search for connected roots
	// set the values
	if( currentIndex <= lastIndex ) {
		*x_ptr = indexArray[currentIndex] % WIDTH;
		*y_ptr = indexArray[currentIndex] / WIDTH;
	} else {
		*x_ptr = rootIterationEnd();
		*y_ptr = rootIterationEnd();
	}
	// search for more fields
	int x = indexArray[currentIndex] % WIDTH;
	int y = indexArray[currentIndex] / WIDTH;
	int i = 0;
	Direction currentDirection = rootDirections->indexToDirection(i);
	while( currentDirection != NEUTRAL ) {				// go through all directions
		int dx = 0, dy = 0;
		DirectionMap::calcDxDy( &dx,&dy, currentDirection );
		int index = (x + dx) + (y + dy)*WIDTH;
		if( !boolArray[index] && checkRootConnection( x, y, currentDirection) ) {	// if there is an unfound connection add the connected field to the list
			indexArray[++lastIndex] = index;
			boolArray[index] = true;
		}
		currentDirection = rootDirections->indexToDirection(++i);
	}
	currentIndex++;
}

int Engine::rootIterationEnd() const {							// returns -1
	return -1;
}

Direction Engine::findRootNeighbour(int x, int y) const {
	int i = 0;
	Direction currentDirection = engine.rootIndexToDirection(i);
	while( currentDirection != NEUTRAL ) {						// go through all directions
		if( engine.checkRootDirection( x, y, currentDirection) ) {		// if you find a neighbour return the direction where your found him
			return currentDirection;
		}
		currentDirection = engine.rootIndexToDirection(++i);
	}
	return NEUTRAL;
}

bool 	Engine::rootAllowed(int x, int y, Direction direction) {
	int dx=0, dy=0;
	DirectionMap::calcDxDy(&dx,&dy, direction);
	if( checkRootDirection( x+dx, y+dy, DirectionMap::opposingDirection(direction) ) ) {	// if this root would connect to a neighbour field
		// check whether these two fields have the same sources, if true they are already connected and the new root would form a circle (forbidden)
		lockRootSource(x,y);
		lockRootSource(x+dx,y+dy);
		bool allowed = !key_compare( getRootSources(x,y), getRootSources(x+dx,y+dy) );	// if they're different everything is fine
		unlockRootSource(x,y);
		unlockRootSource(x+dx,y+dy);
		return allowed;
	}
	return true;
}

void 	Engine::lockRootSource(int x, int y) {
	rootSourceMutex[x+y*WIDTH].lock();
}

void 	Engine::unlockRootSource(int x, int y) {
	rootSourceMutex[x+y*WIDTH].unlock();
}