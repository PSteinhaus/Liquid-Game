#include "main.hpp"
#include <stdlib.h>

Creature::Creature(int x, int y) : x(x), y(y) {}

Tree::Tree(int x, int y) : Creature(x,y) {
	source = new Source(x,y);
}

Tree::~Tree() {
	delete source;
}

void Tree::growRoot() {
	// find the edges of your root-network and add a root at a random edge-point
	static std::vector<unsigned int> edgeIndexes;
	static std::vector<unsigned int> outerEdgeIndexes;
	edgeIndexes.clear();
	outerEdgeIndexes.clear();
	int xPos=x, yPos=y;
	// find the edges
	for( engine.rootIterationBegin(&xPos,&yPos); xPos!=engine.rootIterationEnd(); engine.rootIterationNext(&xPos,&yPos) ) {
		if( *source->myDistanceAt(xPos,yPos) > maxRootLength ) break;	// if you're starting to look outside of your reach stop
		int i = 0;
		bool fieldAdded = false;
		bool fieldAddedOuter = false;
		Direction currentDirection = engine.rootIndexToDirection(i);
		while( currentDirection != NEUTRAL && !(fieldAdded && fieldAddedOuter) ) {							// go through all directions
			Direction opposingDirection = DirectionMap::opposingDirection(currentDirection);
			int dx=0, dy=0;
			DirectionMap::calcDxDy(&dx,&dy, currentDirection);
			// check whether the fluid concentration lures you into that direction
			if( engine.getHeight(xPos,yPos)>=fluidNeededForGrowth && engine.getHeight(xPos+dx,yPos+dy)>=fluidNeededForAttraction )
			{
				if( !engine.checkRootDirection( xPos, yPos, currentDirection) ) {	
					if( !fieldAdded && engine.rootAllowed(xPos,yPos,currentDirection) )		// if this field is missing an allowed root add it to the list
					{
						edgeIndexes.push_back( xPos + yPos*Engine::WIDTH );
						fieldAdded = true;
					}
				} else {
					// if you already have a root in that direction search for a free spot facing you in your neighbours
					if( !engine.checkRootDirection( xPos+dx, yPos+dy, opposingDirection )
						&& engine.rootAllowed(xPos+dx, yPos+dy, opposingDirection) )
					{
						outerEdgeIndexes.push_back( xPos + yPos*Engine::WIDTH );
						fieldAddedOuter = true;
					}
				}
			}
			currentDirection = engine.rootIndexToDirection(++i);
		}
	}
	// grow the root from a random position
	int edgeCount = edgeIndexes.size();
	int outerEdgeCount = outerEdgeIndexes.size();
	if( edgeCount == 0 && outerEdgeCount == 0 ) return;
	unsigned int chosenIndex;
	int chosenX;
	int chosenY;
	if( rand() % (edgeCount+outerEdgeCount) < edgeCount ) {		// decide randomly whether to pick an edge-field or an outer-edge-field
		chosenIndex = edgeIndexes[rand() % edgeCount]; // choose a random edge-field
		chosenX = chosenIndex % Engine::WIDTH;
		chosenY = chosenIndex / Engine::WIDTH;
		while( true ) {
			Direction randomDirection = engine.rootIndexToDirection( rand() % engine.ROOT_DIRECTIONS );	// choose a random direction
			int dx=0, dy=0;
			DirectionMap::calcDxDy(&dx,&dy, randomDirection);
			if( engine.getHeight(chosenX,chosenY)>=fluidNeededForGrowth && engine.getHeight(chosenX+dx,chosenY+dy)>=fluidNeededForAttraction )	// check for attractive fluid
				if( !engine.checkRootDirection( chosenX, chosenY, randomDirection )
					&& engine.rootAllowed(chosenX, chosenY, randomDirection) )							// check for a missing root
				{ 	
					engine.addRootDirection( chosenX, chosenY, randomDirection );		// add it if you find it missing
					engine.changeHeight(chosenX, chosenY, -fluidNeededForGrowth);		// decrease the fluid level as a cost for the player
					edgeCount--;
					rootAmount++;
					break;
				}
		}
	} else {
		chosenIndex = outerEdgeIndexes[rand() % outerEdgeCount]; // choose a random outer-edge-field
		chosenX = chosenIndex % Engine::WIDTH;
		chosenY = chosenIndex / Engine::WIDTH;
		while( true ) {
			Direction randomDirection = engine.rootIndexToDirection( rand() % engine.ROOT_DIRECTIONS );	// choose a random direction
			int dx=0, dy=0;
			DirectionMap::calcDxDy(&dx,&dy, randomDirection);
			Direction opposingDirection = DirectionMap::opposingDirection(randomDirection);
			// check for an existing root on your side and a missing root on the other
			if( engine.getHeight(chosenX,chosenY)>=fluidNeededForGrowth && engine.getHeight(chosenX+dx,chosenY+dy)>=fluidNeededForAttraction )	// check for attractive fluid
				if( engine.checkRootDirection( chosenX, chosenY, randomDirection ) && !engine.checkRootDirection( chosenX+dx, chosenY+dy, opposingDirection )
					&& engine.rootAllowed(chosenX+dx, chosenY+dy, opposingDirection) )
				{  
					engine.addRootDirection( chosenX+dx, chosenY+dy, opposingDirection );	// add it if you find it missing
					engine.changeHeight(chosenX+dx, chosenY+dy, -fluidNeededForGrowth);		// decrease the fluid level as a cost for the player
					outerEdgeCount--;
					rootAmount++;
					break;
				}
		}
	}
}

SolarTree::SolarTree(int x, int y, float chargeProduction) : Tree(x,y), chargeProduction(chargeProduction) {}

SolarTree::~SolarTree() {}

void SolarTree::update() {
	source->increaseCharge( chargeProduction / Engine::CREATURE_UPDATES_PER_SECOND );
	if( rootAmount<maxRootAmount && rand() % (int)(Engine::CREATURE_UPDATES_PER_SECOND) != 0 ) growRoot();
}

void SolarTree::render() const {
	TCOD_console_set_char( NULL, x, y, 'o' );
	//TCOD_console_set_char_foreground( NULL , x, y, {255,255,255} );
}