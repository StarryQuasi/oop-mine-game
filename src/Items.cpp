#include "Items.h"
#include "Blocks.h"
#include "Item.h"
#include "Verify.h"

namespace
{
constexpr ItemBuilder builder() { return {}; }
} // namespace

// clang-format off
const Item Items::air            = builder().name("Air").block(Blocks::air).build();
const Item Items::grassBlock     = builder().name("Grass Block").block(Blocks::grassBlock).build();
const Item Items::dirt           = builder().name("Dirt").block(Blocks::dirt).build();
const Item Items::stone          = builder().name("Stone").block(Blocks::stone).build();
const Item Items::cobblestone    = builder().name("Cobblestone").block(Blocks::cobblestone).build();
const Item Items::coalOre        = builder().name("Coal Ore").block(Blocks::coalOre).build();
const Item Items::copperOre      = builder().name("Copper Ore").block(Blocks::copperOre).build();
const Item Items::diamondOre     = builder().name("Diamond Ore").block(Blocks::diamondOre).build();
const Item Items::coalBlock      = builder().name("Coal Block").block(Blocks::coalBlock).build();
const Item Items::copperBlock    = builder().name("Copper Block").block(Blocks::copperBlock).build();
const Item Items::diamondBlock   = builder().name("Diamond Block").block(Blocks::diamondOre).build();
const Item Items::oakLog         = builder().name("Oak Log").block(Blocks::oakLog).build();
const Item Items::oakPlanks      = builder().name("Oak Planks").block(Blocks::oakPlanks).build();
const Item Items::oakLeaves      = builder().name("Oak Leaves").block(Blocks::oakLeaves).build();
const Item Items::cherryLog      = builder().name("Cherry Log").block(Blocks::cherryLog).build();
const Item Items::cherryPlanks   = builder().name("Cherry Planks").block(Blocks::cherryPlanks).build();
const Item Items::cherryLeaves   = builder().name("Cherry Leaves").block(Blocks::cherryLeaves).build();
const Item Items::craftingTable  = builder().name("Crafting Table").block(Blocks::craftingTable).build();
const Item Items::ladder         = builder().name("Ladder").block(Blocks::ladder).build();
const Item Items::coal           = builder().name("Coal").build();
const Item Items::copperIngot    = builder().name("Copper Ingot").build();
const Item Items::diamond        = builder().name("Diamond").build();
const Item Items::stick          = builder().name("Stick").build();
const Item Items::woodenSpear    = builder().name("Wooden Spear").maxStackSize(1).durability(32).tier(1).build();
const Item Items::stoneSpear     = builder().name("Stone Spear").maxStackSize(1).durability(64).tier(2).build();
const Item Items::copperSpear    = builder().name("Copper Spear").maxStackSize(1).durability(64).tier(2).build();
const Item Items::diamondSpear   = builder().name("Diamond Spear").maxStackSize(1).durability(256).tier(3).build();

const std::vector<const Item*> Items::list = {
	&Items::air,
	&Items::grassBlock,
	&Items::dirt,
	&Items::stone,
	&Items::cobblestone,
	&Items::coalOre,
	&Items::copperOre,
	&Items::diamondOre,
	&Items::coalBlock,
	&Items::copperBlock,
	&Items::diamondBlock,
	&Items::oakLog,
	&Items::oakPlanks,
	&Items::oakLeaves,
	&Items::cherryLog,
	&Items::cherryPlanks,
	&Items::cherryLeaves,
	&Items::craftingTable,
	&Items::ladder,
	&Items::coal,
	&Items::copperIngot,
	&Items::diamond,
	&Items::stick,
	&Items::woodenSpear,
	&Items::stoneSpear,
	&Items::copperSpear,
	&Items::diamondSpear,
};
// clang-format on

const Item& Items::getById(int id)
{
	if (!Verify::index(id, list.size()))
		return air;
	return *list[id];
}

const std::vector<const Item*>& Items::getAllItems() { return list; }
