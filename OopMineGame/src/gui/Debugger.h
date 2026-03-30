#pragma once

#include <format>

#include "FlowContainer.h"
#include "TextContainer.h"

class OopMineGame;

namespace gui
{
	class Debugger : public FlowContainer
	{
	public:
		Debugger(Container* root);

		void update(OopMineGame& game) override;

	private:
		Container* root;
		TextContainer* text;

		std::string format(olc::vi2d);
	};
}
