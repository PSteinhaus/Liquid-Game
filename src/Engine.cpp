#include "main.hpp"
#include <stdlib.h>     /* rand */
#include <limits>

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
	if( perSecond(20) ) {
		// let the liquids flow
		colorMap->update();
	}
}

void Engine::render() {
	TCODConsole::root->clear();

	globalHeightMap->render();	// render the world height
	colorMap->render();	// render the fluids
	for( auto it=structures.begin(); it!=structures.end(); ++it )	// render the structures
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
