#include "Blocks.h"
#include "Block.h"
#include "Items.h"
#include "Verify.h"

namespace
{
constexpr BlockBuilder builder() { return {}; }
} // namespace

// clang-format off
const Block Blocks::air        = builder().name("Air").item(Items::air).transparent(true).build();
const Block Blocks::grassBlock = builder().name("Grass Block").item(Items::grassBlock).build();
const Block Blocks::dirt       = builder().name("Dirt").item(Items::dirt).build();
const Block Blocks::stone      = builder().name("Stone").item(Items::stone).build();
const Block Blocks::coalOre    = builder().name("Coal Ore").item(Items::coalOre).build();
const Block Blocks::copperOre  = builder().name("Copper Ore").item(Items::copperOre).build();
const Block Blocks::diamondOre = builder().name("Diamond Ore").item(Items::diamondOre).build();
const Block Blocks::oakLog     = builder().name("Oak Log").item(Items::oakLog).build();
const Block Blocks::oakLeaves  = builder().name("Oak Leaves").item(Items::oakLeaves).transparent(true).build();
const Block Blocks::ladder     = builder().name("Ladder").item(Items::ladder).build();
// clang-format on

const std::vector<const Block*> Blocks::list = {
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

const std::vector<const Block*>& Blocks::getAllBlocks() { return list; }
