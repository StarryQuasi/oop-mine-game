#ifndef ITEMS_H
#define ITEMS_H

#include <unordered_map>
#include <string>

#include "Item.h"

class Items
{
public:
	static const Item air;
	static const Item grassBlock;
	static const Item dirt;
	static const Item stone;
	static const Item coalOre;
	static const Item copperOre;
	static const Item diamondOre;
	static const Item oakLog;
	static const Item oakLeaves;
	static const Item ladder;

	static const Item& getById(int id);
	static const std::vector<const Item*>& getAllItems();

private:
	static const std::vector<const Item*> list;
};

#endif
