#include "Blocks.h"
#include "Items.h"
#include "Verify.h"

const TransparentBlock Blocks::air("Air", &Items::air);
const Block Blocks::grassBlock("Grass Block", &Items::grassBlock);
const Block Blocks::dirt("Dirt", &Items::dirt);
const Block Blocks::stone("Stone", &Items::stone);
const Block Blocks::coalOre("Coal Ore", &Items::coalOre);
const Block Blocks::copperOre("Copper Ore", &Items::copperOre);
const Block Blocks::diamondOre("Diamond Ore", &Items::diamondOre);
const Block Blocks::oakLog("Oak Log", &Items::oakLog);
const TransparentBlock Blocks::oakLeaves("Oak Leaves", &Items::oakLeaves);
const Block Blocks::ladder("Ladder", &Items::ladder);

const std::vector<const Block*> Blocks::list =
{
	&Blocks::air,
	&Blocks::grassBlock,
	&Blocks::dirt,
	&Blocks::stone,
	&Blocks::coalOre,
	&Blocks::copperOre,
	&Blocks::diamondOre,
	&Blocks::oakLog,
	&Blocks::oakLeaves,
	&Blocks::ladder,
};

const Block& Blocks::getById(int id)
{
	if (!Verify::index(id, list.size()))
		return air;
	return *list[id];
}

const std::vector<const Block*>& Blocks::getAllBlocks()
{
	return list;
}
