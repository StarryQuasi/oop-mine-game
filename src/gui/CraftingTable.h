#pragma once

#include "Player.h"
#include "gui/Container.h"

namespace gui
{
    class CraftingTable : public Container
    {
    public:
    CraftingTable(Player& player);

    private:
        Player* player;
    };
}