class Engine
{
private:
	ColorMap* colorMap;
	HeightMap* globalHeightMap;
public :
	static const int  WIDTH  = 1920/16;
	static const int  HEIGHT = 1080/16;
	static const char DEPTH  = 5;
	static constexpr float FPS	= 60.0;

	Player* players [4];
	unsigned long turn;
	TCOD_key_t lastKey;
	enum GameStatus {
		RUNNING,
		EXIT
	} gameStatus;
	std::vector<Structure*> structures;

	Engine();
	~Engine();
	void update();
	void render();
	Player* getPlayer(int x, int y) const;
	char	getHeight(int x, int y) const;
	char	getGlobalHeight(int x, int y) const;
	char	totalGroundHeight(int x, int y, const Player* player) const;
	char	totalHeight(int x, int y, Player* player) const;
	void	setField (int x, int y, Player* player, char height);
	void	setPlayer(int x, int y, Player* player);
	void	setHeight(int x, int y, char height);
	void	setGlobalHeight(int x, int y, char height);
	void	moveColor(int x0, int y0, int x_dest, int y_dest, char amount=1);
	void	changeHeight(int x, int y, char change);
	char	pumpAt(int x, int y, char amount, Player* pumpingPlayer);
	void	fightBetween(int x1, int y1, int x2, int y2);
	bool	inMap(int x, int y) const;
	bool	perSecond(float repetitions) const;		// returns true "repetitions" times per second
	void 	addStructure(Structure* structure);
	void	removeStructure(Structure* structure);
	void	clearLevel();
	void	loadBitmap(const char* filename);
};

extern Engine engine;