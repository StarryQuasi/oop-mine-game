#include "Items.h"
#include "Blocks.h"
#include "Verify.h"

const Item Items::air("Air", &Blocks::air);
const Item Items::grassBlock("Grass Block", &Blocks::grassBlock);
const Item Items::dirt("Dirt", &Blocks::dirt);
const Item Items::stone("Stone", &Blocks::stone);
const Item Items::coalOre("Coal Ore", &Blocks::coalOre);
const Item Items::copperOre("Copper Ore", &Blocks::copperOre);
const Item Items::diamondOre("Diamond Ore", &Blocks::diamondOre);
const Item Items::oakLog("Oak Log", &Blocks::oakLog);
const Item Items::oakLeaves("Oak Leaves", &Blocks::oakLeaves);
const Item Items::ladder("Ladder", &Blocks::ladder);

const std::vector<const Item*> Items::list =
{
	&Items::air,
	&Items::grassBlock,
	&Items::dirt,
	&Items::stone,
	&Items::coalOre,
	&Items::copperOre,
	&Items::diamondOre,
	&Items::oakLog,
	&Items::oakLeaves,
	&Items::ladder,
};

const Item& Items::getById(int id)
{
	if (!Verify::index(id, list.size()))
		return air;
	return *list[id];
}

const std::vector<const Item*>& Items::getAllItems()
{
	return list;
}
