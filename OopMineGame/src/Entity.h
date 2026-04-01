#pragma once

#include <utility>

#include <olcPixelGameEngine.h>

#include "ItemStack.h"

class World;
class OopMineGame;

class Entity
{
public:
	struct Input
	{
		bool left = false;
		bool right = false;
		bool jump = false;
		bool sprint = false;
		bool attack = false;
		bool use = false;
		olc::vf2d target = {};
	};

	Entity(olc::vf2d pos, olc::vf2d size, int maxInvSize = 9);
	virtual ~Entity() = default;

	int getId() const;
	olc::vf2d getPos() const;
	olc::vf2d getSize() const;
	float getX() const;
	float getY() const;
	olc::vf2d getVel() const;
	Input getInput() const;

	Entity* setPos(olc::vf2d v);
	Entity* setX(float v);
	Entity* setY(float v);
	Entity* setVel(olc::vf2d v);
	Entity* setInput(Input v);

	std::pair<olc::vf2d, olc::vf2d> getBb() const;
	std::pair<olc::vf2d, olc::vf2d> getBbAt(olc::vf2d p) const;
	olc::vf2d getEyePos() const;
	bool getDirection() const;
	bool isDead() const;
	void kill();
	bool isOnGround() const;

	ItemStack getInvItem(int i);
	void setInvItem(int i, const ItemStack& v);
	ItemStack addInvItem(ItemStack v); // Returns the remaining

	void updateInput(World& world, float elapsed);
	virtual void update(World& world, float elapsed);
	virtual void draw(OopMineGame& game) const;

private:
	static int entityIdCounter;
	const int id;
	olc::vf2d pos = {}; // Origin is bottom middle
	olc::vf2d size = { 1,1 };
	olc::vf2d vel = {};
	bool direction = true; // Direction of last horizontal velocity, true = right, false = left
	bool dead = false;
	bool onGround = true;
	std::vector<ItemStack> inv = {};
	const int maxInvSize;
	Input input = {};
	std::chrono::steady_clock::time_point lastJumpTime = {};

	void handleCollisions(World& world, float elapsed);
	void checkOnGround(World& world);
};
