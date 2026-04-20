#pragma once

#include <vector>

#include "Item.h"

class Items
{
public:
	static const Item air;
	static const Item grassBlock;
	static const Item dirt;
	static const Item stone;
	static const Item cobblestone;
	static const Item coalOre;
	static const Item copperOre;
	static const Item diamondOre;
	static const Item oakLog;
	static const Item oakLeaves;
	static const Item cherryLog;
	static const Item cherryLeaves;
	static const Item craftingTable;
	static const Item ladder;
	static const Item coal;
	static const Item copperIngot;
	static const Item diamond;
	static const Item copperPick;
	static const Item diamondPick;

	static const Item& getById(int id);
	static const std::vector<const Item*>& getAllItems();

private:
	static const std::vector<const Item*> list;
};
