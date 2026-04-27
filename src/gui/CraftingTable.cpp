#include "libs/olcPixelGameEngine.h"

#include "Container.h"
#include "CraftingTable.h"
#include "ItemStack.h"
#include "OopMineGame.h"
#include "Recipes.h"
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
	for (int i = 0; i < 9; i++)
	{
		slots[i] = addChild<Slot>(
			{
				.size = size,
				.pos = {{start.x + size.x * i, start.y}},
			},
			player.getInvItem(i));
		slots[i]->onMouseDown(
			std::bind(&CraftingTable::onSlotClick, this, _1, _2, 0, i));
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
		slots[9 + i]->onMouseDown(
			std::bind(&CraftingTable::onSlotClick, this, _1, _2, 0, 9 + i));
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
		slotsInput[i]->onMouseDown(
			std::bind(&CraftingTable::onSlotClick, this, _1, _2, 1, i));
	}
	slotsOutput =
		addChild<Slot>({.size = {{26, 26}}, .pos = {{119, 31}}}, stacksOutput);
	slotsOutput->onMouseDown(
		std::bind(&CraftingTable::onSlotClick, this, _1, _2, 2, 0));
	slotsOnHand = addChild<Slot>({.size = size}, stacksOnHand);
	// Consume all input
	onMouseDown([](Container&, const MouseEvent&) { return true; });
}

void CraftingTable::update(OopMineGame& game)
{
	Container::update(game);
	slotsOnHand->setPos(screenToLocal(game.GetMousePos()));
	if (requiresUpdate)
	{
		requiresUpdate = false;
		auto res = Recipes::findRecipe(getInputStacks());
		if (res)
		{
			stacksOutput.set((*res)->result);
		}
		else
		{
			stacksOutput.set({});
		}
	}
}

void CraftingTable::draw(OopMineGame& game) const { Container::draw(game); }

std::vector<std::vector<ItemStack>> CraftingTable::getInputStacks() const
{
	std::vector<std::vector<ItemStack>> res(3, std::vector<ItemStack>(3));
	for (int i = 0; i < 9; i++)
		res[i / 3][i % 3] = stacksInput[i].get();
	return std::move(res);
}

bool CraftingTable::onSlotClick(
	Container& me, const MouseEvent& event, const int type, const int index)
{
	Slot* slot = static_cast<Slot*>(&me);
	switch (type)
	{
	case 0:
	case 1:
		if (event.btn == olc::Mouse::LEFT)
		{
			if (slot->getStack().isSameType(stacksOnHand.get()))
			{
				const int toAdd = std::min(
					slot->getStack().getItem().getMaxStackSize() -
						slot->getStack().getCount(),
					stacksOnHand->getCount());
				if (toAdd > 0)
				{
					slot->setStack(slot->getStack().copy().increase(toAdd));
					stacksOnHand.set(stacksOnHand->copy().decrease(toAdd));
				}
			}
			else
			{
				ItemStack t = slot->getStack();
				slot->setStack(stacksOnHand.get());
				stacksOnHand.set(std::move(t));
			}
			if (type == 1)
				requiresUpdate = true;
		}
		else if (event.btn == olc::Mouse::RIGHT)
		{
			if (stacksOnHand->isEmpty())
			{
				const int half = slot->getStack().getCount() / 2;
				stacksOnHand.set(slot->getStack().copy().setCount(
					slot->getStack().getCount() - half));
				slot->setStack(slot->getStack().copy().setCount(half));
			}
			else
			{
				if (slot->getStack().isEmpty())
				{
					slot->setStack(stacksOnHand->copy().setCount(1));
					stacksOnHand.set(stacksOnHand->copy().decrease());
				}
				else if (slot->getStack().isSameType(stacksOnHand.get()))
				{
					slot->setStack(slot->getStack().copy().increase());
					stacksOnHand.set(stacksOnHand->copy().decrease());
				}
				else
				{
					ItemStack t = slot->getStack();
					slot->setStack(stacksOnHand.get());
					stacksOnHand.set(std::move(t));
				}
			}
			if (type == 1)
				requiresUpdate = true;
		}
		break;
	case 2:
		if (event.btn == olc::Mouse::LEFT || event.btn == olc::Mouse::RIGHT)
		{
			if (tryCraftAndTakeResult())
				requiresUpdate = true;
		}
		break;
	}
	return true;
}

bool CraftingTable::tryCraftAndTakeResult()
{
	const auto input = getInputStacks();
	const auto res = Recipes::findRecipe(input);
	if (!res.has_value())
		return false;
	if (!stacksOnHand.get().canAddAll(res.value()->result))
		return false;
	const auto bounds = Recipes::findBounds(input).value();
	for (int y = bounds.first.y; y <= bounds.second.y; y++)
	{
		for (int x = bounds.first.x; x <= bounds.second.x; x++)
		{
			int i = y * 3 + x;
			const auto& pat =
				res.value()->pattern[y - bounds.first.y][x - bounds.first.x];
			if (pat.isEmpty())
				continue;
			stacksInput[i].set(
				stacksInput[i].get().copy().decrease(pat.getCount()));
		}
	}
	if (stacksOnHand.get().isEmpty())
		stacksOnHand.set(res.value()->result);
	else
		stacksOnHand.set(
			stacksOnHand.get().copy().increase(res.value()->result.getCount()));
	return true;
}
} // namespace gui