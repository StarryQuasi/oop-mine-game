#include <format>

#include <olcAabb.h>

#include "OopMineGame.h"
#include "Entity.h"
#include "Utils.h"

int Entity::entityIdCounter = 0;

Entity::Entity(olc::vf2d pos, olc::vf2d size, int maxInvSize) :
	id(entityIdCounter++),
	pos(pos),
	size(size),
	inv(maxInvSize, {}),
	maxInvSize(maxInvSize)
{}

int Entity::getId() const
{
	return id;
}

olc::vf2d Entity::getPos() const
{
	return pos;
}

float Entity::getX() const
{
	return pos.x;
}

float Entity::getY() const
{
	return pos.y;
}

void Entity::setX(float v)
{
	pos.x = v;
}

void Entity::setY(float v)
{
	pos.y = v;
}

olc::vf2d Entity::getVel() const
{
	return vel;
}

void Entity::setVel(olc::vf2d v)
{
	const float maxSpeed = 64;
	if (v.mag() > maxSpeed)
		v = v.norm() * maxSpeed;
	vel = v;
}

void Entity::setPos(olc::vf2d v)
{
	pos = v;
}

olc::vf2d Entity::getSize() const
{
	return size;
}

std::pair<olc::vf2d, olc::vf2d> Entity::getBb() const
{
	const olc::vf2d tl = { pos.x - size.x / 2.0f, pos.y - size.y };
	return { tl, tl + size };
}

std::pair<olc::vf2d, olc::vf2d> Entity::getBbAt(olc::vf2d pos) const
{
	const olc::vf2d tl = { pos.x - size.x / 2.0f, pos.y - size.y };
	return { tl, tl + size };
}

olc::vf2d Entity::getEyePos() const
{
	return pos + olc::vf2d{ 0, -size.y + size.y * 0.75f };
}

bool Entity::getDirection() const
{
	return direction;
}

bool Entity::isDead() const
{
	return dead;
}

void Entity::kill()
{
	dead = true;
}

bool Entity::isOnGround() const
{
	return onGround;
}

ItemStack Entity::getInvItem(int i)
{
	return i >= maxInvSize ? ItemStack() : inv[i];
}

void Entity::setInvItem(int i, ItemStack v)
{
	if (i < maxInvSize)
		inv[i] = v;
}

ItemStack Entity::addInvItem(ItemStack v)
{
	//v.validate();
	for (int i = 0; i < inv.size(); i++)
	{
		ItemStack& cur = inv[i];
		if (cur.getItem() == v.getItem())
		{
			//if (cur.getItem().get)
		}
	}
	return {};
}

void Entity::update(World& world, float elapsed)
{
	const olc::vf2d gravity = { 0.0f, 256.0f + 128.0f };
	//const olc::vf2d gravity = { 0.0f, 8.0f };
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
	olc::aabb::rect bbRect = { bb.first, size, vel };
	std::vector<olc::aabb::rect> rects;
	std::vector<std::pair<int, float>> z;
	rects.reserve(16);

	for (const auto& pos : Iterate::over(rangeMin, rangeMax))
	{
		if (!world.isValidPosition(pos) || world.isSolidBlock(pos))
		{
			olc::vf2d cp, cn;
			float t = 0;
			rects.push_back({ { (float)pos.x, (float)pos.y }, { 1.0f, 1.0f } });
			if (olc::aabb::DynamicRectVsRect(&bbRect, elapsed, rects.back(), cp, cn, t))
				z.push_back({ rects.size() - 1, t });
		}
	}
	//OopMineGame::debugMsg = std::format("blocks: {} collisions: {}", rects.size(), z.size());

	std::ranges::sort(z, [](const auto& a, const auto& b)
		{
			return a.second < b.second;
		});

	for (auto j : z)
		olc::aabb::ResolveDynamicRectVsRect(&bbRect, elapsed, &rects[j.first]);

	setPos({ bbRect.pos.x + size.x / 2.0f, bbRect.pos.y + size.y });
	setVel(bbRect.vel);
}

void Entity::checkOnGround(World& world)
{
	// TODO: Maybe this needs to only check for top edge collisions?
	onGround = false;
	auto bb = getBb();
	bb.first.y = bb.second.y;
	bb.second.y += 0.0001f;
	//OopMineGame::debugCallbacks.push_back([bb](OopMineGame& game)
	//	{
	//		Utils::drawRectOutline(game.getView(), bb.first.floor(), bb.second.ceil(), olc::CYAN);
	//	});
	for (const auto& pos : Iterate::over(bb.first, bb.second))
	{
		if (world.isSolidBlock(pos))
		{
			//OopMineGame::debugMsg = std::format("on ground");
			//OopMineGame::debugCallbacks.push_back([pos](OopMineGame& game)
			//	{
			//		Utils::drawRectOutline(game.getView(), pos, pos + olc::vf2d{ 1.0f, 1.0f }, olc::GREY);
			//	});
			onGround = true;
			break;
		}
	}
}
