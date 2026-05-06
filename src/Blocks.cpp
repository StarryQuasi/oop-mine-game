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
const Block Blocks::coalBlock             = builder().name("Coal Block").item(Items::coalBlock).build();
const Block Blocks::copperBlock           = builder().name("Copper Block").item(Items::copperBlock).build();
const Block Blocks::diamondBlock          = builder().name("Diamond Block").item(Items::diamondBlock).build();
const Block Blocks::oakLog                = builder().name("Oak Log").item(Items::oakLog).build();
const Block Blocks::oakPlanks             = builder().name("Oak Planks").item(Items::oakPlanks).build();
const Leaves Blocks::oakLeaves            = builder().name("Oak Leaves").item(Items::oakLeaves).transparent(true).loot().probability(0.25f).item(Items::oakSapling).build<Leaves>();
const Sapling Blocks::oakSapling          = builder().name("Oak Sapling").item(Items::oakSapling).transparent(true).build<Sapling>();
const Block Blocks::cherryLog             = builder().name("Cherry Log").item(Items::cherryLog).build();
const Block Blocks::cherryPlanks          = builder().name("Cherry Planks").item(Items::cherryPlanks).build();
const Leaves Blocks::cherryLeaves         = builder().name("Cherry Leaves").item(Items::cherryLeaves).transparent(true).loot().probability(0.25f).item(Items::cherrySapling).build<Leaves>();
const Sapling Blocks::cherrySapling       = builder().name("Cherry Sapling").item(Items::cherrySapling).transparent(true).build<Sapling>();
const CraftingTable Blocks::craftingTable = builder().name("Crafting Table").item(Items::craftingTable).build<CraftingTable>();
const Block Blocks::ladder                = builder().name("Ladder").item(Items::ladder).build();
const Torch Blocks::torch                 = builder().name("Torch").item(Items::torch).transparent(true).build<Torch>();

const std::vector<const Block*> Blocks::list = {
	&Blocks::air,
	&Blocks::grassBlock,
	&Blocks::dirt,
	&Blocks::stone,
	&Blocks::cobblestone,
	&Blocks::coalOre,
	&Blocks::copperOre,
	&Blocks::diamondOre,
	&Blocks::coalBlock,
	&Blocks::copperBlock,
	&Blocks::diamondBlock,
	&Blocks::oakLog,
	&Blocks::oakPlanks,
	&Blocks::oakLeaves,
	&Blocks::oakSapling,
	&Blocks::cherryLog,
	&Blocks::cherryPlanks,
	&Blocks::cherryLeaves,
	&Blocks::cherrySapling,
	&Blocks::craftingTable,
	&Blocks::ladder,
	&Blocks::torch,
};
// clang-format on

const Block& Blocks::getById(int id)
{
	if (!Verify::index(id, list.size()))
		return air;
	return *list[id];
}

const std::vector<const Block*>& Blocks::getAllBlocks() { return list; }
