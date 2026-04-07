#pragma once

#include <chrono>
#include <functional>

class Transform
{
public:
	Transform(
		float current,
		float change,
		std::chrono::milliseconds duration,
		std::function<float(float)> easer,
		std::function<void(float)> setter);

	void update(
		const std::chrono::steady_clock::time_point now =
			std::chrono::steady_clock::now());

	bool isFinished() const;

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
	bool ended = false;
};
