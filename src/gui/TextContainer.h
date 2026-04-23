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
	float getScale() const;
	olc::Pixel getColor() const;

	TextContainer* setText(std::string v);
	TextContainer* setScale(float v);
	TextContainer* setColor(olc::Pixel v);

	void updateLayout() override;
	void draw(OopMineGame& game) const override;

private:
	static olc::vf2d defaultScale;
	std::string text{};
	float scale = 1.0f;
	olc::Pixel color = olc::WHITE;
};
} // namespace gui
