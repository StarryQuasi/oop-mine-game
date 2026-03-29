#ifndef UTILS_H
#define UTILS_H

#include <ranges>

#include <olcPGEX_TransformedView.h>

class Utils
{
public:
	static void drawRectOutline(olc::TransformedView& view, const olc::vf2d& tl, const olc::vf2d& br, const olc::Pixel& pixel)
	{
		view.DrawLineDecal(tl, { br.x, tl.y }, pixel);
		view.DrawLineDecal({ br.x, tl.y }, br, pixel);
		view.DrawLineDecal(br, { tl.x, br.y }, pixel);
		view.DrawLineDecal({ tl.x, br.y }, tl, pixel);
	}

	static void drawDecalFlipped(olc::TileTransformedView& view, const olc::vf2d& pos, olc::Decal* decal, const olc::vf2d& scale = { 1.0f, 1.0f }, bool flipped = false, const olc::Pixel& tint = olc::WHITE)
	{
		if (flipped)
			view.DrawWarpedDecal(
				decal,
				{
					(pos + olc::vf2d{ decal->width * scale.x, 0.0f } / 32.0f),
					(pos + olc::vf2d{ decal->width * scale.x, decal->height * scale.y } / 32.0f),
					(pos + olc::vf2d{ 0.0f, decal->height * scale.y } / 32.0f),
					(pos),
				},
				tint);
		else
			view.DrawDecal(pos, decal, scale, tint);
	}
};

class Iterate
{
public:
	static auto over(const olc::vi2d& min, const olc::vi2d& max)
	{
		return
			std::views::cartesian_product(
				std::views::iota(min.y, max.y),
				std::views::iota(min.x, max.x))
			| std::views::transform([](std::tuple<int, int>&& tuple)
				{
					const auto [y, x] = tuple;
					return olc::vi2d{ x, y };
				});
	}

	static auto over(const olc::vf2d& min, const olc::vf2d& max)
	{
		return over((olc::vi2d)min.floor(), (olc::vi2d)max.ceil());
	}
};

#endif
