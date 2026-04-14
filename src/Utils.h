#pragma once

#include "Entity.h"
#include <ranges>

#include "libs/olcPGEX_TransformedView.h"

class Utils
{
public:
	static void drawRectOutline(
		olc::TransformedView& view,
		const olc::vf2d& tl,
		const olc::vf2d& br,
		const olc::Pixel& pixel)
	{
		view.DrawLineDecal(tl, {br.x, tl.y}, pixel);
		view.DrawLineDecal({br.x, tl.y}, br, pixel);
		view.DrawLineDecal(br, {tl.x, br.y}, pixel);
		view.DrawLineDecal({tl.x, br.y}, tl, pixel);
	}

	static void drawDecalFlipped(
		olc::TileTransformedView& view,
		const olc::vf2d& pos,
		olc::Decal* decal,
		const olc::vf2d& scale = {1.0f, 1.0f},
		bool flipped = false,
		const olc::Pixel& tint = olc::WHITE)
	{
		if (flipped)
			view.DrawWarpedDecal(
				decal,
				{
					(pos + olc::vf2d{decal->width * scale.x, 0.0f} / 32.0f),
					(pos +
					 olc::vf2d{
						 decal->width * scale.x, decal->height * scale.y} /
						 32.0f),
					(pos + olc::vf2d{0.0f, decal->height * scale.y} / 32.0f),
					(pos),
				},
				tint);
		else
			view.DrawDecal(pos, decal, scale, tint);
	}

	static void drawDecalPatch(
		olc::TileTransformedView& view,
		const olc::vf2d& pos,
		const olc::DecalPatch& patch,
		const olc::vf2d& size)
	{
		const olc::vf2d sourceSize =
			olc::vf2d{
				patch.coords[2].x - patch.coords[1].x,
				patch.coords[0].y - patch.coords[1].y} /
			patch.decal->vUVScale;
		view.DrawPartialDecal(
			pos,
			size * olc::vi2d{32, 32},
			patch.decal,
			patch.coords[1] / patch.decal->vUVScale,
			sourceSize);
	}

	static std::string toString(const olc::vi2d& v)
	{
		return std::format("({},{})", v.x, v.y);
	}

	static std::string toString(const olc::vf2d& v)
	{
		return std::format("({:.3f},{:.3f})", v.x, v.y);
	}

	static int manhattanDist(olc::vi2d a, olc::vi2d b)
	{
		const olc::vi2d diff = b - a;
		return std::abs(diff.x) + std::abs(diff.y);
	}
};

class Iterate
{
public:
	static auto over(const olc::vi2d& min, const olc::vi2d& max)
	{
		return std::views::iota(min.y, max.y) |
			   std::views::transform(
				   [min, max](int y)
				   {
					   return std::views::iota(min.x, max.x) |
							  std::views::transform(
								  [y](int x) { return olc::vi2d{x, y}; });
				   }) |
			   std::views::join;
	}

	static auto over(const olc::vf2d& min, const olc::vf2d& max)
	{
		return over((olc::vi2d)min.floor(), (olc::vi2d)max.ceil());
	}
};

struct vi2dHash
{
	std::size_t operator()(const olc::vi2d& v) const
	{
		return std::hash<int>()(v.x) ^ std::hash<int>()(v.y);
	}
};
