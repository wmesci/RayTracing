#pragma once
#include <math.h>

const double e = 2.7182818284590452354;
const double pi = 3.14159265358979323846;
const double epsilon = 2.2204460492503131E-16;

#undef min
#undef max

template<typename L, typename R>
inline auto min(const L& l, const R& r) -> decltype(l<r ? l : r) { return l<r ? l : r; }

template<typename L, typename R>
inline auto max(const L& l, const R& r) -> decltype(l>r ? l : r) { return l>r ? l : r; }

template<typename Tr, typename Tmin, typename Tmax>
Tr clamp(const Tr& value, const Tmin& min, const Tmax& max)
{
	if (value > max)
		return max;
	if (value < min)
		return min;
	return value;
}

inline bool is_zero(double value)
{
	return value < 1E-10 && value > -1E-10;
}