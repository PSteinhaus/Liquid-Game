// Dependency-management-model: "one to rule them all"

enum Direction : char {
	NEUTRAL, DOWN_LEFT, DOWN, DOWN_RIGHT, RIGHT, UP_RIGHT, UP, UP_LEFT, LEFT
};

class ColorMap;
class HeightMap;
class FourDirectionMap;
class DirectionMap;
class Player;
class Source;
class Creature;
#include <vector>
#include <utility>
#include <map>
#include <mutex>
//#include <shared_mutex>
#include "libtcod.hpp"
#include <chrono>
#include "XInput.hpp"
#include "Structure.hpp"
#include "Engine.hpp"
#include "HeightMap.hpp"
#include "MultiDirectionMap.hpp"
#include "Brush.hpp"
#include "Player.hpp"
#include "Source.hpp"
#include "Creature.hpp"