#ifndef GUI_TEXTCONTAINER_H
#define GUI_TEXTCONTAINER_H

#include "FlowContainer.h"

namespace gui
{
	class TextContainer : public Container
	{
	public:
		TextContainer(
			const std::string& text,
			olc::vf2d scale = { 0.4f, 0.5f });

		std::string getText() const;
		olc::vf2d getScale() const;

		TextContainer* setText(std::string v);
		TextContainer* setScale(olc::vf2d v);

		void updateLayout() override;
		void draw(OopMineGame& game) const override;

	private:
		std::string text;
		olc::vf2d scale;
	};
}

#endif
