#include "libs/olcAabb.h"

#include "Bindable.h"
#include "Blocks.h"
#include "Entity.h"
#include "ItemStack.h"
#include "OopMineGame.h"
#include "Utils.h"
#include "Verify.h"

int Entity::entityIdCounter = 0;

Entity::Entity(olc::vf2d pos, olc::vf2d size, int maxInvSize) :
	id(entityIdCounter++),
	pos(pos),
	size(size),
	inv(maxInvSize),
	maxInvSize(maxInvSize)
{
}

int Entity::getId() const { return id; }

olc::vf2d Entity::getPos() const { return pos; }

float Entity::getX() const { return pos.x; }

float Entity::getY() const { return pos.y; }

Entity* Entity::setX(float v)
{
	pos.x = v;
	return this;
}

Entity* Entity::setY(float v)
{
	pos.y = v;
	return this;
}

olc::vf2d Entity::getVel() const { return vel; }

Entity::Input Entity::getInput() const { return input; }

float Entity::getWalkSpeed() const { return walkSpeed; }

Entity* Entity::setVel(olc::vf2d v)
{
	const float maxSpeed = 64;
	if (v.mag() > maxSpeed)
		v = v.norm() * maxSpeed;
	vel = v;
	return this;
}

Entity* Entity::setInput(Input v)
{
	input = v;
	return this;
}

Entity* Entity::setWalkSpeed(float v)
{
	walkSpeed = v;
	return this;
}

Entity* Entity::setPos(olc::vf2d v)
{
	pos = v;
	return this;
}

olc::vf2d Entity::getSize() const { return size; }

std::pair<olc::vf2d, olc::vf2d> Entity::getBb() const
{
	const olc::vf2d tl = {pos.x - size.x / 2.0f, pos.y - size.y};
	return {tl, tl + size};
}

std::pair<olc::vf2d, olc::vf2d> Entity::getBbAt(olc::vf2d p) const
{
	const olc::vf2d tl = {p.x - size.x / 2.0f, p.y - size.y};
	return {tl, tl + size};
}

olc::vf2d Entity::getEyePos() const
{
	return pos + olc::vf2d{0, -size.y + size.y * 0.75f};
}

bool Entity::getDirection() const { return direction; }

bool Entity::isDead() const { return dead; }

void Entity::kill() { dead = true; }

bool Entity::isOnGround() const { return onGround; }

Bindable<ItemStack>& Entity::getInvItem(int i)
{
	assert(Verify::index(i, maxInvSize));
	return inv[i];
}

ItemStack Entity::addInvItem(ItemStack v)
{
	if (v.isEmpty())
		return {};
	bool fillingExisting = true;
	for (int _ = 0; _ < 2; _++)
	{
		for (int i = 0; i < inv.size(); i++)
		{
			const ItemStack& cur = inv[i].get();
			if (fillingExisting ? (cur.isSameType(v) && cur.canAdd(v))
								: cur.canAdd(v))
			{
				const int toAdd = std::min(
					v.getItem().getMaxStackSize() - cur.getCount(),
					v.getCount());
				inv[i].set(v.copy().setCount(inv[i]->getCount() + toAdd));
				v.decrease(toAdd);
				if (v.isEmpty())
					return {};
			}
		}
		fillingExisting = false;
	}
	return v;
}

void Entity::updateInput(World& world, float elapsed)
{
	const float reach = 6;
	if ((input.target - getEyePos()).mag2() <= reach * reach)
	{
		const olc::vi2d target = input.target.floor();
		if (world.isValidPosition(target))
		{
			if (input.attack)
			{
				const Block& block = world.getBlock(target);
				ItemStack stack = {block.getItem(), 1};
				world.breakBlock(target);
			}
			if (input.use)
			{
				const Block& block =
					getInvItem(input.invSelection)->getItem().getBlock();
				if (block != Blocks::air &&
					world.getBlock(target).isReplaceable())
				{
					getInvItem(input.invSelection)
						.set(getInvItem(input.invSelection)->copy().decrease());
					world.setBlock(target, block);
				}
			}
		}
	}

	olc::vf2d vel = getVel();
	const float acceleration = (input.sprint ? 1.35f : 1.0f) * getWalkSpeed();
	if (input.left)
		vel.x -= acceleration * elapsed;
	if (input.right)
		vel.x += acceleration * elapsed;
	if (input.jump && isOnGround() &&
		std::chrono::steady_clock::now() - lastJumpTime >=
			std::chrono::milliseconds(250))
	{
		lastJumpTime = std::chrono::steady_clock::now();
		vel.y -= 256 * 1024 * 1.5f * elapsed;
	}
	setVel(vel);

	input = {};
}

