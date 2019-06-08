#include "main.hpp"
#include <iostream>

unsigned int Source::lastIndex = 0;											// the last index to check within indexArray
unsigned int Source::indexArray[Engine::WIDTH*Engine::HEIGHT] = { 0 };		// holds the indices of fields to be checked 
bool Source::boolArray [Engine::WIDTH*Engine::HEIGHT] = {};				// tells whether a field has been checked already

Source::Source(int x, int y) : x(x), y(y), charge(0) {
	// add yourself to the list of sources on your field
	addMyPairTo(x,y,0);
	// create roots into all directions
	for( int i=0; i<engine.ROOT_DIRECTIONS; i++ ) {
		Direction direction = engine.rootIndexToDirection(i);
		engine.addRootDirection(x,y,direction);
	}
}

void Source::increaseCharge(float amount) {
	charge += amount;
}

void Source::decreaseCharge(float amount) {
	charge -= amount;
	if( charge < 0 ) charge = 0;
}

void Source::resetCharge() {
	charge = 0;
}

float Source::getCharge() const {
	return charge;
}

void Source::setToPosition(int* x_ptr, int* y_ptr) {
	*x_ptr = x;
	*y_ptr = y;
}

void Source::spreadFrom(int x, int y) {
	// find all fields with roots that you are connected to and add yourself to the sources listed there
	// while doing so, document the distance of the field to you and only spread further into fields that are further away
	std::fill(boolArray, boolArray+(Engine::WIDTH*Engine::HEIGHT), 0);
	boolArray[x+y*Engine::WIDTH] = true;
	indexArray[0] = x+y*Engine::WIDTH;
	lastIndex = 0;
	spread();
}

void Source::killFrom(int xStart, int yStart) {
	// find all fields with roots that you are connected to and delete yourself from the sources listed there
	// while doing so, only spread further into fields that are further away
	std::fill(boolArray, boolArray+(Engine::WIDTH*Engine::HEIGHT), 0);
	boolArray[xStart+yStart*Engine::WIDTH] = true;
	indexArray[0] = xStart+yStart*Engine::WIDTH;
	lastIndex = 0;
	for( unsigned int currentIndex=0; currentIndex <= lastIndex; currentIndex++ ) {
		// find your pair<Source*,int>
		int x = indexArray[currentIndex] % Engine::WIDTH;
		int y = indexArray[currentIndex] / Engine::WIDTH;
		int* myDistance = myDistanceAt(x,y);
		bool deleteSelf = true;					// whether or not to delete this source from the current field
		if( myDistance==NULL ) 					// you're not there... so you can't spread from there
			continue;
		else {								// if you're there add all neighbours that are further away
			int distance = *myDistance;
			for( int i=0; i<engine.ROOT_DIRECTIONS; i++ ) {
				Direction direction = engine.rootIndexToDirection(i);	// check all directions
				int dx=0,dy=0;
				DirectionMap::calcDxDy(&dx,&dy,direction);
				int index = (x+dx)+(y+dy)*Engine::WIDTH;
				if( !boolArray[index] && engine.checkRootConnection(x,y,direction) ) {		// if there is a connection to that field
					boolArray[index] = true;
					int* myDistanceThere = myDistanceAt(x+dx,y+dy);
					if( myDistanceThere ) {									// check if you have a pair there
						if( *myDistanceThere > distance ) {				// if the distance increases
							indexArray[++lastIndex] = index;	// add the field to the killList
						} else {										// if the distance doesn't increase in that direction you've just found an edge
							deleteSelf = false;							// then you yourself are still connected, so remember to not kill yourself please
						}
					}
				}
			}
		}
		if( deleteSelf )
			deleteMyPairAt(x,y);				// delete your pair there
	}

	// make sure you didn't delete the true source by accident
	if( xStart == this->x && xStart == this->x )
		addMyPairTo(xStart,yStart,0);
}

void Source::spread() {
	// find all fields with roots that you are connected to and add yourself to the sources listed there
	// while doing so, document the distance of the field to you and only spread further into fields that are further away

	for( unsigned int currentIndex=0; currentIndex <= lastIndex; currentIndex++ ) {
		// find your pair<Source*,int>
		int x0 = indexArray[currentIndex] % Engine::WIDTH;
		int y0 = indexArray[currentIndex] / Engine::WIDTH;
		std::cout << "checking distance at :"<< x0 <<","<< y0 <<"\n";
		int* myDistance = myDistanceAt(x0,y0);
		if( myDistance==NULL ) 				// you're not there... so you can't spread from there
			continue;
		else {								// if you're there add all neighbours that are further away
			std::cout << "really spreading\n";
			int distance = *myDistance;
			for( int i=0; i<engine.ROOT_DIRECTIONS; i++ ) {
				Direction direction = engine.rootIndexToDirection(i);	// check all directions
				int dx=0,dy=0;
				DirectionMap::calcDxDy(&dx,&dy,direction);
				int index = (x0+dx)+(y0+dy)*Engine::WIDTH;
				if( !boolArray[index] && engine.checkRootConnection(x0,y0,direction) ) {	// if there is a connection to that field
					int* myDistanceThere = myDistanceAt(x0+dx,y0+dy);
					if( myDistanceThere ) {							// check if you already have a pair there
						if( *myDistanceThere > distance ) {			// if the distance increases
							*myDistanceThere = distance+1;			// make sure the distance isn't too high
							// add the field to the list
							indexArray[++lastIndex] = index;
							boolArray[index] = true;
						} else						// will only be interesting for killFrom
							continue;
					} else {											// if you don't have a pair there
						addMyPairTo(x0+dx,y0+dy,distance+1);			// add a pair of yours
						// add the field to the list
						indexArray[++lastIndex] = index;
						boolArray[index] = true;
					}
				}
			}
		}
	}
}

void Source::deleteMyPairAt(int x, int y) {
	engine.getRootSources(x,y)->erase(this);
}

void Source::addMyPairTo(int x, int y, int distance) {
	engine.getRootSources(x,y)->insert({this,distance});
}

int* Source::myDistanceAt(int x0, int y0) {
	try {
		return &(engine.getRootSources(x0,y0)->at(this));
	} catch (std::out_of_range const & ex) {
		std::cout << "no distance\n";
		return NULL;
	}
}