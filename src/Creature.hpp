class Creature
{
protected:
	int x,y;
public:
	Creature(int x, int y);
	virtual ~Creature() = default;
	virtual void update() = 0;
	virtual void render() const = 0;
};

class Tree : public Creature
{
protected:
	static constexpr int STD_MAX_ROOT_LENGTH = 14;
	static constexpr int STD_MAX_ROOT_AMOUNT = 14*14;
	static constexpr char STD_FLUID_NEEDED_FOR_GROWTH = 3;
	int maxRootLength = STD_MAX_ROOT_LENGTH;
	int maxRootAmount = STD_MAX_ROOT_AMOUNT;
	char fluidNeededForGrowth = STD_FLUID_NEEDED_FOR_GROWTH;
	Source* source;
	void growRoot();
public:
	Tree(int x, int y);
	virtual ~Tree();
	virtual void update() = 0;
	virtual void render() const = 0;
};

class SolarTree : public Tree
{
private:
	static constexpr float STD_CHARGE_PRODUCTION = 0.1;
	float chargeProduction = STD_CHARGE_PRODUCTION;
public:
	SolarTree(int x, int y, float chargeProduction=STD_CHARGE_PRODUCTION);
	virtual ~SolarTree();
	void update();
	void render() const;
};

class Seed : Creature
{

};