class Source {
private:
	int x,y;
	float charge;
	static unsigned int lastIndex;									// the last index to check within indexArray
	static unsigned int indexArray[Engine::WIDTH*Engine::HEIGHT];	// holds the indices of fields to be checked 
	static bool boolArray [Engine::WIDTH*Engine::HEIGHT];			// tells whether a field has been checked already
	void spread();
public:
	Source(int x, int y);
	void increaseCharge(float amount);
	void decreaseCharge(float amount);
	void resetCharge();
	float getCharge() const;
	void setToPosition(int* x_ptr, int* y_ptr);
	void spreadFrom(int x, int y);
	void killFrom(int x, int y);
	void deleteMyPairAt(int x, int y);
	void addMyPairTo(int x, int y, int distance);
	int* myDistanceAt(int x, int y);
};