#include <cassert>
#include <unordered_map>
#include <utility>

#include "libs/olcPixelGameEngine.h"

#include "Recipes.h"
#include "Utils.h"

std::vector<Recipes::Recipe> Recipes::recipes{};

void Recipes::init()
{
	const auto addRecipe =
		[](const std::initializer_list<std::pair<char, const ItemStack>>
			   mappings,
		   const ItemStack& result,
		   const std::initializer_list<std::string_view> pattern)
	{
		std::vector<std::vector<ItemStack>> res;
		std::unordered_map<char, ItemStack> map;
		map[' '] = {};
		for (const auto& [c, stack] : mappings)
			map[c] = stack;
		for (const auto& row : pattern)
		{
			std::vector<ItemStack> resultRow;
			for (const char c : row)
				resultRow.push_back(map.at(c));
			res.push_back(std::move(resultRow));
			assert(res.back().size() == res.front().size());
		}
		recipes.push_back({
			{(int)res[0].size(), (int)res.size()},
			std::move(res),
			std::move(result),
		});
	};
	addRecipe({{'w', {Items::oakLog, 1}}}, {Items::oakPlanks, 4}, {"w"});
	addRecipe({{'w', {Items::cherryLog, 1}}}, {Items::cherryPlanks, 4}, {"w"});
	addRecipe({{'x', {Items::oakPlanks, 1}}}, {Items::stick, 4}, {"x", "x"});
	addRecipe({{'x', {Items::cherryPlanks, 1}}}, {Items::stick, 4}, {"x", "x"});
	addRecipe(
		{{'x', {Items::oakPlanks, 1}}, {'s', {Items::stick, 1}}},
		{Items::woodenSpear, 4},
		{"x", "s", "s"});
	addRecipe(
		{{'x', {Items::cherryPlanks, 1}}, {'s', {Items::stick, 1}}},
		{Items::woodenSpear, 4},
		{"x", "s", "s"});
	addRecipe(
		{{'x', {Items::cobblestone, 1}}, {'s', {Items::stick, 1}}},
		{Items::stoneSpear, 4},
		{"x", "s", "s"});
	addRecipe(
		{{'x', {Items::copperIngot, 1}}, {'s', {Items::stick, 1}}},
		{Items::copperSpear, 4},
		{"x", "s", "s"});
	addRecipe(
		{{'x', {Items::diamond, 1}}, {'s', {Items::stick, 1}}},
		{Items::diamondSpear, 4},
		{"x", "s", "s"});
	addRecipe(
		{{'x', {Items::coal, 1}}},
		{Items::coalBlock, 1},
		{"xxx", "xxx", "xxx"});
	addRecipe(
		{{'x', {Items::copperIngot, 1}}},
		{Items::copperBlock, 1},
		{"xxx", "xxx", "xxx"});
	addRecipe(
		{{'x', {Items::diamond, 1}}},
		{Items::diamondBlock, 1},
		{"xxx", "xxx", "xxx"});
}

std::optional<std::pair<olc::vi2d, olc::vi2d>>
Recipes::findBounds(const std::vector<std::vector<ItemStack>>& input)
{
	std::pair<olc::vi2d, olc::vi2d> bounds{{-1, -1}, {-1, -1}};
	bool init = false;
	for (const olc::vi2d p :
		 Iterate::over(olc::vi2d{}, {(int)input[0].size(), (int)input.size()}))
	{
		if (input[p.y][p.x].isEmpty())
			continue;
		if (!init)
			bounds = {p, p};
		init = true;
		bounds.first.x = std::min(bounds.first.x, p.x);
		bounds.first.y = std::min(bounds.first.y, p.y);
		bounds.second.x = std::max(bounds.second.x, p.x);
		bounds.second.y = std::max(bounds.second.y, p.y);
	}
	if (!init)
		return {};
	return bounds;
}

std::optional<const Recipes::Recipe*>
Recipes::findRecipe(const std::vector<std::vector<ItemStack>>& input)
{
	const auto bounds = findBounds(input);

	if (!bounds.has_value())
		return {};
	for (const auto& recipe : recipes)
	{
		if (matchesRecipe(input, bounds.value(), recipe))
			return &recipe;
	}
	return {};
}

bool Recipes::matchesRecipe(
	const std::vector<std::vector<ItemStack>>& input,
	const std::pair<olc::vi2d, olc::vi2d> bounds,
	const Recipe& recipe)
{
	if (bounds.second.x - bounds.first.x + 1 != recipe.size.x ||
		bounds.second.y - bounds.first.y + 1 != recipe.size.y)
		return false;
	for (const olc::vi2d p : Iterate::over(bounds.first, bounds.second + 1))
	{
		const ItemStack pat =
			recipe.pattern[p.y - bounds.first.y][p.x - bounds.first.x];
		if (input[p.y][p.x].getItem() != pat.getItem())
			return false;
		if (input[p.y][p.x].getCount() < pat.getCount())
			return false;
	}
	return true;
}
