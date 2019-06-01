class Structure
{
protected:
	int x,y;
	Player* owner;
public:
	Structure(int x, int y, Player* owner);
	virtual ~Structure() = default;
	virtual void update() = 0;
	virtual void render() const = 0;
};

class Spawner : public Structure
{
private:
	float production;		// of fluid per second
	float producedAmount;	// ready to be given out
	static constexpr float	STANDARD_PRODUCTION = 0.2;
	static constexpr int 	ROLLOUT_SIZE = 4;
public:
	Spawner(int x, int y, Player* owner, float production=STANDARD_PRODUCTION);
	void update();
	void render() const;
};