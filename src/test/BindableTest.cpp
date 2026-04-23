#include <print>

#include "Bindable.h"

#define RUN(code)                                                              \
	std::println("> " #code);                                                  \
	code

int main()
{
	Bindable<int> p1{};
	p1.onValueChanged([](const auto& from, const auto& to)
					  { std::println("p1 {} -> {}", from, to); });
	{
		Bindable<int> p2{};
		p2.onValueChanged([](const auto& from, const auto& to)
						  { std::println("p2 {} -> {}", from, to); });
		RUN(p2.bindTo(p1));
		RUN(p1.set(1));
		RUN(p2.set(2));
		assert(p1.get() == 2);
		assert(p2.get() == 2);
		std::println("p1 is {}", p1.get());
		std::println("p2 is {}", p2.get());
	}
	RUN(p1.set(3));
	std::println("p1 is {}", p1.get());
	{
		Bindable<int> p2;
		p2.onValueChanged([](const auto& from, const auto& to)
						  { std::println("p2 {} -> {}", from, to); });
		RUN(p2.bindTo(p1));
		RUN(p1.set(4));
		RUN(p2.set(5));
		assert(p1.get() == 5);
		assert(p2.get() == 5);
		std::println("p1 is {}", p1.get());
		std::println("p2 is {}", p2.get());
	}
	return 0;
}