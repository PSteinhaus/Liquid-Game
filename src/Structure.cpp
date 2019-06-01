#include "main.hpp"
#include <cmath>

Structure::Structure(int x, int y, Player* owner) : x(x), y(y), owner(owner) {}

Spawner::Spawner(int x, int y, Player* owner, float production) : Structure(x,y,owner), production(production), producedAmount(0) {}

void Spawner::update() {
	// if there is someone new at your position, make him the new owner
	Player* player = engine.getPlayer(x,y);
	if( player!=NULL && player!=owner )
		owner = player;
	if(owner) {
		// produce fluid
		producedAmount += production;
		int output = (int)producedAmount;
		if( output >= ROLLOUT_SIZE ) {
			producedAmount -= output;
			engine.setPlayer(x,y,owner);
			engine.pumpAt(x,y,output,owner);
		}
	}
}

void Spawner::render() const {
	if( owner ) {
		TCOD_console_set_char( NULL, x, y, engine.turn%(int)Engine::FPS >= Engine::FPS/2 ? '+' : 'x');
		TCOD_console_set_char_foreground( NULL , x, y, owner->col );
	}
	else {
		TCOD_console_set_char( NULL, x, y, '+' );
		TCOD_console_set_char_foreground( NULL , x, y, TCOD_color_t{127,127,127} );
	}
}