#pragma once

#include <vector>

#include "Block.h"

class Blocks
{
public:
	static const Block air;
	static const Block grassBlock;
	static const Block dirt;
	static const Block stone;
	static const Block cobblestone;
	static const Block coalOre;
	static const Block copperOre;
	static const Block diamondOre;
	static const Block coalBlock;
	static const Block copperBlock;
	static const Block diamondBlock;
	static const Block oakLog;
	static const Block oakPlanks;
	static const Leaves oakLeaves;
	static const Sapling oakSapling;
	static const Block cherryLog;
	static const Block cherryPlanks;
	static const Leaves cherryLeaves;
	static const Sapling cherrySapling;
	static const CraftingTable craftingTable;
	static const Block ladder;
	static const Torch torch;

	static const Block& getById(int id);
	static const std::vector<const Block*>& getAllBlocks();

private:
	static const std::vector<const Block*> list;
};
