#include "main.hpp"

Player::Player(TCOD_color_t col, int cId) : heightMap(0), directionMap(), gamepad(cId,0.1f,0.07f), cursorX(1), cursorY(1), col(col) {
}

void Player::update() {
	if( gamepad.Refresh() ) {
		// Actions
		action = NONE;
		if (gamepad.leftTrigger)		action = SHOW_HEIGHTMAP;
		else if (gamepad.rightTrigger)	action = SHOW_DIRECTIONMAP;
		// cursor movement
		cursorX += gamepad.leftStickX/2;
		cursorY -= gamepad.leftStickY/2;
		if( cursorX < 1 ) cursorX = 1;
		else if( cursorX >= Engine::WIDTH-1 ) cursorX = Engine::WIDTH-2;
		if( cursorY < 1 ) cursorY = 1;
		else if( cursorY >= Engine::HEIGHT-1 ) cursorY = Engine::HEIGHT-2;
		// selection radius
		if( action != SHOW_DIRECTIONMAP )
			changeBrushSize( gamepad.rightStickX/2 );
		// Shoulder buttons control brush height
		if( gamepad.IsPressed(XINPUT_GAMEPAD_RIGHT_SHOULDER) ) {
			if( gamepad.IsPressed(XINPUT_GAMEPAD_RIGHT_SHOULDER,0.15) || wasPressedRB == false )
				brush.changeHeight(1);
			wasPressedRB = true;
		} else wasPressedRB = false;
		if( gamepad.IsPressed(XINPUT_GAMEPAD_LEFT_SHOULDER) ) {
			if( gamepad.IsPressed(XINPUT_GAMEPAD_LEFT_SHOULDER,0.15) || wasPressedLB == false )
				brush.changeHeight(-1);
			wasPressedLB = true;
		} else wasPressedLB = false;
		
		if (action == SHOW_DIRECTIONMAP) {
			char dx=0;
			char dy=0;
			if(gamepad.rightStickX > 0.1)		dx = 1;
			else if(gamepad.rightStickX < -0.1) dx = -1;
			if(gamepad.rightStickY > 0.1)		dy = 1;
			else if(gamepad.rightStickY < -0.1) dy = -1;
			
			if( gamepad.IsPressed(XINPUT_GAMEPAD_B) )	useDirectionBrush(0,0);
			else if( dx != 0 || dy != 0 )				useDirectionBrush(dx,dy);
		}
		else {
			decreaseCursorOpacity = true;
			if (gamepad.IsPressed(XINPUT_GAMEPAD_A))		useDigBrush();
			else if (gamepad.IsPressed(XINPUT_GAMEPAD_B))	useFillBrush();
			else decreaseCursorOpacity = false;
		}

	}
}

void Player::render() const {
	//heightMap.render();
	// render cursor
	//TCOD_console_set_char_background( NULL , cursorX, cursorY, TCOD_color_multiply_scalar( {255,255,255}, 0.3 ), TCOD_BKGND_ADD);
	if( action == SHOW_HEIGHTMAP )
		brush.renderHeightMap( (int)cursorX, (int)cursorY, &heightMap, this );
	else if( action == SHOW_DIRECTIONMAP )
		brush.renderDirectionMap( (int)cursorX, (int)cursorY, &directionMap, this );
	else
		brush.render( (int)cursorX, (int)cursorY, decreaseCursorOpacity );
	//DEBUG
	if(this==engine.players[0])TCOD_console_put_char( NULL, Engine::WIDTH-1, 0, engine.getHeight((int)cursorX, (int)cursorY)+48 , TCOD_BKGND_SET);
}

char Player::getHeight(int x, int y) const {
	return heightMap.getHeight(x,y);
}

void Player::setHeight(int x, int y, char height) {
	heightMap.setHeight(x,y,height);
}

void Player::setDirection(int x, int y, DirectionMap::Direction direction) {
	directionMap.setDirection(x,y, direction);
}

DirectionMap::Direction Player::getDirection(int x, int y) const {
	return directionMap.getDirection(x,y);
}


// brush stuff
void Player::changeBrushSize(float amount) {
	brush.changeSize(amount);
}

void Player::changeBrushHeight(char amount) {
	brush.changeHeight(amount);
}

void Player::useDigBrush() {
	brush.digAt((int)cursorX,(int)cursorY, this);
}

void Player::useFillBrush() {
	brush.fillAt((int)cursorX,(int)cursorY, this);
}

void Player::useDirectionBrush(char dx, char dy) {
	brush.setDirection((int)cursorX,(int)cursorY, this, DirectionMap::calcDirection(dx,dy) );
}
