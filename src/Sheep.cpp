#include "Sheep.h"
#include "OopMineGame.h"
#include "Utils.h"

Sheep::Sheep(olc::vf2d pos) :
	Mob(pos, {0.8f, 0.8f})
{
	setWalkSpeed(64);
}

void Sheep::updateAi(World& world, float elapsed)
{
	const auto& player = world.getPlayer()->get();
	const float dist = (player.getPos() - getPos()).mag();
	if (dist <= 16 && dist >= 3.0f)
	{
		setTargetPos(player.getPos());
	}
	else
	{
		setTargetPos({});
	}
	Mob::updateAi(world, elapsed);
}

void Sheep::update(World& world, float elapsed) { Mob::update(world, elapsed); }

void Sheep::draw(OopMineGame& game) const
{
	const auto assetSize = olc::vf2d{1, 1} * 1.5f;
	Utils::drawDecalFlipped(
		game.getView(),
		getPos() - olc::vf2d{assetSize.x / 2.0f, assetSize.y},
		game.getAsset("entity/sheep/sheep.png")->Decal(),
		assetSize,
		getDirection());
	Mob::draw(game);
}
