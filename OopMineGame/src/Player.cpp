#include "OopMineGame.h"
#include "Player.h"
#include "Utils.h"

Player::Player(olc::vf2d pos) :
	Entity(pos, olc::vf2d{ 0.9f, 1.5f }, 36),
	hairPoints(8, getHairOrigin())
{}

void Player::update(World& world, float elapsed)
{
	Entity::update(world, elapsed);

	// https://www.youtube.com/watch?v=imkT4kFP43k
	const olc::vf2d hairGravity = olc::vf2d{ 1.0f, 1.25f } * (getDirection() ? olc::vf2d{ -1,1 } : olc::vf2d{ 1,1 });
	const float lerpSpeed = 12.5f;
	const float maxDistance = 0.07f;

	for (int i = 0; i < hairPoints.size(); i++)
	{
		olc::vf2d& p = hairPoints[i];
		const olc::vf2d target = (i == 0) ? getHairOrigin() : hairPoints[(size_t)i - 1];

		p += hairGravity * elapsed;

		const olc::vf2d vec = p - target;
		float currentDist = vec.mag();

		if (currentDist > maxDistance)
			p = target + vec.norm() * maxDistance;

		p = p.lerp(target, 1.0f - std::pow(0.1f, lerpSpeed * elapsed));
	}
}

void Player::draw(OopMineGame& game) const
{
	const olc::Pixel color = olc::Pixel(0xe8, 0x64, 0x94);
	const olc::vf2d hairOrigin = getHairOrigin();

	float currentRadius = 15.0f / 32.0f;
	//game.getView().FillCircle(hairOrigin, currentRadius, olc::BLACK);
	//for (int i = 0; i < hairPoints.size(); i++)
	//{
	//	currentRadius *= 0.8f;
	//	game.getView().FillCircle(hairPoints[i], currentRadius, olc::BLACK);
	//}

	currentRadius = 12.0f / 32.0f;
	game.getView().FillCircle(hairOrigin, currentRadius, color);
	for (int i = 0; i < hairPoints.size(); i++)
	{
		//currentRadius *= 0.95f;
		currentRadius *= 0.925f;
		game.getView().FillCircle(hairPoints[i], currentRadius, color);
	}

	Utils::drawDecalFlipped(
		game.getView(),
		getPos() - olc::vf2d{ 1.0f, 2.0f },
		game.getAsset("entity/player/player_no_hair.png")->Decal(),
		{ 2, 2 },
		getDirection());
	Utils::drawRectOutline(
		game.getView(),
		getPos() - olc::vf2d{ 1.0f, 2.0f },
		getPos() - olc::vf2d{ 1.0f, 2.0f } + olc::vf2d{ 32.0f, 32.0f } * 2 / 32,
		olc::WHITE);

	Entity::draw(game);
}

olc::vf2d Player::getHairOrigin() const
{
	return getPos() - olc::vf2d{ 1.0f, 2.0f } + olc::vf2d{ 16 / 32.0f, 11 / 32.0f } * 2;
}
