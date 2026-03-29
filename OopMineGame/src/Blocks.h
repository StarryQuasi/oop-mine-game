#ifndef BLOCKS_H
#define BLOCKS_H

#include <unordered_map>
#include <string>

#include "Block.h"

class Blocks
{
public:
	static const TransparentBlock air;
	static const Block grassBlock;
	static const Block dirt;
	static const Block stone;
	static const Block coalOre;
	static const Block copperOre;
	static const Block diamondOre;
	static const Block oakLog;
	static const TransparentBlock oakLeaves;
	static const Block ladder;

	static const Block& getById(int id);
	static const std::vector<const Block*>& getAllBlocks();

private:
	static const std::vector<const Block*> list;
};

#endif
