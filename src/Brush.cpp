#include "main.hpp"
#include <algorithm>
#include <cmath>

Brush::Brush() : radius(0), height(Engine::DEPTH) {
	calcBrush();
}

void Brush::changeSize(float amount) {
	radius += amount;
	if( radius > MAX_BRUSH_RADIUS ) radius = MAX_BRUSH_RADIUS;
	else if( radius < 0 ) radius = 0;
	calcBrush();
}

void Brush::changeHeight(char amount) {
	height += amount;
	if( height > Engine::DEPTH ) height = Engine::DEPTH;
	else if( height < 0 ) height = 0;
	calcBrush();
}

void Brush::calcBrush() {
	// delete the old brush
	std::fill(*brushArray, *brushArray+((int)MAX_BRUSH_RADIUS*2+1*(int)MAX_BRUSH_RADIUS*2+1), 0);
	// calculate the shape of the brush-head
	for( int x = -(int)radius; x <= (int)(radius); ++x )
		for( int y = -(int)radius; y <= (int)(radius); ++y )
		{
			double distance = sqrt( pow(x,2) + pow(y,2) );	// calc the distance to the center
			if( distance > radius ) continue;				// if it's larger than the allowed radius -> skip
															// else calc the height of the brush at this point
			if( radius != 0.0 )
				brushArray[x+BRUSH_CENTER_INDEX][y+BRUSH_CENTER_INDEX] = (char)((1.0 - distance/radius) * (float)height);
			else
				brushArray[x+BRUSH_CENTER_INDEX][y+BRUSH_CENTER_INDEX] = height;
		}
}

double Brush::radiusCondition() const {
	double condition;
	condition = radius-1;
	if( condition < 0 ) condition = 0;
	return condition;
}

void Brush::digAt(int xCenter, int yCenter, Player* player) {
	double condition = radiusCondition();
	// go through all fields inside the set radius
	for( int x = -(int)radius; x <= (int)(radius); ++x )
		for( int y = -(int)radius; y <= (int)(radius); ++y )
		{
			//if(brushArray[x+BRUSH_CENTER_INDEX][y+BRUSH_CENTER_INDEX]==0) continue;
			int xPos = xCenter+x;
			int yPos = yCenter+y;
			if( engine.inMap(xPos,yPos) == false ) continue;// if the field isn't on the board -> skip
			double distance = sqrt( pow(x,2) + pow(y,2) );	// calc the distance to the center
			if( distance > condition ) continue;				// if it's larger than the allowed radius -> skip
															// else lower the height of the player's heightmap according to the brush
			char heightHere = Engine::DEPTH - height ;
			if( player->getHeight(xCenter+x, yCenter+y) > heightHere )		// but only if it's higher than it should be
				player->setHeight( xCenter+x, yCenter+y, heightHere );
		}
}

void Brush::fillAt(int xCenter, int yCenter, Player* player) {
	double condition = radiusCondition();
	// go through all fields inside the set radius
	for( int x = -(int)radius; x <= (int)(radius); ++x )
		for( int y = -(int)radius; y <= (int)(radius); ++y )
		{
			//if(brushArray[x+BRUSH_CENTER_INDEX][y+BRUSH_CENTER_INDEX]==0) continue;
			int xPos = xCenter+x;
			int yPos = yCenter+y;
			if( engine.inMap(xPos,yPos) == false ) continue;// if the field isn't on the board -> skip
			double distance = sqrt( pow(x,2) + pow(y,2) );	// calc the distance to the center
			if( distance > condition ) continue;				// if it's larger than the allowed radius -> skip
															// else raise the height of the player's heightmap according to the brush
			char heightHere = height;
			if( player->getHeight(xCenter+x, yCenter+y) < heightHere )		// but only if it's lower than it should be
				player->setHeight( xCenter+x, yCenter+y, heightHere );
		}
}

void Brush::setDirection(int xCenter, int yCenter, Player* player, Direction direction) {
	double condition = radiusCondition();
	// go through all fields inside the set radius
	for( int x = -(int)radius; x <= (int)(radius); ++x )
		for( int y = -(int)radius; y <= (int)(radius); ++y )
		{
			int xPos = xCenter+x;
			int yPos = yCenter+y;
			if( engine.inMap(xPos,yPos) == false ) continue;// if the field isn't on the board -> skip
			double distance = sqrt( pow(x,2) + pow(y,2) );	// calc the distance to the center
			if( distance > condition ) continue;				// if it's larger than the allowed radius -> skip
															// else set the direction on this field
			player->setDirection(xPos,yPos, direction);
			engine.addRootDirection(xPos,yPos, direction);
			if( direction == NEUTRAL )
				engine.clearRootField(xPos,yPos);
		}
}


