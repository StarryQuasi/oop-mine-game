#include "TextContainer.h"
#include "OopMineGame.h"

namespace gui
{
olc::vf2d TextContainer::defaultScale = {0.4f, 0.5f};

TextContainer::TextContainer(Props props) :
	Container(props)
{
	if (props.text.has_value())
		text = props.text.value();
	if (props.scale.has_value())
		scale = props.scale.value();
	if (props.color.has_value())
		color = props.color.value();
}

std::string TextContainer::getText() const { return text; }

float TextContainer::getScale() const { return scale; }

TextContainer* TextContainer::setText(std::string v)
{
	if (text != v)
		invalidate();
	text = std::move(v);
	return this;
}

TextContainer* TextContainer::setScale(float v)
{
	if (scale != v)
		invalidate();
	scale = v;
	return this;
}

TextContainer* TextContainer::setColor(olc::Pixel v)
{
	color = v;
	return this;
}

void TextContainer::updateLayout()
{
	validate();

	setSize((OopMineGame::instance->GetTextSize(text) * defaultScale * scale).ceil());

	Container::updateLayout();
}

void TextContainer::draw(OopMineGame& game) const
{
	Container::draw(game);
	game.DrawStringDecal(getAbsolutePos(), text, color, defaultScale * scale);
}
} // namespace gui
