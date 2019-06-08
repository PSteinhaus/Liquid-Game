class MultiDirectionMap
{
public:
	virtual bool checkDirection(int x, int y, Direction direction) = 0;
	virtual void addDirection(int x, int y, Direction direction) = 0;
	virtual void removeDirection(int x, int y, Direction direction) = 0;
	virtual void clear() = 0;
	virtual void render() = 0;
};

class FourDirectionMap : MultiDirectionMap
{
private:
	bool directions [Engine::WIDTH*Engine::HEIGHT][4];
	const int getTilePosition(int x, int y);
public:
	FourDirectionMap();
	virtual ~FourDirectionMap() = default;
	static const int directionToIndex(Direction direction);
	static const Direction indexToDirection(int index);
	bool checkDirection(int x, int y, Direction direction);
	void addDirection(int x, int y, Direction direction);
	void removeDirection(int x, int y, Direction direction);
	void clearField(int x, int y);
	void clear();
	void render();
};

class EightDirectionMap : MultiDirectionMap
{
private:
	bool directions [Engine::WIDTH*Engine::HEIGHT][8];
public:
	EightDirectionMap();
	static const int directionToIndex(Direction direction);
	bool checkDirection(int x, int y, Direction direction);
	void addDirection(int x, int y, Direction direction);
	void removeDirection(int x, int y, Direction direction);
	void clear();
	//void clearField();
	//void render();
}; 