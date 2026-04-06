#include "OopMineGame.h"
#include "Player.h"
#include "Utils.h"

Player::Player(olc::vf2d pos) :
	Entity(pos, olc::vf2d{ 0.9f, 1.5f }, 36),
	hairPoints(4, olc::vf2d{})
{
	hairBuffer.Create(32, 32);
}

void Player::update(World& world, float elapsed)
{
	const olc::vf2d lastPos = getPos();
	Entity::update(world, elapsed);
	const olc::vf2d posOffset = getPos() - lastPos;

	// Modified from https://www.youtube.com/watch?v=imkT4kFP43k
	const olc::vf2d hairGravity = olc::vf2d{ 3.0f * (getDirection() ? -1 : 1), 4.5f } * 3.0f;
	const float lerpSpeed = 5.0f;
	const float maxDist = 2.0f;

	hairPoints[0] = {};

	for (int i = 1; i < hairPoints.size(); i++)
	{
		olc::vf2d& p = hairPoints[i];
		const olc::vf2d target = hairPoints[(size_t)i - 1];

		p -= posOffset * 3;

		p += hairGravity * elapsed;

		const olc::vf2d vec = p - target;
		const float dist = vec.mag();
		if (dist > maxDist)
			p = target + vec.norm() * maxDist;

		p = p.lerp(target, 1.0f - std::pow(0.1f, lerpSpeed * elapsed));
	}
	// Verlet integration test
	//for (int i = 1; i < hairPoints.size(); i++)
	//{
	//	olc::vf2d& p = hairPoints[i];
	//	olc::vf2d& pold = hairPointsOld[i];

	//	const olc::vf2d pcopy = p;
	//	p = p * 2 - pold + elapsed * elapsed * (hairGravity + getVel()) * 10;
	//	pold = pcopy;
	//}
	//const float maxDist = 3.0f;
	//for (int _ = 0; _ < 3; _++)
	//{
	//	for (int i = 1; i < hairPoints.size(); i++)
	//	{
	//		olc::vf2d& p1 = hairPoints[i-1];
	//		olc::vf2d& p2 = hairPoints[i];
	//		const olc::vf2d dir = (p2 - p1).norm();
	//		const float dist = (p2 - p1).mag() - maxDist;
	//		p1 += dist * dir / 2.0f;
	//		p2 -= dist * dir / 2.0f;
	//	}
	//}
}

void Player::draw(OopMineGame& game) const
{
	const olc::Pixel color = olc::Pixel(0xe8, 0x64, 0x94);

	game.SetDrawTarget(hairBuffer.Sprite());
	std::memset(hairBuffer.Sprite()->pColData.data(), 0, hairBuffer.Sprite()->Size().area() * sizeof(olc::Pixel));

	float currentRadius = 6.0f;
	for (int i = 0; i < hairPoints.size(); i++)
	{
		game.FillCircle(hairPoints[i] + getHairOrigin(), currentRadius, color);
		currentRadius *= 0.9f;
	}

	// TODO: DISGUSTING!!!
	game.SetDrawTarget((uint8_t)0);

	hairBuffer.Decal()->Update();

	Utils::drawDecalFlipped(
		game.getView(),
		getPos() - olc::vf2d{ 1.0f, 2.0f },
		hairBuffer.Decal(),
		{ 2, 2 },
		false);
	Utils::drawDecalFlipped(
		game.getView(),
		getPos() - olc::vf2d{ 1.0f, 2.0f },
		game.getAsset("entity/player/player_no_hair.png")->Decal(),
		{ 2, 2 },
		getDirection());
	//Utils::drawRectOutline(
	//	game.getView(),
	//	getPos() - olc::vf2d{ 1.0f, 2.0f },
	//	getPos() - olc::vf2d{ 1.0f, 2.0f } + olc::vf2d{ 32.0f, 32.0f } * 2 / 32,
	//	olc::WHITE);

	Entity::draw(game);
}

olc::vf2d Player::getHairOrigin() const
{
	return olc::vf2d{ 16, 11 };
}
