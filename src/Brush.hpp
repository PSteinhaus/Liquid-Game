class Brush
{
private:
	static constexpr float MAX_BRUSH_RADIUS = 10;	// must be an even number
	static constexpr int BRUSH_CENTER_INDEX = (int)MAX_BRUSH_RADIUS;

	float radius;
	char height;
	char brushArray[(int)MAX_BRUSH_RADIUS*2+1][(int)MAX_BRUSH_RADIUS*2+1];
	void calcBrush();
public:
	Brush();
	void changeSize(float amount);
	void changeHeight(char amount);
	void digAt(int xCenter, int yCenter, Player* player);
	void fillAt(int xCenter, int yCenter, Player* player);
	void setDirection(int xCenter, int yCenter, Player* player, Direction direction);
	void render( int xCenter, int yCenter, bool decreaseCursorOpacity ) const;
	void renderHeightMap( int xCenter, int yCenter, const HeightMap* heightMap, const Player* player ) const;
	void renderDirectionMap( int xCenter, int yCenter, const DirectionMap* directionMap, const Player* player ) const;
	double radiusCondition() const;
};