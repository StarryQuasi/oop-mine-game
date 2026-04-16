#include "CraftingTable.h"
#include "Container.h"

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
	addChild<Container>(
		{.size = {{256, 256}}, .assetName = "gui/crafting_table.png"});
	// Consume all input
	onMouseDown([](Container&, const MouseEvent&) { return true; });
}
} // namespace gui