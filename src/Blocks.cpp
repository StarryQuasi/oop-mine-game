#include "Blocks.h"
#include "Block.h"
#include "Items.h"
#include "Verify.h"

namespace
{
constexpr BlockBuilder builder() { return {}; }
} // namespace

// clang-format off
const Block Blocks::air                   = builder().name("Air").item(Items::air).transparent(true).build();
const Block Blocks::grassBlock            = builder().name("Grass Block").item(Items::grassBlock).loot().item(Items::dirt).build();
const Block Blocks::dirt                  = builder().name("Dirt").item(Items::dirt).build();
const Block Blocks::stone                 = builder().name("Stone").item(Items::stone).loot().item(Items::cobblestone).build();
const Block Blocks::cobblestone           = builder().name("Cobblestone").item(Items::cobblestone).build();
const Block Blocks::coalOre               = builder().name("Coal Ore").item(Items::coalOre).loot().min(0).item(Items::cobblestone).loot().max(3).item(Items::coal).build();
const Block Blocks::copperOre             = builder().name("Copper Ore").item(Items::copperOre).loot().min(0).item(Items::cobblestone).loot().max(3).item(Items::copperIngot).build();
const Block Blocks::diamondOre            = builder().name("Diamond Ore").item(Items::diamondOre).loot().min(0).item(Items::cobblestone).loot().item(Items::diamond).build();
const Block Blocks::oakLog                = builder().name("Oak Log").item(Items::oakLog).build();
const Block Blocks::oakLeaves             = builder().name("Oak Leaves").item(Items::oakLeaves).transparent(true).build();
const Block Blocks::cherryLog             = builder().name("Cherry Log").item(Items::cherryLog).build();
const Block Blocks::cherryLeaves          = builder().name("Cherry Leaves").item(Items::cherryLeaves).transparent(true).build();
const CraftingTable Blocks::craftingTable = builder().name("Crafting Table").item(Items::craftingTable).build<CraftingTable>();
const Block Blocks::ladder                = builder().name("Ladder").item(Items::ladder).build();
// clang-format on

const std::vector<const Block*> Blocks::list = {
	&Blocks::air,
	&Blocks::grassBlock,
	&Blocks::dirt,
	&Blocks::stone,
	&Blocks::cobblestone,
	&Blocks::coalOre,
	&Blocks::copperOre,
	&Blocks::diamondOre,
	&Blocks::oakLog,
	&Blocks::oakLeaves,
	&Blocks::cherryLog,
	&Blocks::cherryLeaves,
	&Blocks::oakLog,
	&Blocks::oakLeaves,
	&Blocks::craftingTable,
	&Blocks::ladder,
};

const Block& Blocks::getById(int id)
{
	if (!Verify::index(id, list.size()))
		return air;
	return *list[id];
}

const std::vector<const Block*>& Blocks::getAllBlocks() { return list; }
