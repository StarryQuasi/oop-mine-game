#pragma once

#include <vector>

#include "libs/olcPixelGameEngine.h"

#include "Block.h"

class Data
{
public:
    struct TreeType
    {
        const Block* log{};
        const Block* planks{};
        const Leaves* leaves{};
        const Sapling* sapling{};
        const olc::Pixel color{};
        const std::vector<std::vector<std::vector<const Block*>>> patterns{};
    };

    static void init();
    static const std::vector<TreeType>& getTreeTypes();

private:
    static std::vector<TreeType> treeTypes;
};