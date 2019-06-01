class Player
{
private:
	HeightMap heightMap;
	DirectionMap directionMap;
	Gamepad gamepad;
	float cursorX;
	float cursorY;
	enum ActionType {
		NONE,
		SHOW_HEIGHTMAP,
		SHOW_DIRECTIONMAP
	} action;
	bool wasPressedRB;	// whether the shoulder-buttons have already been pressed in the last frame
	bool wasPressedLB;
	// brush stuff
	Brush brush;
	friend void Brush::renderHeightMap(int,int,const HeightMap*,const Player*) const;
	friend void Brush::renderDirectionMap(int,int,const DirectionMap*,const Player*) const;
	bool decreaseCursorOpacity;	// to decrease the brush opacity during an action
	void changeBrushSize(float amount);
	void changeBrushHeight(char amount);
	void useDigBrush();
	void useFillBrush();
	void useDirectionBrush(char dx, char dy);
public:
	TCOD_color_t col;
	Player(TCOD_color_t col, int cId);
	void update();
	void render() const;
	// HeightMap
	void setHeight(int x, int y, char height);
	char getHeight(int x, int y) const;
	// DirectionMap
	void setDirection(int x, int y, DirectionMap::Direction direction);
	DirectionMap::Direction getDirection(int x, int y) const;
};