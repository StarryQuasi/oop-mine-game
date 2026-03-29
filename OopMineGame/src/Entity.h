#ifndef ENTITY_H
#define ENTITY_H

#include <utility>

#include <olcPixelGameEngine.h>

#include "ItemStack.h"

class World;
class OopMineGame;

class Entity
{
public:
	Entity(olc::vf2d pos, olc::vf2d size, int maxInvSize = 9);
	virtual ~Entity() = default;

	int getId() const;
	olc::vf2d getPos() const;
	olc::vf2d getSize() const;
	float getX() const;
	float getY() const;
	olc::vf2d getVel() const;

	void setPos(olc::vf2d);
	void setX(float);
	void setY(float);
	void setVel(olc::vf2d);

	std::pair<olc::vf2d, olc::vf2d> getBb() const;
	std::pair<olc::vf2d, olc::vf2d> getBbAt(olc::vf2d) const;
	olc::vf2d getEyePos() const;
	bool getDirection() const;
	bool isDead() const;
	void kill();
	bool isOnGround() const;
	
	ItemStack getInvItem(int);
	void setInvItem(int, ItemStack);
	ItemStack addInvItem(ItemStack); // Returns the remaining

	virtual void update(World& world, float elapsed);
	virtual void draw(OopMineGame&) const;

private:
	static int entityIdCounter;
	const int id;
	olc::vf2d pos; // Origin is bottom middle
	olc::vf2d size;
	olc::vf2d vel;
	bool direction; // Direction of last horizontal velocity, true = right, false = left
	bool dead;
	bool onGround;
	std::vector<ItemStack> inv;
	const int maxInvSize;

	void handleCollisions(World& world, float elapsed);
	void checkOnGround(World& world);
};

#endif
