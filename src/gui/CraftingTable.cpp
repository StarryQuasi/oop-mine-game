#include "OopMineGame.h"
#include "gui/FlowContainer.h"
#include "libs/olcPixelGameEngine.h"

#include "Container.h"
#include "CraftingTable.h"
#include "gui/TextContainer.h"

namespace gui
{
CraftingTable::CraftingTable(Player& player) :
	Container(
		Props{
			.size = {{176, 166}},
			.anchor = Anchor::midMiddle,
			.origin = Anchor::midMiddle,
		}),
	player(&player)
{
	// Oversized texture so put it in another container
	addChild<Container>({
		.size = {{256, 256}},
		.assetName = "gui/crafting_table.png",
	});
	addChild<TextContainer>({
		.pos = {{55, 10}},
		.origin = Anchor::midMiddle,
		.text = "Crafting Table",
		.color = olc::Pixel(0xffefefef),
	});
	addChild<TextContainer>({
		.pos = {{33, 76}},
		.origin = Anchor::midMiddle,
		.text = "Inventory",
		.color = olc::Pixel(0xffefefef),
	});
	// 8 86 8 144
	{
		auto* flow = addChild<FlowContainer>({.pos = {{8, 144}}});
		for (int i = 0; i < 9; i++)
		{
			slots[i] = flow->addChild<Slot>({}, player.getInvItem(i));
		}
	}
	// Consume all input
	onMouseDown([](Container&, const MouseEvent&) { return true; });
}

void CraftingTable::update(OopMineGame& game) {}
} // namespace gui