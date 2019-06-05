class Creature
{
private:
	int x,y;
public:
	virtual void update() = 0;
	virtual void render() const = 0;
};

class Tree : Creature
{
private:
	static constexpr int STD_MAX_ROOT_LENGTH = 14;
	static constexpr int STD_MAX_ROOT_AMOUNT = 14*14;
	int maxRootLength = STD_MAX_ROOT_LENGTH;
	int maxRootAmount = STD_MAX_ROOT_AMOUNT;
	void growRoots();
public:
	virtual void update() = 0;
	virtual void render() const = 0;
};

class Seed : Creature
{

};