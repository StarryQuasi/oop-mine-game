#pragma once

#include <functional>

#include "Container.h"

namespace gui
{
class Slider : public Container
{
public:
	Slider(Props props, float min, float max, float step, float value);

	float getMin() const;
	float getMax() const;
	float getStep() const;
	float getValue() const;

	Slider* setMin(float v);
	Slider* setMax(float v);
	Slider* setStep(float v);
	Slider* setValue(float v);

	void valueChanged(float old, float $new) const;
	Slider* onValueChanged(std::function<void(float, float)> handler);

	void updateLayout() override;

private:
	float min;
	float max;
	float step;
	float value;
	Container* handleContainer;
	Container* handleEle;
	std::function<void(float, float)> valueChangedHandler = {};

	void commit(float newValue);
	bool handleDragHandler(Container& me, const MouseDragEvent& e);
};
} // namespace gui
