class HeightMap
{
protected:
	char heights [Engine::WIDTH*Engine::HEIGHT];
public:
	HeightMap(char initialHeight = Engine::DEPTH);
	void render() const;
	void setHeight(int x, int y, char height);
	void changeHeight(int x, int y, char change);
	char changeHeightConserving(int x, int y, char change);
	void moveHeight(int x0, int y0, int x_dest, int y_dest, char amount=1);
	void moveHeightConserving(int x0, int y0, int x_dest, int y_dest, char amount);
	char getHeight(int x, int y) const;
	void clear();
};

class ColorMap : public HeightMap
{
private:
	Player* players[Engine::WIDTH*Engine::HEIGHT];
	// stuff for the pump-algorithm
	unsigned int lastIndex;										// the last index to check within indexArray
	unsigned int indexArray [Engine::WIDTH*Engine::HEIGHT];		// holds the indices of fields to be checked
	bool boolArray [Engine::WIDTH*Engine::HEIGHT];				// tells whether a field has been checked already
	bool pumpFromIndex(unsigned int i, char& amount, Player* pumpingPlayer);
public:
	ColorMap();
	void  setPlayer(int x, int y, Player* player);
	Player* getPlayer(int x, int y) const;
	void render() const;
	void update();
	void moveColor(int x0, int y0, int x_dest, int y_dest, char amount=1);
	void moveColorConserving(int x0, int y0, int x_dest, int y_dest, char amount);
	char pumpAt(int x, int y, char amount, Player* pumpingPlayer);
	void clear();
};

class DirectionMap : public HeightMap
{
public:
	DirectionMap() : HeightMap(0) {}
	void setDirection(int x, int y, Direction direction);
	Direction getDirection(int x, int y) const;
	static const Direction calcDirection(char dx, char dy);
	static const Direction opposingDirection(Direction direction);
	static const Direction calcDirectionInverted(char dx, char dy);
	static void calcDxDy(int* dx, int* dy, Direction direction);	// set *dx and *dy to the correct values based on the given direction (WARNING: y is inverted)
	static const int getTilePosition(Direction direction);
};