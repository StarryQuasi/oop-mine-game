#pragma once

#include "libs/olcPixelGameEngine.h"

#include "Container.h"

namespace gui
{
class TextContainer : public Container
{
public:
	TextContainer(Props props);

	std::string getText() const;
	olc::vf2d getScale() const;
	olc::Pixel getColor() const;

	TextContainer* setText(std::string v);
	TextContainer* setScale(olc::vf2d v);
	TextContainer* setColor(olc::Pixel v);

	void updateLayout() override;
	void draw(OopMineGame& game) const override;

private:
	std::string text = "";
	olc::vf2d scale = {0.4f, 0.5f};
	olc::Pixel color = olc::WHITE;
};
} // namespace gui
