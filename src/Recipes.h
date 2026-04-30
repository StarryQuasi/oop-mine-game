#pragma once

#include <optional>
#include <vector>

#include "libs/olcPixelGameEngine.h"

#include "ItemStack.h"

class Recipes
{
public:
	struct Recipe
	{
		const olc::vi2d size{};
		const std::vector<std::vector<ItemStack>> pattern{};
		const ItemStack result{};
	};

	static void init();

	// Returns the inclusive bounds of the content area in the input
	static std::optional<std::pair<olc::vi2d, olc::vi2d>>
	findBounds(const std::vector<std::vector<ItemStack>>& input);

	// Finds the matching recipe for a arbitrary sized input
	static std::optional<const Recipe*>
	findRecipe(const std::vector<std::vector<ItemStack>>& input);

private:
	static std::vector<Recipe> recipes;

	static bool matchesRecipe(
		const std::vector<std::vector<ItemStack>>& input,
		const std::pair<olc::vi2d, olc::vi2d> bounds,
		const Recipe& recipe);
};