void Brush::render( int xCenter, int yCenter, bool decreaseCursorOpacity ) const {
	TCOD_color_t color = (decreaseCursorOpacity ? TCOD_color_t{100,100,100} : TCOD_color_t{170,170,170});
	double condition = radiusCondition();
	// go through all fields inside the set radius
	for( int x = -(int)radius; x <= (int)(radius); ++x )
		for( int y = -(int)radius; y <= (int)(radius); ++y )
		{
			//if(brushArray[x+BRUSH_CENTER_INDEX][y+BRUSH_CENTER_INDEX]==0) continue;
			int xPos = xCenter+x;
			int yPos = yCenter+y;
			if( engine.inMap(xPos,yPos) == false ) continue;// if the field isn't on the board -> skip
			double distance = sqrt( pow(x,2) + pow(y,2) );	// calc the distance to the center
			if( distance > condition ) continue;				// if it's larger than the allowed radius -> skip
															// else draw the height
			TCOD_console_set_char_background( NULL , xPos, yPos, TCOD_color_multiply_scalar(
				color, (float)height/(float)Engine::DEPTH ), TCOD_BKGND_ADD);
		}
	TCOD_console_set_char( NULL, xCenter, yCenter, height+48 );
	TCOD_console_set_char_foreground( NULL , xCenter, yCenter, TCOD_color_multiply_scalar(color,1.7) );
}

void Brush::renderHeightMap( int xCenter, int yCenter, const HeightMap* heightMap, const Player* player ) const {
	TCOD_color_t white = TCOD_color_t{255,255,255};
	// go through all fields inside the set radius
	for( int x = -(int)MAX_BRUSH_RADIUS; x <= (int)(MAX_BRUSH_RADIUS); ++x )
		for( int y = -(int)MAX_BRUSH_RADIUS; y <= (int)(MAX_BRUSH_RADIUS); ++y )
		{
			//if(brushArray[x+BRUSH_CENTER_INDEX][y+BRUSH_CENTER_INDEX]==0) continue;
			int xPos = xCenter+x;
			int yPos = yCenter+y;
			if( engine.inMap(xPos,yPos) == false ) continue;	// if the field isn't on the board -> skip
			double distance = sqrt( pow(x,2) + pow(y,2) );		// calc the distance to the center
			if( distance > MAX_BRUSH_RADIUS-1 ) continue;		// if it's larger than the allowed radius -> skip
																// else draw the height
			TCOD_console_set_char_background( NULL , xPos, yPos, TCOD_color_lerp(
					TCOD_console_get_char_background(NULL,xPos,yPos),
					TCOD_color_multiply_scalar( white, ( (float)engine.totalGroundHeight(xPos,yPos,player) )/(float)Engine::DEPTH), 0.8),
				TCOD_BKGND_SET );
			if( distance <= radiusCondition() ) {			// also draw the brush radius with colored dots
				TCOD_console_set_char( NULL, xPos, yPos, '.' );
				TCOD_console_set_char_foreground( NULL , xPos, yPos, player->col );
			}
		}
	TCOD_console_set_char( NULL, xCenter, yCenter, height+48 );
	TCOD_console_set_char_foreground( NULL , xCenter, yCenter, player->col );
}

void Brush::renderDirectionMap( int xCenter, int yCenter, const DirectionMap* directionMap, const Player* player ) const {
	// go through all fields inside the set radius
	for( int x = -(int)MAX_BRUSH_RADIUS; x <= (int)(MAX_BRUSH_RADIUS); ++x )
		for( int y = -(int)MAX_BRUSH_RADIUS; y <= (int)(MAX_BRUSH_RADIUS); ++y )
		{
			//if(brushArray[x+BRUSH_CENTER_INDEX][y+BRUSH_CENTER_INDEX]==0) continue;
			int xPos = xCenter+x;
			int yPos = yCenter+y;
			if( engine.inMap(xPos,yPos) == false ) continue;	// if the field isn't on the board -> skip
			double distance = sqrt( pow(x,2) + pow(y,2) );		// calc the distance to the center
			if( distance > MAX_BRUSH_RADIUS-1 ) continue;		// if it's larger than the allowed radius -> skip
																// else draw the direction
			TCOD_console_set_char( NULL, xPos, yPos, DirectionMap::getTilePosition( directionMap->getDirection(xPos,yPos) ) );
			TCOD_console_set_char_foreground( NULL , xPos, yPos, player->col );
			TCOD_console_set_char_background( NULL , xPos, yPos, TCOD_color_multiply_scalar(
					TCOD_console_get_char_background(NULL,xPos,yPos), 0.5),
				TCOD_BKGND_SET );
			if( distance <= radiusCondition() ) {			// also draw the brush radius with colored fields
				TCOD_console_set_char_background( NULL , xPos, yPos, TCOD_color_multiply_scalar(player->col,0.3), TCOD_BKGND_ADD );
			}
		}
}
