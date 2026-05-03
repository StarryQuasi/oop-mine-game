#include "DroppedItem.h"
#include "Blocks.h"
#include "Entity.h"
#include "ItemStack.h"
#include "OopMineGame.h"
#include "Utils.h"
#include <chrono>
#include <numbers>

DroppedItem::DroppedItem(olc::vf2d pos, ItemStack stack) :
	Entity(pos, {0.2f, 0.2f}),
	stack(std::move(stack)),
	drawTimeStart(std::chrono::steady_clock::now())
{
}

ItemStack DroppedItem::getStack() const { return stack; }

void DroppedItem::draw(OopMineGame& game) const
{
	const bool isBlock = stack.getItem().getBlock() != Blocks::air;
	const auto& decalPatch =
		isBlock
			? game.getBlockAssetPatch(stack.getItem().getBlock().getId())
			: game.getAsset("item/" + stack.getItem().getTextureName() + ".png")
				  .transform([](const olc::Renderable& r)
							 { return (olc::DecalPatch)*r.Decal(); });
	if (decalPatch.has_value())
	{
		const auto drawTime = std::chrono::steady_clock::now();
		auto drawPos = getPos() - olc::vf2d{getSize().x / 2, getSize().y};
		const float yes =
			std::chrono::duration_cast<std::chrono::duration<float>>(
				drawTime - drawTimeStart)
				.count();
		drawPos.y += -0.1f + std::sin(yes * (float)std::numbers::pi) * 0.1f;
		Utils::drawDecalPatch(
			game.getView(), drawPos, decalPatch.value(), getSize(), game.getWorld().getTimeTint());
	}
	Entity::draw(game);
}