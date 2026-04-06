#pragma once

#include <cassert> // Include assert() for users of this class

#include <olcPixelGameEngine.h>

class Verify
{
public:
	template<typename T>
	static bool in(const T& v, const T& a, const T& b)
	{
		return v >= a && v <= b;
	}

	template<typename T>
	static bool inExclusive(const T& v, const T& a, const T& b)
	{
		return v >= a && v < b;
	}

	template<>
	bool in<olc::vi2d>(const olc::vi2d& v, const olc::vi2d& a, const olc::vi2d& b)
	{
		return in(v.x, a.x, b.x) && in(v.y, a.y, b.y);
	}

	template<>
	bool in<olc::vf2d>(const olc::vf2d& v, const olc::vf2d& a, const olc::vf2d& b)
	{
		return in(v.x, a.x, b.x) && in(v.y, a.y, b.y);
	}

	template<>
	bool inExclusive<olc::vi2d>(const olc::vi2d& v, const olc::vi2d& a, const olc::vi2d& b)
	{
		return inExclusive(v.x, a.x, b.x) && inExclusive(v.y, a.y, b.y);
	}

	template<>
	bool inExclusive<olc::vf2d>(const olc::vf2d& v, const olc::vf2d& a, const olc::vf2d& b)
	{
		return inExclusive(v.x, a.x, b.x) && inExclusive(v.y, a.y, b.y);
	}

	template<typename T, typename TSize> requires std::is_arithmetic_v<T>&& std::is_arithmetic_v<TSize>
	static bool index(const T& v, const TSize& size)
	{
		return inExclusive(v, 0, (T)size);
	}

	// Pass a comparator object like std::greater_equal{} which will be invoked using cmp(v.e, a) on every element
	template<typename T, typename C>
	static bool elems(const olc::v_2d<T>& v, const C& cmp, const T& a)
	{
		for (const T& e : v.a())
			if (!cmp(e, a))
				return false;
		return true;
	}

	template<typename T>
	static bool elemsNonNeg(const olc::v_2d<T>& v)
	{
		return elems(v, std::greater_equal{}, 0);
	}
};
