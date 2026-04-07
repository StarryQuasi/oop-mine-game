#include "Sheep.h"
#include "OopMineGame.h"
#include "Utils.h"

Sheep::Sheep(olc::vf2d pos) :
	Entity(pos, {0.8f, 0.8f})
{
}

void Sheep::update(World& world, float elapsed)
{
	Entity::update(world, elapsed);
}

void Sheep::draw(OopMineGame& game) const
{
	const auto assetSize = olc::vf2d{1, 1} * 1.5f;
	Utils::drawDecalFlipped(
		game.getView(),
		getPos() - olc::vf2d{assetSize.x / 2.0f, assetSize.y},
		game.getAsset("entity/sheep/sheep.png")->Decal(),
		assetSize,
		getDirection());
	Entity::draw(game);
}
