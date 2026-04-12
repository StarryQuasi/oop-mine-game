#include <cassert>

#include "Anchor.h"
#include "Slider.h"
#include "TextContainer.h"
#include "WorldSetting.h"

namespace gui
{
WorldSetting::WorldSetting(Props props) :
	FlowContainer(props)
{
	FlowContainer::setDirection(gui::Direction::vertical);
	addFloat(
		"freq", &GenerationSettings::noiseFrequency, 0.001f, 0.10f, 0.001f, 3);
	addFloat(
		"diamond", &GenerationSettings::diamondThreshold, 0.0f, 1.0f, 0.01f, 2);
	addFloatDouble(
		"copper",
		&GenerationSettings::copperThresholdMin,
		&GenerationSettings::copperThresholdMax,
		0.0f,
		1.0f,
		0.01f,
		2);
}

void WorldSetting::valueChanged() const { valueChangedHandler(value); }

WorldSetting* WorldSetting::onValueChanged(
	std::function<void(const GenerationSettings&)> handler)
{
	valueChangedHandler = std::move(handler);
	return this;
}

void WorldSetting::addFloat(
	std::string name,
	float GenerationSettings::* p,
	float min,
	float max,
	float step,
	int labelPrecision)
{
	assert(min <= max);
	const float def = GenerationSettings().*p;
	auto flow = addChild<FlowContainer>();
	flow->addChild<TextContainer>({
		.anchor = Anchor::midLeft,
		.origin = Anchor::midLeft,
		.text = std::move(name),
	});
	auto slider = flow->addChild<Slider>(
		{
			.size = {{50, 8}},
			.anchor = Anchor::midLeft,
			.origin = Anchor::midLeft,
		},
		min,
		max,
		step,
		def);
	auto label = flow->addChild<TextContainer>({
		.anchor = Anchor::midLeft,
		.origin = Anchor::midLeft,
		.text = std::format("{:.{}f}", def, labelPrecision),
	});

	slider->onValueChanged(
		[this, p, label, def, labelPrecision](float old, float $new)
		{
			this->value.*p = $new;
			label->setText(std::format("{:.{}f}", $new, labelPrecision));
			this->valueChanged();
		});
}

void WorldSetting::addFloatDouble(
	std::string name,
	float GenerationSettings::* p1,
	float GenerationSettings::* p2,
	float min,
	float max,
	float step,
	int labelPrecision)
{
	assert(min <= max);
	const float def1 = GenerationSettings().*p1;
	const float def2 = GenerationSettings().*p2;
	assert(def1 <= def2);

	auto flow1 = addChild<FlowContainer>();
	flow1->addChild<TextContainer>({
		.anchor = Anchor::midLeft,
		.origin = Anchor::midLeft,
		.text = name,
	});
	auto slider1 = flow1->addChild<Slider>(
		{
			.size = {{50, 8}},
			.anchor = Anchor::midLeft,
			.origin = Anchor::midLeft,
		},
		min,
		def2,
		step,
		def1);
	auto label1 = flow1->addChild<TextContainer>({
		.anchor = Anchor::midLeft,
		.origin = Anchor::midLeft,
		.text = std::format("{:.{}f}", def1, labelPrecision),
	});

	auto flow2 = addChild<FlowContainer>();
	flow2->addChild<TextContainer>({
		.anchor = Anchor::midLeft,
		.origin = Anchor::midLeft,
		.text = name,
	});
	auto slider2 = flow2->addChild<Slider>(
		{
			.size = {{50, 8}},
			.anchor = Anchor::midLeft,
			.origin = Anchor::midLeft,
		},
		def1,
		max,
		step,
		def2);
	auto label2 = flow2->addChild<TextContainer>({
		.anchor = Anchor::midLeft,
		.origin = Anchor::midLeft,
		.text = std::format("{:.{}f}", def2, labelPrecision),
	});

	slider1->onValueChanged(
		[this, p1, label1, def1, labelPrecision, slider2](float old, float $new)
		{
			this->value.*p1 = $new;
			label1->setText(std::format("{:.{}f}", $new, labelPrecision));
			if (std::abs($new - slider2->getMin()) > 0.00001f)
				slider2->setMin($new);
			this->valueChanged();
		});
	slider2->onValueChanged(
		[this, p2, label2, def2, labelPrecision, slider1](float old, float $new)
		{
			this->value.*p2 = $new;
			label2->setText(std::format("{:.{}f}", $new, labelPrecision));
			if (std::abs($new - slider1->getMax()) > 0.00001f)
				slider1->setMax($new);
			this->valueChanged();
		});
}
} // namespace gui
