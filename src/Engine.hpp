class Engine
{
public:
	static const int  WIDTH  = 1920/16;
	static const int  HEIGHT = 1080/16;
	static const char DEPTH  = 5;
	static constexpr float FPS	= 60.0;
	static constexpr float CREATURE_UPDATES_PER_SECOND = 20;
	static constexpr int ROOT_DIRECTIONS = 4;
private:
	ColorMap* colorMap;
	HeightMap* globalHeightMap;
	// roots
	FourDirectionMap* rootDirections;
	std::map<Source*,int> rootSources [WIDTH*HEIGHT];
	float rootCharges [WIDTH*HEIGHT];
	std::vector<Creature*> creatures;
	// stuff for the pump/pathfinding-algorithm
	unsigned int currentIndex;									// the current index to check within indexArray
	unsigned int lastIndex;										// the last index to check within indexArray
	unsigned int indexArray [Engine::WIDTH*Engine::HEIGHT];		// holds the indices of fields to be checked
	bool boolArray [Engine::WIDTH*Engine::HEIGHT];				// tells whether a field has been checked already
public :
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
	void 	addCreature(Creature* creature);
	void	removeCreature(Creature* creature);
	void 	addStructure(Structure* structure);
	void	removeStructure(Structure* structure);
	void	clearLevel();
	void	loadBitmap(const char* filename);
	// roots
	bool 	checkRootConnection(int x, int y, Direction direction) const;
	bool 	checkRootDirection(int x, int y, Direction direction);
	void 	addRootDirection(int x, int y, Direction direction);
	void 	removeRootDirection(int x, int y, Direction direction);
	void 	clearRootField(int x, int y);
	Direction rootIndexToDirection(int i) const;
	std::map<Source*,int>* getRootSources(int x, int y);
	float 	getRootCharge(int x, int y);
	float 	getLastRootCharge(int x, int y) const;
	void 	rootIterationBegin(int* x_ptr, int* y_ptr);	// starts a search for connected roots at *x_ptr,*y_ptr
	void	rootIterationNext(int* x_ptr, int* y_ptr);	// sets *x_ptr,*y_ptr to the next field in the search for connected roots
	int 	rootIterationEnd() const;					// returns -1
	Direction findRootNeighbour(int x, int y) const;
	bool 	rootAllowed(int x, int y, Direction direction); // whether a root starting from x,y in the specified direction would not create a circle (circles are forbidden)
};

extern Engine engine;