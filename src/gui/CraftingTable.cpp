#include "ItemStack.h"
#include "OopMineGame.h"
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
		.pos = {{56, 10}},
		.origin = Anchor::midMiddle,
		.text = "Crafting",
		.scale = 1.5f,
		.color = olc::Pixel(0xffefefef),
	});
	addChild<TextContainer>({
		.pos = {{33, 76}},
		.origin = Anchor::midMiddle,
		.text = "Inventory",
		.scale = 1.5f,
		.color = olc::Pixel(0xffefefef),
	});
	const olc::vi2d size{18, 18};
	olc::vi2d start{7, 141};
	using namespace std::placeholders;
	const auto onClick = std::bind(&CraftingTable::onSlotClick, this, _1, _2);
	for (int i = 0; i < 9; i++)
	{
		slots[i] = addChild<Slot>(
			{
				.size = size,
				.pos = {{start.x + size.x * i, start.y}},
			},
			player.getInvItem(i));
		slots[i]->onMouseDown(onClick);
	}
	start = {7, 83};
	for (int i = 0; i < 27; i++)
	{
		slots[9 + i] = addChild<Slot>(
			{
				.size = size,
				.pos = {{start.x + i % 9 * size.x, start.y + i / 9 * size.y}},
			},
			player.getInvItem(9 + i));
		slots[9 + i]->onMouseDown(onClick);
	}
	start = {29, 16};
	for (int i = 0; i < 9; i++)
	{
		slotsInput[i] = addChild<Slot>(
			{
				.size = size,
				.pos = {{start.x + i % 3 * size.x, start.y + i / 3 * size.y}},
			},
			stacksInput[i]);
		slotsInput[i]->onMouseDown(onClick);
	}
	slotsOutput =
		addChild<Slot>({.size = {{26, 26}}, .pos = {{119, 31}}}, stacksOutput);
	slotsOutput->onMouseDown(onClick);
	slotsOnHand = addChild<Slot>({.size = size}, stacksOnHand);
	// Consume all input
	onMouseDown([](Container&, const MouseEvent&) { return true; });
}

void CraftingTable::update(OopMineGame& game)
{
	Container::update(game);
	slotsOnHand->setPos(screenToLocal(game.GetMousePos()));
}

void CraftingTable::draw(OopMineGame& game) const { Container::draw(game); }

bool CraftingTable::onSlotClick(Container& me, const MouseEvent& event)
{
	Slot* slot = static_cast<Slot*>(&me);
	if (event.btn == olc::Mouse::LEFT)
	{
		ItemStack t = slot->getStack();
		slot->setStack(stacksOnHand.get());
		stacksOnHand.set(std::move(t));
		return true;
	}
	return false;
}
} // namespace gui