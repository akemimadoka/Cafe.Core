#ifndef CAFE_CORE_MISC_MATH_H
#define CAFE_CORE_MISC_MATH_H

#include "TypeTraits.h"
#include <cassert>

namespace Cafe::Core::Misc::Math
{
	/// @brief  计算乘幂
	/// @remark 仅适用于 exp 是非负整数的情况
	template <typename TBase, typename TExp>
	constexpr std::enable_if_t<std::is_integral_v<TExp>, TBase> Pow(TBase base, TExp exp) noexcept
	{
		if (exp == 0)
		{
			return TBase{ 1 };
		}

		assert(exp > 0);

		TBase result{ 1 };

		while (exp > 1)
		{
			if ((exp & 1) == 1)
			{
				result *= base;
			}
			exp /= 2;
			base *= base;
		}

		if (exp == 1)
		{
			result *= base;
		}

		return result;
	}
} // namespace Cafe::Core::Misc::Math

#endif
