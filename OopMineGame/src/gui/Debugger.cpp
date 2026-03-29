#include "Debugger.h"
#include "../OopMineGame.h"

namespace gui
{
	Debugger::Debugger(Container* root) :
		FlowContainer(),
		root(root)
	{
		text = addChild<TextContainer>("");
		text->setDebugName("debugger text");
	}

	void Debugger::update(OopMineGame& game)
	{
		// TODO: Implement std::formatter for max rizz
		std::string s = "";
		const auto mouse = game.GetMousePos();
		Container* hovered = root->searchElement(mouse);
		s += std::format("hovered: \n");
		if (hovered)
		{
			s.pop_back();
			s += std::format("{}\n", hovered->getId());
			s += std::format("  {}\n", typeid(*hovered).name());
			if (!hovered->getDebugName().empty())
				s += std::format("  ({})\n", hovered->getDebugName());
		}
		s += std::format("pos: ");
		if (hovered)
			s += format(hovered->getPos());
		s += '\n';
		s += std::format("size: ");
		if (hovered)
			s += format(hovered->getSize());
		s += '\n';
		s += std::format("draw size: ");
		if (hovered)
			s += format(hovered->getDrawSize());
		s += '\n';
		s += std::format("margin: ");
		if (hovered)
			s += format(hovered->getMargin());
		s += '\n';
		s += std::format("padding: ");
		if (hovered)
			s += format(hovered->getPadding());
		s += '\n';
		s += std::format("anchor: ");
		if (hovered)
			s += std::format("{}", hovered->getAnchor().toString());
		s += '\n';
		s += std::format("origin: ");
		if (hovered)
			s += std::format("{}", hovered->getOrigin().toString());
		s += '\n';
		s += std::format("hovered pos: ");
		if (hovered)
			s += format(hovered->screenToLocal(mouse));
		s += '\n';
		text->setText(std::move(s));
	}

	std::string Debugger::format(olc::vi2d v)
	{
		return std::format("{}, {}", v.x, v.y);
	}
}
