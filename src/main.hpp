// Dependency-management-model: "one to rule them all"

enum Direction : char {
	NEUTRAL, DOWN_LEFT, DOWN, DOWN_RIGHT, RIGHT, UP_RIGHT, UP, UP_LEFT, LEFT
};

class ColorMap;
class HeightMap;
class FourDirectionMap;
class DirectionMap;
class Player;
#include <vector>
#include "libtcod.hpp"
#include <chrono>
#include "XInput.hpp"
#include "Structure.hpp"
#include "Engine.hpp"
#include "HeightMap.hpp"
#include "MultiDirectionMap.hpp"
#include "Brush.hpp"
#include "Player.hpp"