void Entity::update(World& world, float elapsed)
{
	updateInput(world, elapsed);
	const olc::vf2d gravity = {0.0f, 256.0f + 128.0f};
	// const olc::vf2d gravity = { 0.0f, 8.0f };
	vel += gravity * elapsed;
	handleCollisions(world, elapsed);
	pos += vel * elapsed;
	vel *= 1.0f - std::min(40.0f * elapsed, 1.0f);
	direction = vel.x > 0.01f ? true : vel.x < -0.01f ? false : direction;
	checkOnGround(world);
	// Walk particles
	if (isOnGround() && std::abs(vel.x) >= 0.5f)
	{
		const olc::vi2d standingOn = {
			(int)getPos().x, (int)(getPos().y + 0.00001f)};
		if (world.isValidPosition(standingOn))
		{
			const auto& assetOpt = world.getGame().getBlockAssetPatch(
				world.getBlock(standingOn).getId());
			if (assetOpt.has_value())
			{
				const auto now = std::chrono::steady_clock::now();
				const olc::vf2d velRange = {0.3f, 0.4f};
				const int dir = getDirection() ? 1 : -1;
				const auto& asset = assetOpt.value();
				for (int _ = 0; _ < 4; _++)
				{
					olc::Pixel color = asset.decal->sprite->GetPixel(
						(int)(world.randomFloat(
								  asset.coords[1].x, asset.coords[2].x) *
							  asset.decal->width),
						(int)(world.randomFloat(
								  asset.coords[1].y, asset.coords[0].y) *
							  asset.decal->height));
					world.addParticle({
						.lifeStart = now,
						.lifeEnd = now + std::chrono::milliseconds(
											 world.randomInt(100, 500)),
						.pos =
							{world.randomFloat(
								 getBb().first.x, getBb().second.x),
							 getPos().y},
						.vel =
							{-dir * world.randomFloat(0.0f, velRange.x),
							 world.randomFloat(-velRange.y, velRange.y)},
						.color = color,
						.scale = world.randomFloat(2.0f, 4.0f),
						.alive = true,
						.type = ParticleType::generic,
					});
				}
			}
		}
	}
}

void Entity::draw(OopMineGame& game) const
{
	const auto [tl, br] = getBb();
	Utils::drawRectOutline(game.getView(), tl, br, olc::RED);
}

void Entity::handleCollisions(World& world, float elapsed)
{
	const auto bb = getBb();
	const auto bbNew = getBbAt(pos + vel * elapsed);
	const olc::vf2d rangeMin = bbNew.first.min(bb.first) - size / 2;
	const olc::vf2d rangeMax = bbNew.second.max(bb.second) + size / 2;
	olc::aabb::rect bbRect = {bb.first, size, vel};
	std::vector<olc::aabb::rect> rects;
	std::vector<std::pair<int, float>> z;
	rects.reserve(16);

	for (const auto& pos : Iterate::over(rangeMin, rangeMax))
	{
		if (!world.isValidPosition(pos) || world.isSolidBlock(pos))
		{
			olc::vf2d cp, cn;
			float t = 0;
			rects.push_back({{(float)pos.x, (float)pos.y}, {1.0f, 1.0f}});
			if (olc::aabb::DynamicRectVsRect(
					&bbRect, elapsed, rects.back(), cp, cn, t))
				z.push_back({rects.size() - 1, t});
		}
	}
	// OopMineGame::debugMsg = std::format("blocks: {} collisions: {}",
	// rects.size(), z.size());

	std::ranges::sort(
		z, [](const auto& a, const auto& b) { return a.second < b.second; });

	for (auto j : z)
		olc::aabb::ResolveDynamicRectVsRect(&bbRect, elapsed, &rects[j.first]);

	setPos({bbRect.pos.x + size.x / 2.0f, bbRect.pos.y + size.y});
	setVel(bbRect.vel);
}

void Entity::checkOnGround(World& world)
{
	// TODO: Maybe this needs to only check for top edge collisions?
	onGround = false;
	auto bb = getBb();
	bb.first.y = bb.second.y;
	bb.second.y += 0.00001f;
	for (const auto& pos : Iterate::over(bb.first, bb.second))
	{
		if (!world.isValidPosition(pos) || world.isSolidBlock(pos))
		{
			onGround = true;
			break;
		}
	}
}
