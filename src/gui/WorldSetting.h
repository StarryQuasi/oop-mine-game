#pragma once

#include "FlowContainer.h"
#include "World.h"

namespace gui
{
class WorldSetting : public FlowContainer
{
public:
	WorldSetting();

	void valueChanged() const;
	WorldSetting*
	onValueChanged(std::function<void(const GenerationSettings&)> handler);

private:
	GenerationSettings value = {};
	std::function<void(const GenerationSettings&)> valueChangedHandler;

	void addFloat(
		std::string name,
		float GenerationSettings::* p,
		float min,
		float max,
		float step,
		int labelPrecision = 2);
	void addFloatDouble(
		std::string name,
		float GenerationSettings::* p1,
		float GenerationSettings::* p2,
		float min,
		float max,
		float step,
		int labelPrecision = 2);
};
} // namespace gui
