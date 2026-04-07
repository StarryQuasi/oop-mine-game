#include <format>

#include <olcAabb.h>

#include "Blocks.h"
#include "Entity.h"
#include "OopMineGame.h"
#include "Utils.h"
#include "Verify.h"

int Entity::entityIdCounter = 0;

Entity::Entity(olc::vf2d pos, olc::vf2d size, int maxInvSize) :
	id(entityIdCounter++),
	pos(pos),
	size(size),
	inv(maxInvSize, ItemStack{}),
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

ItemStack Entity::getInvItem(int i)
{
	assert(Verify::index(i, maxInvSize));
	return inv[i];
}

void Entity::setInvItem(int i, const ItemStack& v)
{
	assert(Verify::index(i, maxInvSize));
	inv[i] = v;
}

ItemStack Entity::addInvItem(ItemStack v)
{
	if (v.isEmpty())
		return {};
	if (v.getItem().getMaxStackSize() > 1)
	{
		for (int i = 0; i < inv.size(); i++)
		{
			ItemStack& cur = inv[i];
			if (cur.isEmpty() ||
				(cur.getItem() == v.getItem() &&
				 cur.getDamage() == v.getDamage() &&
				 cur.getCount() < v.getItem().getMaxStackSize()))
			{
				const int toAdd = std::min(
					v.getItem().getMaxStackSize() - cur.getCount(),
					v.getCount());
				cur = ItemStack(
					v.getItem(), cur.getCount() + toAdd, v.getDamage());
				v.setCount(v.getCount() - toAdd);
				if (v.isEmpty())
					break;
			}
		}
	}
	else
	{
		for (int i = 0; i < inv.size(); i++)
		{
			ItemStack& cur = inv[i];
			if (cur.isEmpty())
			{
				cur = v;
				return {};
			}
		}
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
				addInvItem(stack);
				world.setBlock(target, Blocks::air);
			}
			if (input.use)
			{
				const Block& block =
					getInvItem(input.invSelection).getItem().getBlock();
				if (block != Blocks::air &&
					world.getBlock(target).isReplaceable())
				{
					setInvItem(
						input.invSelection,
						getInvItem(input.invSelection).decrease());
					world.setBlock(target, block);
				}
			}
		}
	}

	olc::vf2d vel = getVel();
	const float acceleration = (input.sprint ? 1.35f : 1.0f) * (256 + 64);
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
	// OopMineGame::debugCallbacks.push_back([bb](OopMineGame& game)
	//	{
	//		Utils::drawRectOutline(game.getView(), bb.first.floor(),
	// bb.second.ceil(), olc::CYAN);
	//	});
	for (const auto& pos : Iterate::over(bb.first, bb.second))
	{
		if (world.isValidPosition(pos) && world.isSolidBlock(pos))
		{
			// OopMineGame::debugMsg = std::format("on ground");
			// OopMineGame::debugCallbacks.push_back([pos](OopMineGame& game)
			//	{
			//		Utils::drawRectOutline(game.getView(), pos, pos +
			// olc::vf2d{ 1.0f, 1.0f }, olc::GREY);
			//	});
			onGround = true;
			break;
		}
	}
}
