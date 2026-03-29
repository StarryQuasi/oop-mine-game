#include <print>

#include "Slider.h"

namespace gui
{
	Slider::Slider(float min, float max, float step, float value) :
		min(min),
		max(max),
		step(step),
		value(value)
	{
		handleContainer = addChild<Container>();
		handleContainer->setOrigin(Anchor::midLeft);
		handleContainer->setAnchor(Anchor::midLeft);
		handleContainer->setAssetName("gui/slider.png");
		handleContainer->setDebugName(std::format("slider {} inner", getId()));
		handleEle = handleContainer->addChild<Container>(olc::vi2d{ 4, 8 });
		handleEle->setAssetName("gui/slider_handle.png");
		handleEle->setDebugName(std::format("slider {} handle", getId()));
		handleEle->onMouseDown([](auto& _, const auto& __)
			{
				return true; // Return true to receive focus and the drag events
			});
		handleEle->onMouseDrag([this](auto& me, const auto& e)
			{
				return this->handleDragHandler(me, e);
			});
	}

	float Slider::getMin() const
	{
		return min;
	}

	float Slider::getMax() const
	{
		return max;
	}

	float Slider::getStep() const
	{
		return step;
	}

	float Slider::getValue() const
	{
		return value;
	}

	Slider* Slider::setMin(float v)
	{
		if (min != v)
			invalidate();
		min = v;
		commit(value);
		return this;
	}

	Slider* Slider::setMax(float v)
	{
		if (max != v)
			invalidate();
		max = v;
		commit(value);
		return this;
	}

	Slider* Slider::setStep(float v)
	{
		if (step != v)
			invalidate();
		step = v;
		commit(value);
		return this;
	}

	Slider* Slider::setValue(float v)
	{
		if (value != v)
		{
			float old = value;
			commit(v);
			if (old != value)
				invalidate();
		}
		return this;
	}

	void Slider::valueChanged(float old, float $new) const
	{
		if (valueChangedHandler)
			valueChangedHandler(old, $new);
	}

	Slider* Slider::onValueChanged(std::function<void(float, float)> handler)
	{
		valueChangedHandler = std::move(handler);
		return this;
	}

	void Slider::updateLayout()
	{
		validate();

		handleContainer->setSize({ getSize().x, handleEle->getSize().y });
		// It's actually width - 1, since [0, 1] * width-1 = [0, width-1],
		// I can't find a better name for it
		const int regionWidth = handleContainer->getSize().x - handleEle->getSize().x;
		const int handleX = (int)((value - min) / std::max(max - min, 0.000001f) * regionWidth);
		handleEle->setPos({ handleX, 0 });

		Container::updateLayout();
	}

	void Slider::commit(float newValue)
	{
		if (newValue < min)
			newValue = min;
		else if (newValue > max)
			newValue = max;
		else
			newValue = std::round((newValue - min) / step) * step + min;
		if (newValue != value)
		{
			valueChanged(value, newValue);
			// TODO: Remove debug line
			//std::println("value changed from {} to {}", value, newValue);
			value = newValue;
		}
	}

	bool Slider::handleDragHandler(Container& me, const MouseDragEvent& e)
	{
		const int regionWidth = handleContainer->getSize().x - handleEle->getSize().x;
		int newX = (e.globalPos - e.globalPosStart + e.elePosStart).x;
		newX = std::clamp(newX, 0, regionWidth);
		setValue((max - min) * ((float)newX / std::max(regionWidth, 1)) + min);
		return true;
	}
}
