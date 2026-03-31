#include "Transform.h"

Transform::Transform(
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

void Transform::update(const std::chrono::steady_clock::time_point now)
{
	if (now >= endTime)
	{
		ended = true;
		setter(end);
		return;
	}
	const float t = std::chrono::duration<float>(now - startTime).count() / std::chrono::duration<float>(endTime - startTime).count();
	const float easedT = easer(t);
	const float newValue = start + (end - start) * easedT;
	setter(newValue);
}

bool Transform::isFinished() const
{
	return ended;
}
