#include <Cafe/Misc/Utility.h>
#include <catch2/catch.hpp>
#include <iostream>

using namespace Cafe;
using namespace Core;
using namespace Misc;

namespace
{
	struct Foo
	{
		operator int() const noexcept
		{
			return 3;
		}
	};
} // namespace

TEST_CASE("Cafe.Core.Misc.Utility", "[Core][Misc][Utility]")
{
	SECTION("RuntimeGet")
	{
		std::tuple tup{ 1, 2.0, Foo{} };
		auto visitor = [i = 3](auto&& item) mutable { REQUIRE(i-- == static_cast<int>(item)); };
		for (std::size_t i = std::tuple_size_v<decltype(tup)>; i-- > 0;)
		{
			std::cout << i << ": ";
			RuntimeGet(i, tup, visitor);
		}
	}
}
