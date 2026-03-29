#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <functional>
#include <chrono>

class Transform
{
public:
	Transform(
		float current,
		float change,
		std::chrono::milliseconds duration,
		std::function<float(float)> easer,
		std::function<void(float)> setter) :
		start(current),
		end(current + change),
		startTime(std::chrono::steady_clock::now()),
		endTime(startTime + duration),
		easer(std::move(easer)),
		setter(std::move(setter))
	{}
	void update() const
	{
		const auto now = std::chrono::steady_clock::now();
		if (now >= endTime)
		{
			setter(end);
			return;
		}
		const float t = std::chrono::duration<float>(now - startTime).count() / std::chrono::duration<float>(endTime - startTime).count();
		const float easedT = easer(t);
		const float newValue = start + (end - start) * easedT;
		setter(newValue);
	}
	bool isFinished() const
	{
		return std::chrono::steady_clock::now() >= endTime;
	}

	class Easing
	{
	public:
		static float linear(float v) { return v; }
		static float outQuart(float v) { return 1 - std::powf(1 - v, 4); }
	};

private:
	float start;
	float end;
	std::chrono::steady_clock::time_point startTime;
	std::chrono::steady_clock::time_point endTime;
	std::function<float(float)> easer;
	std::function<void(float)> setter;
};

#endif
